#ifndef RELAYERSERVER_H
#define RELAYERSERVER_H

#include <vector>
#include <memory>
#include <QTcpServer>
#include <QObject>
#include <QTcpServer>

class SfMServerRelayer;
class ClientRelayer;
class RelayerServer: public QTcpServer
{
    Q_OBJECT
public:
    RelayerServer();
    ~RelayerServer();
    bool startServer();
    bool handerClient(qintptr descriptor);
protected:
    void incomingConnection(qintptr handle);
private slots:
    void onRelayerStatusUpdated();
private:
    std::vector<std::shared_ptr<SfMServerRelayer>> sfmServerRelayers;

};

#endif // RELAYERSERVER_H
