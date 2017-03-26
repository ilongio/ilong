#ifndef MAP_H
#define MAP_H

#include <QObject>
#include <QLocale>

#include "ilong_global.h"

class Map : public QObject
{
    Q_OBJECT
public:
    explicit Map(QObject *parent = 0);
    //取到地图供应商服务地址
    QString getServer();
    QString getPath();
    //得到坐标点的瓦片下载地址
    QString queryTile(int x, int y, int z);
    //判断瓦片是否有效
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
