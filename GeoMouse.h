#ifndef GEOMOUSE_H
#define GEOMOUSE_H

#include "Geometry.h"

class ILONGSHARED_EXPORT GeoMouse : public Geometry
{
    Q_OBJECT
public:
    explicit GeoMouse(QPointF world, quint32 gsize = 80);
protected:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QPainterPath shape() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) Q_DECL_OVERRIDE;
    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
signals:

public slots:
};

#endif // GEOMOUSE_H
