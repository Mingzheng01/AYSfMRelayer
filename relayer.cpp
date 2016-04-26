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
    if (this->status != Idel)
        return false;

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
    if (this->status != Started)
        return false;

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

#ifdef DEBUG
    qDebug() << "server ready read..";
#endif
    while (this->serverSocket->bytesAvailable() > 0)
    {
        //just relay the data
        if (this->status == Relaying)
        {
            try {
                QByteArray byteArray = this->serverSocket->readAll();
                this->clientSocket->write(byteArray);
                this->clientSocket->flush();
#ifdef DEBUG
    qDebug() << "server forwarded to client..";
#endif
            } catch (...) {
                this->updateStatus(Dead);
                break;
            }
        }
        else
        {
            this->updateStatus(Dead);
            break;
        }
    }
}

void Relayer::onClientDisconnected()
{
    this->updateStatus(Dead);
}

void Relayer::onClientSocketReadyRead()
{
#ifdef DEBUG
    qDebug() << "client ready read..";
#endif

    while (this->clientSocket->bytesAvailable() > 0)
    {
        if (this->status == Relaying)
        {
            try {
                QByteArray byteArray = this->clientSocket->readAll();
                this->serverSocket->write(byteArray);
                this->serverSocket->flush();
#ifdef DEBUG
    qDebug() << "client forwarded to server..";
#endif
            } catch (...) {
                this->updateStatus(Dead);
                break;
            }
        }
        else
        {
            this->updateStatus(Dead);
            break;
        }

    }
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

