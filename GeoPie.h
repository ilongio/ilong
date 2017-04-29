#ifndef GEOPIE_H
#define GEOPIE_H

#include <QObject>
#include <QDebug>

#include "Geometry.h"

class ILONGSHARED_EXPORT GeoPie : public Geometry
{
    Q_OBJECT
public:
    explicit GeoPie(QPointF world, int size = 80, int dir = 0, QColor pen = QColor(Qt::blue), QColor brush = QColor(Qt::yellow));
protected:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QPainterPath shape() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) Q_DECL_OVERRIDE;
signals:

public slots:
};

#endif // GEOPIE_H
