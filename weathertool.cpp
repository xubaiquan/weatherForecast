#include "weathertool.h"


WeatherTool::WeatherTool()
{
    QString filePath = QCoreApplication::applicationDirPath();
//    qDebug() << "filePath : " << filePath;
    filePath += "/cityCode.json";
    QFile file(filePath);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray jsonData = file.readAll();
//    qDebug() << "jsonData : " << QString(jsonData);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    QJsonArray jsonArray = jsonDoc.array();
    for(int i=0; i<jsonArray.size(); i++) {
        QString cityCode = jsonArray.at(i).toObject().value("cityCode").toString();
        QString cityName = jsonArray.at(i).toObject().value("cityName").toString();
        qDebug() <<"print cityName and cityCode";
        if (cityCode.size() >0) {
            mmapCityCode.insert(pair<QString, QString>(cityName, cityCode));
//            qDebug() << "insert cityName: " << cityName;
//            qDebug() << "insert cityCode: " << cityCode;
        }
    }

}

QString WeatherTool::operator[](const QString &cityName)
{
    map<QString, QString>::iterator it = mmapCityCode.find(cityName);
    if (it == mmapCityCode.end()) {
        QString city = cityName + u8"市";
        it = mmapCityCode.find(city);
    }
    if (it == mmapCityCode.end()) {
        return "000000000";
    }

    return it->second;
}
