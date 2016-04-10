#ifndef RELAYERSERVER_H
#define RELAYERSERVER_H

#include <vector>
#include <memory>
#include <QTcpServer>

class SfMServerRelayer;
class RelayerServer: public QTcpServer
{
public:
    RelayerServer();
private:
    std::vector<std::shared_ptr<SfMServerRelayer>> sfmServerRelayers;
};

#endif // RELAYERSERVER_H
