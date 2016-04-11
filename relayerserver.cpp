#include "relayerserver.h"
#include "sfmserverrelayer.h"
#include <QNetworkInterface>

RelayerServer::RelayerServer()
{

#ifdef DEBUG
    for (QHostAddress address : QNetworkInterface::allAddresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol)
            qDebug() << address.toString();
    }
#endif
}

bool RelayerServer::startServer()
{
    if (this->listen(QHostAddress::Any, 18669))
    {
#ifdef DEBUG
        qDebug() << "relayer server started";
#endif

        return true;
    }
    else
    {
#ifdef DEBUG
        qDebug() << "relayer server starting failed";
#endif
        return false;
    }
}

bool RelayerServer::handerClient(qintptr descriptor)
{
    for (std::shared_ptr<SfMServerRelayer> relayer : this->sfmServerRelayers)
    {
        if (relayer->getStatus() == SfMServerRelayer::Status::Started)
        {
            relayer->startRelaying(descriptor);
            return true;
        }
    }
    return false;
}

void RelayerServer::incomingConnection(qintptr handle)
{
#ifdef DEBUG
    qDebug() << "incommig sfm server connection with des : " << handle;
#endif

    std::shared_ptr<SfMServerRelayer> relayer = std::make_shared<SfMServerRelayer>();
    if (relayer->startRelayer(handle))
    {
        this->sfmServerRelayers.push_back(relayer);
#ifdef DEBUG
    qDebug() << "start relayer succeed";
#endif
    }
    else
    {
#ifdef DEBUG
    qDebug() << "start relayer failed";
#endif
    }
}

void RelayerServer::onRelayerStatusUpdated()
{

}
