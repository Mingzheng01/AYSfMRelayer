#ifndef RELAYERSERVER_H
#define RELAYERSERVER_H

#include <vector>
#include <memory>
#include <QTcpServer>
#include <QObject>
#include <QTcpServer>

class Relayer;
class ClientRelayer;
class RelayerServer: public QTcpServer
{
    Q_OBJECT
public:
    RelayerServer(int port);
    ~RelayerServer();
    bool startServer();
    bool handerClient(qintptr descriptor);
    int getNumReadyRelayers() const;
protected:
    void incomingConnection(qintptr handle);
private slots:
    void onRelayerStatusUpdated();
private:
    std::vector<std::shared_ptr<Relayer>> serverRelayers;
    int port = 17689;
};

#endif // RELAYERSERVER_H
