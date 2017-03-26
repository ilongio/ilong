#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QList>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include "ILong.h"
#include "SQLExcute.h"

class ILong;
class Network : public QObject
{
    Q_OBJECT
public:
    typedef struct
    {
        int x;
        int y;
        int z;
    } TPoint;

    explicit Network(ILong *iL , QObject * parent = 0);
    ~Network();
    bool getDownloadState();
private:
    QString getUrl(QString host, QString path);
    TPoint getXYZFromUrl(QString Url);
    QList<QString>  *list;
    QNetworkAccessManager * manager;
    bool isDownloading;
    ILong *iLong;
    SQLExcute sqlExcute;
signals:
    void startAgain();
    void sendTileCount(int);
    void newImage();
public slots:
    void start();
    void requestFinished(QNetworkReply *reply);
};

#endif // NETWORK_H
