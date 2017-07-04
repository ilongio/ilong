#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QObject>
#include <QVariant>
#include <QGraphicsObject>
#include <QPainter>
#include <QLabel>
#include <QUuid>
#include <QDebug>

#include "ilong_global.h"

/*
 * 图元基类,提供所有点类和面类的基本结构
 * */

class ILONGSHARED_EXPORT Geometry : public QGraphicsObject
{
    Q_OBJECT
public:
    /*
     * 插入图元结构
     * */
    typedef struct
    {
        Geometry * geometry;    //图元
        QList<QVariant> data;   //一行数据放到data列表里
    } ILongDataType;
    explicit Geometry(ILongGeoType gType,quint8 lWidth, QColor iPen, QColor iBrush);
    ILongGeoRect getRect();
    ILongGeoType getGeoType();
    QPointF getCenter();
    QString getPen();
    QString getBrush();
    QString getPoints();
    quint8 getLineWidth();
    quint8 getSize();
    quint32 getID();
    int getDir();
    bool getCloseFlag();
    void rotate(int dir);
    void setLabel(QString lb);
    QString getLabel();
    int getLabelPixeSize();
protected:
    void checkRect();
    QList<QPointF> list;
    ILongGeoType geoType;
    QColor pen, brush;
    quint8 lineWidth;
    int size;
    ILongGeoRect rect;
    quint32 itemID;
    QString label;
    int dir;
    bool closeFlag;
signals:

public slots:
};

#endif // GEOMETRY_H
