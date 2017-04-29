#include "GeoPie.h"

GeoPie::GeoPie(QPointF world, int size, int dir, QColor pen, QColor brush) :
    Geometry(iGeoPie, LineNull, 1, size, pen, brush)
{
    list.append(world);
    this->dir = dir;
    checkRect();
}

QRectF GeoPie::boundingRect() const
{
    qreal adjust = 0.0;
    return QRectF(-size/2 - adjust, -size/2 - adjust, size + adjust, size + adjust);
}

QPainterPath GeoPie::shape() const
{
    QPainterPath path;
    path.addRect(-size/2, -size/2, size, size);
    return path;
}

void GeoPie::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawPie(-size/2, -size/2, size, size-2,70*16, 40*16);
    if(label.length())
    {
        QFont font = painter->font();
        font.setBold(true);
        //font.setPixelSize(12);
        painter->setFont(font);
        painter->setPen(brush);
        painter->drawText(-getLabelPixeSize()/2,-size/2-5,label);

    }
}
