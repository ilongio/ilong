#ifndef GEOTRI_H
#define GEOTRI_H

#include <QObject>
#include <math.h>
#include "Geometry.h"
class ILONGSHARED_EXPORT GeoTri : public Geometry
{
    Q_OBJECT
public:
    explicit GeoTri(QPointF world, int size = 80, QColor pen = QColor(Qt::blue), QColor brush = QColor(Qt::yellow));
protected:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QPainterPath shape() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) Q_DECL_OVERRIDE;
signals:

public slots:
};

#endif // GEOTRI_H
