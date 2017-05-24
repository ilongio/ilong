#include "GeoMouse.h"

GeoMouse::GeoMouse(QPointF world, quint32 gsize): Geometry(iGeoMouse, gsize * 0.3,
                                                   QColor(qrand() % 256, qrand() % 256, qrand() % 256),
                                                   QColor(qrand() % 256, qrand() % 256, qrand() % 256))
{
    list.append(world);
    checkRect();
    startTimer(200);
}

QRectF GeoMouse::boundingRect() const
{
    return QRectF(-size/2, -size/2, size, size);
}

QPainterPath GeoMouse::shape() const
{
    QPainterPath path;
    path.addEllipse(-size/2, -size/2, size, size);
    return path;
}

void GeoMouse::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(QColor(qrand() % 256, qrand() % 256, qrand() % 256));
    painter->setBrush(QColor(qrand() % 256, qrand() % 256, qrand() % 256));
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawEllipse(-size/2, -size/2, size, size);
    if(label.length())
    {
        QFont font = painter->font();
        font.setBold(true);
        //font.setPixelSize(12);
        painter->setFont(font);
        painter->setPen(brush);
        painter->drawText(-getLabelPixeSize()/2,size+5,label);

    }

}

void GeoMouse::timerEvent(QTimerEvent *)
{
    update();
}
