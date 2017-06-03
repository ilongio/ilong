#ifndef GEOSTAR_H
#define GEOSTAR_H

#include <QObject>
#include "Geometry.h"
#include <math.h>

class ILONGSHARED_EXPORT GeoStar : public Geometry
{
    Q_OBJECT
public:
    explicit GeoStar(QPointF world, int size = 80, QColor pen = QColor(Qt::blue), QColor brush = QColor(Qt::yellow));
protected:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QPainterPath shape() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) Q_DECL_OVERRIDE;
signals:

public slots:
};

#endif // GEOSTAR_H
