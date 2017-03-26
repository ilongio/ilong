#ifndef ILOVECHINA_H
#define ILOVECHINA_H

#include <QtCore>
#include <QtMath>

#include "ilong_global.h"


class ILoveChina
{
public:
    ILoveChina();
    static QPointF wgs84TOgcj02(QPointF wgs);
    static QPointF gcj02Towgs84(QPointF gcj);
    static bool DelDir(const QString &path);
private:
    static bool outOfChina(double lon, double lat);
    static double transformLat(double x, double y);
    static double transformLon(double x, double y);
    static QPointF delta(double lon, double lat);
};

#endif // ILOVECHINA_H
