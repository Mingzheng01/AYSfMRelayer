#include "relayerconfig.h"

#include <boost/property_tree/xml_parser.hpp>
#include <boost/typeof/typeof.hpp>
#include <iostream>

RelayerConfig::RelayerConfig(const char *configFile)
{
    boost::property_tree::ptree ptree;
    boost::property_tree::read_xml(configFile, ptree);

    this->root = ptree.get_child("root");
}

QString RelayerConfig::getStringConfigAttr(const QString &attr) const
{
    boost::optional<std::string> child = root.get_optional<std::string>(attr.toStdString());
    if (child)
    {
        return QString::fromStdString(child.get());
    }
    else
        assert(0);
}
int RelayerConfig::getIntConfigAttr(const QString &attr) const
{
    boost::optional<int> child = root.get_optional<int>(attr.toStdString());
    if (child)
    {
        return child.get();
    }
    else
        assert(0);
}

double RelayerConfig::getDoubleConfigAttr(const QString &attr) const
{
    boost::optional<double> child = root.get_optional<double>(attr.toStdString());
    if (child)
    {
        return child.get();
    }
    else
        assert(0);
}



