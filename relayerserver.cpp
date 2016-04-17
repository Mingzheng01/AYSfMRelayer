#include "relayerserver.h"
#include "relayer.h"
#include <QNetworkInterface>
#include <QThread>

RelayerServer::RelayerServer(int port): port(port)
{

}

RelayerServer::~RelayerServer()
{
}

bool RelayerServer::startServer()
{
    if (this->listen(QHostAddress::Any, this->port))
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

    for (std::shared_ptr<Relayer> relayer : this->serverRelayers)
    {
        if (relayer->getStatus() == Relayer::Status::Started)
        {
            relayer->startRelaying(descriptor);
            return true;
        }
    }
    return false;
}

int RelayerServer::getNumReadyRelayers() const
{
    return std::count_if(this->serverRelayers.begin(), this->serverRelayers.end(), [](std::shared_ptr<Relayer> relayer)->bool
    {
       return  relayer->getStatus() == Relayer::Status::Started;
    });
}

void RelayerServer::incomingConnection(qintptr handle)
{

#ifdef DEBUG
    qDebug() << "incommig sfm server connection with des : " << handle;
#endif

    std::shared_ptr<Relayer> relayer = std::make_shared<Relayer>();
    if (relayer->startRelayer(handle))
    {
        this->connect(relayer.get(), SIGNAL(statusUpdated()), this, SLOT(onRelayerStatusUpdated())), Qt::QueuedConnection; //should happen asyn
        this->serverRelayers.push_back(relayer);


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
    //check and delete the dead relayers
    //remove the clients these are finished
    auto newEndItr = std::remove_if(this->serverRelayers.begin(), this->serverRelayers.end(), [&](std::shared_ptr<Relayer> relayer)->bool{
       return relayer->isDead();
    });
    this->serverRelayers.erase(newEndItr, this->serverRelayers.end());

#ifdef DEBUG
    qDebug() << "relayer status updated (current relayer number : " << this->serverRelayers.size() << " )";
#endif
}
