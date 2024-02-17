#ifndef WEATHERTOOL_H
#define WEATHERTOOL_H
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>
#include <QCoreApplication>
#include <QFile>
#include <map>
#include <QDebug>

using namespace std;

class WeatherTool
{
public:
    WeatherTool();
    QString operator[](const QString& cityName);


private:
    map<QString, QString> mmapCityCode;
};

#endif // WEATHERTOOL_H
