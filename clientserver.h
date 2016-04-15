#ifndef CLIENTSERVER_H
#define CLIENTSERVER_H

#include <QTcpServer>
#include <memory>

class RelayerServer;
class ClientServer: public QTcpServer
{
public:
    ClientServer(int port);
    bool startServer();
    ~ClientServer();
    void setRelayerServer(std::shared_ptr<RelayerServer> server);
protected:
    void incomingConnection(qintptr handle);
private:
    std::shared_ptr<RelayerServer> relayerServer = nullptr;
    int port = 18998;
};

#endif // CLIENTSERVER_H
