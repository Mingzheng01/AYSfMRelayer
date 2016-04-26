#ifndef SFMSERVERRELAYER_H
#define SFMSERVERRELAYER_H

#include <memory>
#include <QTcpSocket>
#include <QObject>

class Relayer: public QObject
{
    Q_OBJECT
public:
    enum Status {Idel, Started, Relaying, Dead};

    Relayer();
    ~Relayer();
    bool startRelayer(qintptr serverSocketDescriptor);
    bool startRelaying(qintptr clientSocketDes);
    Status getStatus() const;
    bool isDead() const;

signals:
    void statusUpdated();
private slots:
    void onSfMServerDisconnected();
    void onSfMServerReadyRead();
    void onClientDisconnected();
    void onClientSocketReadyRead();

private:
    std::shared_ptr<QTcpSocket> serverSocket = nullptr;
    std::shared_ptr<QTcpSocket> clientSocket = nullptr;
    Status status = Idel;

    void updateStatus(Status status);
};

#endif // SFMSERVERRELAYER_H
