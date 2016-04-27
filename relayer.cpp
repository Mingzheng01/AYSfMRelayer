#include "relayer.h"
#include <functional>
#include <QDateTime>

Relayer::Relayer()
{
    std::function<void(QTcpSocket*)> socketDeleter = [](QTcpSocket *socket){
        socket->deleteLater();
    };

    this->serverSocket = std::shared_ptr<QTcpSocket>(new QTcpSocket(), socketDeleter);
    this->serverSocket->setReadBufferSize(0); //unlimited size for the read buffer
    this->connect(this->serverSocket.get(), SIGNAL(disconnected()), this, SLOT(onSfMServerDisconnected()), Qt::DirectConnection);
    this->connect(this->serverSocket.get(), SIGNAL(readyRead()), this, SLOT(onSfMServerReadyRead()), Qt::DirectConnection);

    this->clientSocket = std::shared_ptr<QTcpSocket>(new QTcpSocket(), socketDeleter);
    this->clientSocket->setReadBufferSize(0); //unlimited size for the read buffer
    this->connect(this->clientSocket.get(), SIGNAL(disconnected()), this, SLOT(onClientDisconnected()), Qt::DirectConnection);
    this->connect(this->clientSocket.get(), SIGNAL(readyRead()), this, SLOT(onClientSocketReadyRead()), Qt::DirectConnection);

    this->start();
}

Relayer::~Relayer()
{
}

bool Relayer::startRelayer(qintptr serverSocketDescriptor)
{
    if (this->serverSocket->setSocketDescriptor(serverSocketDescriptor))
    {
        this->updateStatus(Started);
        this->serverSocket->moveToThread(this);
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
        this->clientSocket->moveToThread(this);
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

void Relayer::run()
{
#ifdef DEBUG
    qDebug() << "THREAD started with id : " << QThread::currentThreadId();
#endif

    exec();
}

void Relayer::onSfMServerDisconnected()
{
    this->updateStatus(Dead);
}

void Relayer::onSfMServerReadyRead()
{
    while (this->serverSocket->bytesAvailable() > 0)
    {

        //just relay the data
        if (this->status == Relaying)
        {
            try {
#ifdef DEBUG
                qint64 time = QDateTime::currentMSecsSinceEpoch();
                qDebug() << "start server forwarding to client. availiable:" << this->serverSocket->bytesAvailable() << " on thread:" << QThread::currentThreadId();
#endif
                QByteArray byteArray = this->serverSocket->readAll();
                if (! sendByteArray(byteArray, this->clientSocket))
                {
#ifdef DEBUG
                qDebug() << "server forwarding to client failed." << " bytes:" << byteArray.size() << " t :" << QDateTime::currentMSecsSinceEpoch() - time << "ms ";
#endif
                    this->updateStatus(Dead);
                    break;
                }

#ifdef DEBUG
                qDebug() << "server forwarded to client." << " bytes:" << byteArray.size() << " t :" << QDateTime::currentMSecsSinceEpoch() - time << "ms ";
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
    while (this->clientSocket->bytesAvailable() > 0)
    {
        if (this->status == Relaying)
        {
            try {

#ifdef DEBUG
                qint64 time = QDateTime::currentMSecsSinceEpoch();
                qDebug() << "start client forwarding to server. availiable:" << this->clientSocket->bytesAvailable() << " on thread:" << QThread::currentThreadId();;
#endif
                QByteArray byteArray = this->clientSocket->readAll();
                if (! sendByteArray(byteArray, this->serverSocket))
                {
#ifdef DEBUG
                qDebug() << "client forwarding to server failed." << " bytes:" << byteArray.size() << " t : " << QDateTime::currentMSecsSinceEpoch() - time << "ms ";
#endif
                    this->updateStatus(Dead);
                    break;
                }

#ifdef DEBUG
                qDebug() << "client forwarded to server." << " bytes:" << byteArray.size() << " t : " << QDateTime::currentMSecsSinceEpoch() - time << "ms ";
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

bool Relayer::sendByteArray(const QByteArray &array, std::shared_ptr<QTcpSocket> socket)
{
    int packets = array.size() / this->sendingPacketSize;
    if (array.size() % this->sendingPacketSize != 0)
        ++packets;

    for (int i = 0; i < packets; ++i)
    {
        int writeSize = this->sendingPacketSize;
        if (i * this->sendingPacketSize + writeSize > array.size())
            writeSize = array.size() - i * this->sendingPacketSize;

        if (socket->write(array.data() + i * this->sendingPacketSize, writeSize) != writeSize)
        {
#ifdef DEBUG
            qDebug() << "data not fully written.";
#endif
            return false;
        }

        if (! socket->waitForBytesWritten(3000000)) //forever
            return false;
    }

    return true;
}

