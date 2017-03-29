#ifndef GEOMOUSE_H
#define GEOMOUSE_H

#include "Geometry.h"

class GeoMouse : public Geometry
{
    Q_OBJECT
public:
    explicit GeoMouse(QPointF world);
protected:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QPainterPath shape() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) Q_DECL_OVERRIDE;
signals:

public slots:
};

#endif // GEOMOUSE_H
