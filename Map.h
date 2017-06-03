#ifndef MAP_H
#define MAP_H

#include <QObject>
#include <QLocale>

#include "ilong_global.h"

/*
 * 地图供应商管理
 * */

class Map : public QObject
{
    Q_OBJECT
public:
    explicit ILONGSHARED_EXPORT Map(QObject *parent = 0);
    /*
     * 取到地图供应商服务地址
     * */
    QString getServer();
    QString getPath();
    /*
     * 得到坐标点的瓦片下载地址的path部分,如:/maps/vt?lyrs=s@701,r@701&gl=cn&x=%2&y=%3&z=%1
     * */
    QString queryTile(int x, int y, int z);
    /*
     * 判断瓦片是否有效
     * */
    bool isTileValid(int x, int y, int z);
private:
    int param1;
    int param2;
    int param3;
    int order[3][2];

    QString server;
    QString path;

    QLocale loc;
signals:

public slots:
};

#endif // MAP_H
