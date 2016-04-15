#include "sfmserverrelayer.h"

SfMServerRelayer::SfMServerRelayer()
{
    this->serverSocket = new QTcpSocket();
    this->connect(this->serverSocket, SIGNAL(disconnected()), this, SLOT(onSfMServerDisconnected()));
    this->connect(this->serverSocket, SIGNAL(readyRead()), this, SLOT(onSfMServerReadyRead()));

    this->clientSocket = new QTcpSocket();
    this->connect(this->clientSocket, SIGNAL(disconnected()), this, SLOT(onClientDisconnected()));
    this->connect(this->clientSocket, SIGNAL(readyRead()), this, SLOT(onClientSocketReadyRead()));
}

SfMServerRelayer::~SfMServerRelayer()
{
    this->serverSocket->deleteLater();
    this->clientSocket->deleteLater();
}

bool SfMServerRelayer::startRelayer(qintptr serverSocketDescriptor)
{
    if (this->serverSocket->setSocketDescriptor(serverSocketDescriptor))
    {
        this->updateStatus(Started);
        return true;
    }
    else
    {
        this->updateStatus(Dead);
        return false;
    }
}

bool SfMServerRelayer::startRelaying(qintptr clientSocketDes)
{
    if (this->clientSocket->setSocketDescriptor(clientSocketDes))
    {
        this->updateStatus(Relaying);
        return true;
    }
    else
    {
        this->updateStatus(Dead);
        return false;
    }

    return true;
}

SfMServerRelayer::Status SfMServerRelayer::getStatus() const
{
    return this->status;
}

bool SfMServerRelayer::isDead() const
{
    return this->status == Dead;
}

void SfMServerRelayer::onSfMServerDisconnected()
{
    this->updateStatus(Dead);
}

void SfMServerRelayer::onSfMServerReadyRead()
{
    //just relay the data
    if (this->status == Relaying)
    {
        try {
            QByteArray byteArray = this->serverSocket->readAll();
            this->clientSocket->write(byteArray);
            this->clientSocket->flush();
        } catch (...) {
            this->updateStatus(Dead);
        }

    }
    else
        this->updateStatus(Dead);
}

void SfMServerRelayer::onClientDisconnected()
{
    this->updateStatus(Dead);
}

void SfMServerRelayer::onClientSocketReadyRead()
{
    if (this->status == Relaying)
    {
        try {
            QByteArray byteArray = this->clientSocket->readAll();
            this->serverSocket->write(byteArray);
            this->serverSocket->flush();
        } catch (...) {
            this->updateStatus(Dead);
        }
    }
    else
        this->updateStatus(Dead);
}

void SfMServerRelayer::updateStatus(SfMServerRelayer::Status mstatus)
{
    if (mstatus == this->status)
        return;

    this->status = mstatus;

    //close both the serverSocket when in closed mode
    if (this->status == Dead)
    {
        this->serverSocket->close();
        this->clientSocket->close();
    }

    emit statusUpdated();
}

