#include "GeoMouse.h"

GeoMouse::GeoMouse(QPointF world): Geometry(iGeoMouse, LineNull, 1, 40,
                                                   QColor(qrand() % 256, qrand() % 256, qrand() % 256),
                                                   QColor(qrand() % 256, qrand() % 256, qrand() % 256))
{
    list.append(world);
    checkRect();
    startTimer(1000 / 200);
}

QRectF GeoMouse::boundingRect() const
{
    qreal adjust = 0.5;
    return QRectF(-18 - adjust, -22 - adjust, 36 + adjust, 60 + adjust);
}

QPainterPath GeoMouse::shape() const
{
    QPainterPath path;
    path.addRect(-10, -20, 20, 40);
    return path;
}

void GeoMouse::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(QColor(qrand() % 256, qrand() % 256, qrand() % 256));
    // Body
    painter->setBrush(QColor(qrand() % 256, qrand() % 256, qrand() % 256));
    painter->drawEllipse(-10, -20, 20, 40);

    // Eyes
    painter->setBrush(Qt::white);
    painter->drawEllipse(-10, -17, 8, 8);
    painter->drawEllipse(2, -17, 8, 8);

    // Nose
    painter->setBrush(Qt::black);
    painter->drawEllipse(QRectF(-2, -22, 4, 4));

    // Pupils
    int eysDir = qrand() % 3;
    painter->drawEllipse(QRectF(-8.0 + eysDir, -17, 4, 4));
    painter->drawEllipse(QRectF(4.0 + eysDir, -17, 4, 4));

    // Ears
    painter->setBrush(QColor(qrand() % 256, qrand() % 256, qrand() % 256));
    painter->drawEllipse(-17, -12, 16, 16);
    painter->drawEllipse(1, -12, 16, 16);

    // Tail
    QPainterPath path(QPointF(0, 20));
    path.cubicTo(-5, 22, -5, 22, 0, 25);
    path.cubicTo(5, 27, 5, 32, 0, 30);
    path.cubicTo(-5, 32, -5, 42, 0, 35);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path);

}

void GeoMouse::timerEvent(QTimerEvent *)
{
    update();
}
