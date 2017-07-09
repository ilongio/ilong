#ifndef ILOVECHINA_H
#define ILOVECHINA_H

#include <QtCore>
#include <QtMath>

#include "ilong_global.h"

/*
 * 这个东西嘛,大家都知道在天朝地图偏移问题了,虽然有坑但不影响我爱中国是吧?所以还是叫ILoveChina,虽然代码不是我写的!
 * 说白了就是用来把WGS坐标和火星坐标(gcj)相互调教的,可以不用理它
 * */

class ILONGSHARED_EXPORT ILoveChina
{
public:
    ILoveChina();
    /*
     * 世界WGS坐标转火星坐标
     * */
    static QPointF wgs84TOgcj02(QPointF wgs);
    /*
     * 火星坐标转世界WGS坐标
     * */
    static QPointF gcj02Towgs84(QPointF gcj);
    /*
     * 这个先不管它,可能以后我要用到,先留着
     * */
    static bool DelDir(const QString &path);

private:
    static bool outOfChina(double lon, double lat);
    static double transformLat(double x, double y);
    static double transformLon(double x, double y);
    static QPointF delta(double lon, double lat);
};

#endif // ILOVECHINA_H
