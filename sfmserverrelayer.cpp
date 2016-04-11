#include "sfmserverrelayer.h"

SfMServerRelayer::SfMServerRelayer()
{

}

bool SfMServerRelayer::startRelayer(qintptr socketDescriptor)
{
    this->socket = std::make_shared<QTcpSocket>();
    this->connect(this->socket.get(), SIGNAL(disconnected()), this, SLOT(onSfMServerDisconnected()));
    this->connect(this->socket.get(), SIGNAL(readyRead()), this, SLOT(onSfMServerReadyRead()));

    if (this->socket->setSocketDescriptor(socketDescriptor))
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
    this->clientSocket = std::make_shared<QTcpSocket>();
    this->connect(this->clientSocket.get(), SIGNAL(disconnected()), this, SLOT(onClientDisconnected()));
    this->connect(this->clientSocket.get(), SIGNAL(readyRead()), this, SLOT(onClientSocketReadyRead()));

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

void SfMServerRelayer::onSfMServerDisconnected()
{
    if (this->clientSocket)
        this->clientSocket->close();

    this->updateStatus(Dead);
}

void SfMServerRelayer::onSfMServerReadyRead()
{
    if (this->status == Relaying)
    {
        QByteArray byteArray = this->socket->readAll();
        this->clientSocket->write(byteArray);
        this->clientSocket->flush();
    }
}

void SfMServerRelayer::onClientDisconnected()
{
    if (this->socket)
        this->socket->close();

    this->updateStatus(Dead);
}

void SfMServerRelayer::onClientSocketReadyRead()
{
    if (this->status == Relaying)
    {
        qDebug() << "came here";

        QByteArray byteArray = this->clientSocket->readAll();
        this->socket->write(byteArray);
        this->socket->flush();
    }
}

void SfMServerRelayer::updateStatus(SfMServerRelayer::Status status)
{
    this->status = status;
    emit statusUpdated();
}

