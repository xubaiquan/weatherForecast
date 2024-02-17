#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QContextMenuEvent>
#include <QMessageBox>
#include <QDebug>
#include <QPainter>
#include <QPen>
#include <QTime>

#define SPAN_INDEX 3//温度曲线间隔指数
#define ORIGIN_SIZE 3 //温度曲线原点大小
#define TEMPERATURE_STARTING_COORDINATE 45 //高温平均值起始坐标

//sun rise set bottom line
const QPoint MainWindow::sun[2] = {
    QPoint(20, 75),
    QPoint(130, 75)
};

//sun rise set time
const QRect MainWindow::sunRiseSet[2] = {
    QRect(0, 80, 50, 20),
    QRect(100, 80, 50,20)
};

//sun rise set Arc(圆弧)
const QRect MainWindow::rect[2] = {
    QRect(25, 25, 100, 100),
    QRect(50, 80, 50,20)
};

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //set frameless window
    setWindowFlag(Qt::FramelessWindowHint);
    //set fixed size
    setFixedSize(width(), height());

    exitMenu = new QMenu(this);
    exitAct = new QAction;
    exitAct->setText(u8"退出");
    exitAct->setIcon(QIcon(":/icon/icon/close.png"));
    exitMenu->addAction(exitAct);
    connect(exitAct, SIGNAL(triggered(bool)), this, SLOT(appExit()));

    //initialize UI
    forecast_week_list << ui->labelWeek01 << ui->labelWeek02 << ui->labelWeek03 << ui->labelWeek04 << ui->labelWeek05 << ui->labelWeek06;
    forecast_date_list << ui->labelDay01 << ui->labelDay02 << ui->labelDay03 << ui->labelDay04 << ui->labelDay05 << ui->labelDay06;
    forecast_high_list << ui->labelHighTpt01 << ui->labelHighTpt02 << ui->labelHighTpt03 << ui->labelHighTpt04 << ui->labelHighTpt05 << ui->labelHighTpt06;
    forecast_low_list << ui->labelLowTpt01 << ui->labelLowTpt02 << ui->labelLowTpt03 << ui->labelLowTpt04 << ui->labelLowTpt05 << ui->labelLowTpt06;
    forecast_quality_list << ui->labelQuality01 << ui->labelQuality02 << ui->labelQuality03 << ui->labelQuality04 << ui->labelQuality05 << ui->labelQuality06;
    forecast_weatherType_list << ui->labelType01 << ui->labelType02 << ui->labelType03 << ui->labelType04 << ui->labelType05 << ui->labelType06;
    forecast_weatherTypeIcon_list << ui->labelTypeIcon01 << ui->labelTypeIcon02 << ui->labelTypeIcon03 << ui->labelTypeIcon04 << ui->labelTypeIcon05 << ui->labelTypeIcon06;

    //set date and week stylesheet
    for (int i = 0; i < 6; i++) {
        forecast_date_list.at(i)->setStyleSheet("background-color:rgba(0, 255, 255, 100)");
        forecast_week_list.at(i)->setStyleSheet("background-color:rgba(0, 255, 255, 100)");
    }

    ui->lineEditCity->setStyleSheet("QLineEdit{border : 1px gray solid; border-radius : 4px;}"
                                   "QLinEdit:hover{border-color:rgb(101, 255, 106)}");

    //request weather info
    url = "http://t.weather.itboy.net/api/weather/city/";
    city = u8"长沙市";
    tmpCity = city;
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    mGetWeatherInfo(manager);

    ui->labelSunRiseSet->installEventFilter(this);
    sunTimer = new QTimer(this);
    connect(sunTimer, SIGNAL(timeout()), this, SLOT(update()));
    sunTimer->start(1000);

    ui->labelTptCurve->installEventFilter(this);
    ui->labelTptCurve->update();



}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->labelSunRiseSet && event->type() == QEvent::Paint) {
        mDrawSunRiseSet();
    } else if (watched == ui->labelTptCurve && event->type() == QEvent::Paint) {
        mPaintCurve();
    }

    return QWidget::eventFilter(watched, event);
}

void MainWindow::appExit()
{
    qApp->exit(0);
}

void MainWindow::replyFinished(QNetworkReply *reply)
{
    //check reply info and status code
    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (reply->error() != QNetworkReply::NoError || status_code != 200) {
        QMessageBox::warning(this, u8"错误", u8"天气：请求数据错误，检查网络连接", QMessageBox::Ok);
        return;
    }

    //parse reply data
    QByteArray data = reply->readAll();
    //qDebug() << data;
    mParseJson(data);
    mRefreshUI();

}


void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    exitMenu->exec(QCursor::pos());
    event->accept();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    this->mPos = event->globalPos() - this->pos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - this->mPos);
}

