#ifndef SFMSERVERRELAYER_H
#define SFMSERVERRELAYER_H

#include <memory>
#include <QTcpSocket>
#include <QObject>

class SfMServerRelayer: public QObject
{
    Q_OBJECT
public:
    enum Status {Idel, Started, Relaying, Dead};

    SfMServerRelayer();
    bool startRelayer(qintptr socketDescriptor);
    bool startRelaying(qintptr clientSocketDes);
    Status getStatus() const;

signals:
    void statusUpdated();
private slots:
    void onSfMServerDisconnected();
    void onSfMServerReadyRead();
    void onClientDisconnected();
    void onClientSocketReadyRead();

private:
    std::shared_ptr<QTcpSocket> socket = nullptr;
    std::shared_ptr<QTcpSocket> clientSocket = nullptr;
    Status status = Idel;

    void updateStatus(Status status);
};

#endif // SFMSERVERRELAYER_H
