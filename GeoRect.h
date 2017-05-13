#ifndef GEORECT_H
#define GEORECT_H

#include <QObject>
#include "Geometry.h"

class ILONGSHARED_EXPORT GeoRect : public Geometry
{
    Q_OBJECT
public:
    explicit GeoRect(QPointF world, int size = 80, QColor pen = QColor(Qt::blue), QColor brush = QColor(Qt::yellow));
protected:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QPainterPath shape() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) Q_DECL_OVERRIDE;
signals:

public slots:
};

#endif // GEORECT_H