void MainWindow::mGetWeatherInfo(QNetworkAccessManager *manager)
{
    QString cityCode = tool[city];
    if (cityCode == "000000000") {
        QMessageBox::warning(this, "warning", "no such city", QMessageBox::Ok);
        return;
    }

    QUrl jsonUrl(url + cityCode);
    manager->get(QNetworkRequest(jsonUrl));

}

void MainWindow::mParseJson(const QByteArray& data)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    QJsonObject jsObj = jsonDoc.object();

    QString message = jsObj.value("message").toString();
    qDebug() << message;
    if (message.contains("success") != true) {
        QMessageBox::information(this, "information", "no city data", QMessageBox::Ok);
        city = tmpCity;
        return;
    }

    today = jsObj;

    forecast[0] = jsObj.value("data").toObject().value("yesterday").toObject();

    for (int i = 1; i < 6; i++) {
        forecast[i] = jsObj.value("data").toObject().value("forecast").toArray()[i-1].toObject();
    }


}

void MainWindow::mRefreshUI()
{
    //refresh today data
    ui->labelDate->setText(today.date);
    ui->labelTemperature->setText(today.temperature);
    ui->labelCity->setText(today.city);
    ui->labelType->setText(today.weatherType);
    ui->labelTip->setText(today.tip);
    ui->labelHumidity->setText(today.humidity);
    ui->labelPM25->setText(today.pm25);
    ui->labelWindDirection->setText(today.windDirection);
    ui->labelWindForce->setText(today.windForce);
    ui->txtBrowserGetCold->setText(today.getColdIndex);

    //refresh 6 days data
    for (int i = 0; i < 6; i++) {
        forecast_week_list[i]->setText(forecast[i].week.right(3));
        forecast_date_list[i]->setText(forecast[i].date.left(3));
        forecast_weatherType_list[i]->setText(forecast[i].weatherType);
        forecast_high_list[i]->setText(forecast[i].high.split(" ").at(1));
        forecast_low_list[i]->setText(forecast[i].low.split(" ").at(1));
        forecast_weatherTypeIcon_list[i]->setStyleSheet(tr("image:url(:/weather_icon/weather_icons/%1.png)").arg(forecast[i].weatherType));

        mWeatherQualityRefresh(i, forecast[i].quality.toInt());
    }

    ui->labelWeek01->setText(u8"昨天");
    ui->labelWeek02->setText(u8"今天");
    ui->labelWeek03->setText(u8"明天");

    ui->labelTpeIcon->setStyleSheet(tr("image:url(:/weather_icon/weather_icons/%1.png)").arg(forecast[1].weatherType));

}

void MainWindow::mWeatherQualityRefresh(int i, int index)
{
    if (index >=0 && index <= 50) {
        forecast_quality_list[i]->setText(u8"优质");
        forecast_quality_list[i]->setStyleSheet("color: rgb(0, 255, 0)");
    } else if (index >50 && index <= 100) {
        forecast_quality_list[i]->setText(u8"良好");
        forecast_quality_list[i]->setStyleSheet("color: rgb(255, 255, 0)");
    } else if (index > 100 && index <= 150) {
        forecast_quality_list[i]->setText(u8"轻度污染");
        forecast_quality_list[i]->setStyleSheet("color: rgb(255, 170, 0)");
    } else if (index > 150 && index <= 200) {
        forecast_quality_list[i]->setText(u8"中度污染");
        forecast_quality_list[i]->setStyleSheet("color: rgb(255, 0, 0)");
    } else if (index > 200 && index <= 250) {
        forecast_quality_list[i]->setText(u8"重度污染");
        forecast_quality_list[i]->setStyleSheet("color: rgb(170, 0, 0)");
    }
}

