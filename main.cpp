#include <QCoreApplication>
#include "relayerserver.h"
#include "clientserver.h"
#include "relayerconfig.h"
#include <memory>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    RelayerConfig config("./config.xml");

    std::shared_ptr<RelayerServer> sfmRelayerServer = std::make_shared<RelayerServer>(config.getIntConfigAttr("sfm_relayer_server_port"));
    sfmRelayerServer->startServer();

    std::shared_ptr<RelayerServer> userRelayerServer = std::make_shared<RelayerServer>(config.getIntConfigAttr("user_relayer_server_port"));
    userRelayerServer->startServer();

    ClientServer sfmClientServer(config.getIntConfigAttr("sfm_client_server_port"));
    sfmClientServer.setRelayerServer(sfmRelayerServer);
    sfmClientServer.startServer();

    ClientServer userClientServer(config.getIntConfigAttr("user_client_server_port"));
    userClientServer.setRelayerServer(userRelayerServer);
    userClientServer.startServer();

    return a.exec();
}
