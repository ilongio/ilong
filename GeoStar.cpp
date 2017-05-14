#include "GeoStar.h"

GeoStar::GeoStar(QPointF world, int size, QColor pen, QColor brush) :
    Geometry(iGeoStar, LineNull, 1, size*0.6, pen, brush)
{
    list.append(world);
    checkRect();
}

QRectF GeoStar::boundingRect() const
{
    return QRectF(-size/2, -size/2, size, size);
}

QPainterPath GeoStar::shape() const
{
    QPainterPath path;
    QPolygonF polygon;
    qreal r = size/2;
    polygon.append(QPointF(0,-r));
    polygon.append(QPointF(-r*sin(36),r/3*sin(36)));//右上
    polygon.append(QPointF(r*cos(18),-r*sin(18)));//右下
    polygon.append(QPointF(-r*cos(18),-r*sin(18)));//左下
    polygon.append(QPointF(r*sin(36),r/3*sin(36)));//左上
    path.addPolygon(polygon);
    path.closeSubpath();
    return path;
}

void GeoStar::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(pen);
    painter->setBrush(brush);
    QPolygonF polygon;
    qreal r = size/2;
    polygon.append(QPointF(0,-r));
    polygon.append(QPointF(-r*sin(36),r/3*sin(36)));//右上
    polygon.append(QPointF(r*cos(18),-r*sin(18)));//右下
    polygon.append(QPointF(-r*cos(18),-r*sin(18)));//左下
    polygon.append(QPointF(r*sin(36),r/3*sin(36)));//左上
    painter->drawPolygon(polygon,Qt::WindingFill);
    if(label.length())
    {
        QFont font = painter->font();
        font.setBold(true);
        painter->setFont(font);
        painter->setPen(brush);
        painter->drawText(-getLabelPixeSize()/2,size+5,label);

    }
}
