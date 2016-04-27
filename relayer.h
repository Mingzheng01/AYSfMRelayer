#ifndef SFMSERVERRELAYER_H
#define SFMSERVERRELAYER_H

#include <memory>
#include <QTcpSocket>
#include <QObject>
#include <QThread>

class Relayer: public QThread
{
    Q_OBJECT
public:
    enum Status {Idel, Ready, Started, Relaying, Dead};

    Relayer();
    ~Relayer();
    bool startRelayer(qintptr serverSocketDescriptor);
    bool startRelaying(qintptr clientSocketDes);
    Status getStatus() const;

    bool isDead() const;

protected:
    void run();

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
    int sendingPacketSize = 256;

    void updateStatus(Status status);
    bool sendByteArray(const QByteArray &array, std::shared_ptr<QTcpSocket> socket);
};

#endif // SFMSERVERRELAYER_H
