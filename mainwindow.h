#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QPoint>
#include <QList>
#include <QLabel>
#include <QPoint>
#include <QRect>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include "weathertool.h"
#include "weatherdatatoday.h"
#include "weatherdataforecast.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mGetWeatherInfo(QNetworkAccessManager *manager);
    void mParseJson(const QByteArray& data);
    void mRefreshUI();
    void mWeatherQualityRefresh(int i, int index);
    void mDrawSunRiseSet();
    void mPaintCurve();


private slots:
    void appExit();
    void replyFinished(QNetworkReply* reply);

    void on_btnRefresh_clicked();

    void on_btnSearch_clicked();

private:
    Ui::MainWindow *ui;
    QMenu *exitMenu;
    QAction *exitAct;
    QPoint mPos;

    //UI
    QList<QLabel *> forecast_week_list;
    QList<QLabel *> forecast_date_list;
    QList<QLabel *> forecast_quality_list;
    QList<QLabel *> forecast_weatherType_list;
    QList<QLabel *> forecast_weatherTypeIcon_list;
    QList<QLabel *> forecast_high_list;
    QList<QLabel *> forecast_low_list;

    QNetworkAccessManager *manager;
    QString url;
    QString city;
    QString tmpCity;
    WeatherTool tool;

    WeatherDataToday today;
    WeatherDataForecast forecast[6];

    static const QPoint sun[2];
    static const QRect sunRiseSet[2];
    static const QRect rect[2];

    QTimer* sunTimer;
    QTimer* tmpCurveTimer;
};


#endif // MAINWINDOW_H
