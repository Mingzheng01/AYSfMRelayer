#ifndef RELAYERCONFIG_H
#define RELAYERCONFIG_H

#include <QString>
#include "boost/property_tree/ptree.hpp"

class RelayerConfig
{
public:
    RelayerConfig(const char* configFile);

    QString getStringConfigAttr(const QString &attr) const;
    int getIntConfigAttr(const QString &attr) const;
    double getDoubleConfigAttr(const QString &attr) const;

private:
    boost::property_tree::ptree root;
};

#endif // RELAYERCONFIG_H
