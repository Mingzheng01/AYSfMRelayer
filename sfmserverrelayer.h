#ifndef SFMSERVERRELAYER_H
#define SFMSERVERRELAYER_H

#include <memory>
#include <QTcpSocket>

class SfMServerRelayer
{
public:
    SfMServerRelayer();
private:
    std::shared_ptr<QTcpSocket> socket = nullptr;
};

#endif // SFMSERVERRELAYER_H
