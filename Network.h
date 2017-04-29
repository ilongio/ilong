#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QList>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include "ILong.h"
#include "SQLExcute.h"

/*
 * 提供简单的网络下载能力,主要就是用来下载瓦片的
 * */

class ILong;
class ILONGSHARED_EXPORT Network : public QObject
{
    Q_OBJECT
public:
    explicit Network(ILong *iL , QObject * parent = 0);
    ~Network();
    /*
     * 判断当前是否有数据在下载了
     */
    bool getDownloadState();
private:
    /*
     * 从@host和@path里生成瓦片下载地址
     */
    QString getUrl(QString host, QString path);
    /*
     * 从@Url计算出x,y,z坐标
     */
    TPoint getXYZFromUrl(QString Url);
    /*
     * 用来保存所有要下载的瓦片地址了,下完一个山一个
     */
    QList<QString>  *list;
    QNetworkAccessManager * manager;
    bool isDownloading;
    ILong *iLong;
    SQLExcute * sqlExcute;
signals:
    void startAgain();
    void sendTileCount(int);
    void newImage();
public slots:
    void start();
    void requestFinished(QNetworkReply *reply);
};

#endif // NETWORK_H
