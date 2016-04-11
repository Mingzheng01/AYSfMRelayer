#include <QCoreApplication>
#include "relayerserver.h"
#include "clientserver.h"
#include <memory>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    std::shared_ptr<RelayerServer> relayerServer = std::make_shared<RelayerServer>();
    relayerServer->startServer();


    ClientServer clientServer;
    clientServer.setRelayerServer(relayerServer);
    clientServer.startServer();

    return a.exec();
}
