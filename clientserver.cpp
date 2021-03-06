#include "clientserver.h"
#include "relayerserver.h"
#include <QTcpSocket>

ClientServer::ClientServer(int port): port(port)
{

}

bool ClientServer::startServer()
{
    if (this->listen(QHostAddress::Any, this->port))
    {
#ifdef DEBUG
        qDebug() << "clinet server started";
#endif
        return true;
    }
    else
    {
#ifdef DEBUG
        qDebug() << "client server starting failed";
#endif
        return false;
    }
}

ClientServer::~ClientServer()
{

}

void ClientServer::setRelayerServer(std::shared_ptr<RelayerServer> server)
{
    this->relayerServer = server;
}

void ClientServer::incomingConnection(qintptr handle)
{


#ifdef DEBUG
    static long long counter = 0;
    qDebug() << "incoming client connection on port : " << this->port << " #" << counter;
    ++counter;
#endif
    if (this->relayerServer)
    {
        if (this->relayerServer->handerClient(handle))
        {
#ifdef DEBUG
    qDebug() << "incoming client connection handled";
#endif
        }
        else
        {
#ifdef DEBUG
    qDebug() << "incoming client connection handled failed";
#endif

        // create the socket and close the connection
            QTcpSocket *socket = new QTcpSocket(this);
            socket->setSocketDescriptor(handle);
            socket->close();
            socket->deleteLater();
        }
    }
}

