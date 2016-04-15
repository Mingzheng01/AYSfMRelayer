#include "relayer.h"
#include <functional>

Relayer::Relayer()
{
    std::function<void(QTcpSocket*)> socketDeleter = [](QTcpSocket *socket){
        socket->deleteLater();
    };

    this->serverSocket = std::shared_ptr<QTcpSocket>(new QTcpSocket(), socketDeleter);
    this->connect(this->serverSocket.get(), SIGNAL(disconnected()), this, SLOT(onSfMServerDisconnected()));
    this->connect(this->serverSocket.get(), SIGNAL(readyRead()), this, SLOT(onSfMServerReadyRead()));

    this->clientSocket = std::shared_ptr<QTcpSocket>(new QTcpSocket(), socketDeleter);
    this->connect(this->clientSocket.get(), SIGNAL(disconnected()), this, SLOT(onClientDisconnected()));
    this->connect(this->clientSocket.get(), SIGNAL(readyRead()), this, SLOT(onClientSocketReadyRead()));
}

Relayer::~Relayer()
{
}

bool Relayer::startRelayer(qintptr serverSocketDescriptor)
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

bool Relayer::startRelaying(qintptr clientSocketDes)
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

Relayer::Status Relayer::getStatus() const
{
    return this->status;
}

bool Relayer::isDead() const
{
    return this->status == Dead;
}

void Relayer::onSfMServerDisconnected()
{
    this->updateStatus(Dead);
}

void Relayer::onSfMServerReadyRead()
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

void Relayer::onClientDisconnected()
{
    this->updateStatus(Dead);
}

void Relayer::onClientSocketReadyRead()
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

void Relayer::updateStatus(Relayer::Status mstatus)
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

