#include "GeoCircle.h"

GeoCircle::GeoCircle(QPointF world, int size,  QColor pen, QColor brush) :
    Geometry(iGeoCircle, LineNull, 1, size*0.6, pen, brush)
{
    list.append(world);
    checkRect();
}

QRectF GeoCircle::boundingRect() const
{
    return QRectF(-size/2, -size/2, size, size);
}

QPainterPath GeoCircle::shape() const
{
    QPainterPath path;
    path.addEllipse(-size/2, -size/2, size, size);
    return path;
}

void GeoCircle::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(pen);
    painter->setBrush(brush);
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
