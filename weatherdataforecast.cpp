#include "weatherdataforecast.h"

WeatherDataForecast::WeatherDataForecast()
{
}

WeatherDataForecast &WeatherDataForecast::operator=(const QJsonObject &obj)
{
    date = obj.value("date").toString();
    week = obj.value("week").toString();
    high = obj.value("high").toString();
    low = obj.value("low").toString();
    quality = QString::number(obj.value("aqi").toDouble());
    weatherType = obj.value("type").toString();
    return *this;
}
