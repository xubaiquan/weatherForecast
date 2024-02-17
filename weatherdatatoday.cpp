#include "weatherdatatoday.h"
#include <QDate>
#include <QJsonArray>

WeatherDataToday::WeatherDataToday()
{
    date = "0000-00-00";
    city = "null";
    windDirection = u8"无数据";
    windForce = u8"无数据";
    getColdIndex =  u8"无数据";
    tip =  u8"无数据";
    pm25 =  u8"无数据";
    quality =  u8"无数据";
    humidity =  u8"无数据";
    sunRise =  u8"无数据";
    sunSet =  u8"无数据";
    temperature = "null";
    weatherType = u8"无数据";
}

WeatherDataToday &WeatherDataToday::operator=(const QJsonObject &obj)
{
    QString dateStr = obj.value("date").toString();
    date = QDate::fromString(dateStr, "yyyyMMdd").toString("yyyy-MM-dd");
    city = obj.value("cityInfo").toObject().value("city").toString();

    //parse data
    QJsonObject dataObj = obj.value("data").toObject();
    humidity = dataObj.value("shidu").toString();
    pm25 = QString::number(dataObj.value("pm25").toDouble());
    quality = dataObj.value("quality").toString();
    temperature = dataObj.value("wendu").toString();
    getColdIndex = dataObj.value("ganmao").toString();
    QJsonArray forecastArray = dataObj.value("forecast").toArray();
    QJsonObject todayObj = forecastArray.at(0).toObject();
    windDirection = todayObj.value("fx").toString();
    windForce = todayObj.value("fl").toString();
    weatherType = todayObj.value("type").toString();
    sunRise =  todayObj.value("sunrise").toString();
    sunSet =  todayObj.value("sunset").toString();
    tip = todayObj.value("notice").toString();

    return *this;
}
