#ifndef WEATHERDATAFORECAST_H
#define WEATHERDATAFORECAST_H
#include <QString>
#include <QJsonObject>

class WeatherDataForecast
{
public:
    WeatherDataForecast();
    WeatherDataForecast &operator=(const QJsonObject& obj);

public:
    QString date;
    QString week;
    QString high;
    QString low;
    QString quality;
    QString weatherType;
};

#endif // WEATHERDATAFORECAST_H
