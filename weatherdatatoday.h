#ifndef WEATHERDATATODAY_H
#define WEATHERDATATODAY_H
#include <QString>
#include <QJsonObject>

class WeatherDataToday
{
public:
    WeatherDataToday();
    WeatherDataToday& operator=(const QJsonObject& obj);

public:
    //当天信息
    QString date;
    QString temperature;
    QString city;
    QString humidity; //湿度
    QString pm25;
    QString quality;
    QString getColdIndex;//感冒指数
    QString windDirection;//风向
    QString windForce;//风力
    QString weatherType;//天气情况
    QString sunRise;//日出时间
    QString sunSet;//日落时间
    QString tip;//提示信息
};
#endif // WEATHERDATA_H
