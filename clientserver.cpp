#include "clientserver.h"
#include "relayerserver.h"

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
        qDebug() << "clinet server starting failed";
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
    qDebug() << "incoming client connection";
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

        }
    }
}

