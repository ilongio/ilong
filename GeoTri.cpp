#include "GeoTri.h"

GeoTri::GeoTri(QPointF world, int size, QColor pen, QColor brush) :
    Geometry(iGeoTri, LineNull, 1, size, pen, brush)
{
    list.append(world);
    checkRect();
}

QRectF GeoTri::boundingRect() const
{
    return QRectF(-size/2, -size/2, size, size);
}

QPainterPath GeoTri::shape() const
{
    QPainterPath path;
    QPolygonF polygon;
    qreal r = size/2;
    polygon.append(QPointF(0,-r+r/2));
    polygon.append(QPointF(-r/2,r*sin(60)+r/2));
    polygon.append(QPointF(r/2,r*sin(60)+r/2));
    path.addPolygon(polygon);
    return path;
}

void GeoTri::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(pen);
    painter->setBrush(brush);
    QPolygonF polygon;
    qreal r = size/2;
    polygon.append(QPointF(0,-r+r/2));
    polygon.append(QPointF(-r/2,r*sin(60)+r/2));
    polygon.append(QPointF(r/2,r*sin(60)+r/2));
    painter->drawPolygon(polygon);
    if(label.length())
    {
        QFont font = painter->font();
        font.setBold(true);
        painter->setFont(font);
        painter->setPen(brush);
        painter->drawText(-getLabelPixeSize()/2,size+5,label);
    }

}