void MainWindow::mDrawSunRiseSet()
{
    QPainter painter(ui->labelSunRiseSet);
    //set antialias
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.save();
    QPen pen = painter.pen();
    pen.setColor(Qt::yellow);
    pen.setWidth(0.5);
    painter.setPen(pen);
    painter.drawLine(sun[0], sun[1]);
    painter.restore();

    painter.save();
    painter.setFont(QFont("Microsoft Yahei", 8, QFont::Normal));
    painter.setPen(Qt::white);

    if (today.sunRise != "" && today.sunSet != "") {
        painter.drawText(sunRiseSet[0], Qt::AlignHCenter, today.sunRise);
        painter.drawText(sunRiseSet[1], Qt::AlignHCenter, today.sunSet);
    }
    painter.restore();

    painter.save();
    pen.setWidth(0.5);
    pen.setStyle(Qt::DotLine);
    pen.setColor(Qt::green);
    painter.setPen(pen);
    painter.drawArc(rect[0], 0 * 16, 180 * 16);
    painter.restore();

    painter.save();
    if (today.sunRise != "" && today.sunSet != "") {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 85, 0, 100));

        int startAngle, spanAngle;
        QString sunsetTime = today.date + " " + today.sunSet;

        if (QDateTime::currentDateTime() > QDateTime::fromString(sunsetTime, "yyyy-MM-dd hh:mm")) {
            startAngle = 0 * 16;
            spanAngle = 180 * 16;
        } else {
            static QStringList sunSetTime = today.sunSet.split(":");
            static QStringList sunRiseTime = today.sunRise.split(":");

            static QString sunsetHour = sunSetTime.at(0);
            static QString sunsetMint = sunSetTime.at(1);
            static QString sunriseHour = sunRiseTime.at(0);
            static QString sunriseMint = sunRiseTime.at(1);

            static int sunrise = sunriseHour.toInt() * 60 + sunriseMint.toInt();
            static int sunset = sunsetHour.toInt() * 60 + sunsetMint.toInt();

            int now = QTime::currentTime().hour() * 60 + QTime::currentTime().minute();

            startAngle = ((double)(sunset - now) / (sunset - sunrise)) * 180 * 16;
            spanAngle = ((double)(now - sunrise) / (sunset - sunrise)) * 180 * 16;

//            qDebug() << "startAngle : " << startAngle;
//            qDebug() << "spanAngle : " << spanAngle;
        }

        if (startAngle >= 0 && spanAngle >= 0) {
            painter.drawPie(rect[0], startAngle, spanAngle);
        }
    }

    painter.restore();
}

void MainWindow::mPaintCurve()
{
    QPainter painter(ui->labelTptCurve);
    painter.setRenderHint(QPainter::Antialiasing);

    int high[6] = {0};
    int low[6] = {0};
    int tmpTotal = 0;
    QString h;
    QString l;


    for (int i = 0; i < 6; i++) {
        h = forecast[i].high.split(" ").at(1);
        h = h.left(h.size() - 1);
        high[i] = (int)h.toDouble();
        tmpTotal += high[i];

        l = forecast[i].low.split(" ").at(1);
        l = l.left(l.size() - 1);
        low[i] = (int)l.toDouble();
    }

    int averageTmp = tmpTotal / 6;

    //calculate temperature coordinate
    int pointX[6] = {35, 103, 172, 241, 310, 379};
    int pointHY[6] = {0};
    int pointLY[6] = {0};

    for (int i = 0; i < 6; i++) {
        pointHY[i] = TEMPERATURE_STARTING_COORDINATE - (high[i] - averageTmp)*SPAN_INDEX;
        pointLY[i] = TEMPERATURE_STARTING_COORDINATE + (averageTmp - low[i])*SPAN_INDEX;
    }

    //paint curve
    QPen pen = painter.pen();
    pen.setWidth(1);

    //high temperature curve
    painter.save();
    pen.setColor(QColor(255, 170, 0));
    pen.setStyle(Qt::DotLine);
    painter.setPen(pen);
    painter.setBrush(QColor(255, 170, 0));
    painter.drawEllipse(pointX[0], pointHY[0], ORIGIN_SIZE, ORIGIN_SIZE);
    painter.drawEllipse(pointX[1], pointHY[1], ORIGIN_SIZE, ORIGIN_SIZE);
    painter.drawLine(pointX[0], pointHY[0], pointX[1], pointHY[1]);

    pen.setStyle(Qt::SolidLine);
    pen.setWidth(1);
    painter.setPen(pen);

    for (int i = 2; i < 6; i++) {
        painter.drawEllipse(pointX[i], pointHY[i], ORIGIN_SIZE, ORIGIN_SIZE);
        painter.drawLine(pointX[i-1], pointHY[i-1], pointX[i], pointHY[i]);
    }

    painter.restore();

    //low temperature curve
    painter.save();
    pen.setColor(QColor(0, 255, 255));
    pen.setStyle(Qt::DotLine);
    painter.setPen(pen);
    painter.setBrush(QColor(0, 255, 255));
    painter.drawEllipse(pointX[0], pointLY[0], ORIGIN_SIZE, ORIGIN_SIZE);
    painter.drawEllipse(pointX[1], pointLY[1], ORIGIN_SIZE, ORIGIN_SIZE);
    painter.drawLine(pointX[0], pointLY[0], pointX[1], pointLY[1]);

    pen.setStyle(Qt::SolidLine);
    pen.setWidth(1);
    painter.setPen(pen);

    for (int i = 2; i < 6; i++) {
        painter.drawEllipse(pointX[i], pointLY[i], ORIGIN_SIZE, ORIGIN_SIZE);
        painter.drawLine(pointX[i-1], pointLY[i-1], pointX[i], pointLY[i]);
    }

    painter.restore();


}


void MainWindow::on_btnRefresh_clicked()
{
    mGetWeatherInfo(manager);
    ui->labelTptCurve->update();
}


void MainWindow::on_btnSearch_clicked()
{
    tmpCity = city;
    city = ui->lineEditCity->text();
    mGetWeatherInfo(manager);
}

