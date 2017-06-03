#include "GeoPolygon.h"

GeoPolygon::GeoPolygon(ILong *iL, QList<QPointF> * pointList, bool closePath, quint8 lineWidth, QColor pen, QColor brush) :
    Geometry(iGeoPolygon,lineWidth, pen, brush),iLong(iL)
{
    closeFlag = closePath;
    if(pointList->size() == 0)
    {
        list.append(QPointF(0,0));
        list.append(QPointF(0,0));
    }
    else if(pointList->size() == 1)
    {
        list.append(pointList->at(0));
        list.append(pointList->at(0));
    }
    else
    {
        for(int i=0; i<pointList->size(); i++)
            list.append(pointList->at(i));
    }
    checkRect();
    QPointF minPoint = iLong->worldToScene(QPointF(rect.minX,rect.minY));
    QPointF maxPoint = iLong->worldToScene(QPointF(rect.maxX,rect.maxY));
    QLineF polygonWidth(QPointF(minPoint.x(),0),QPointF(maxPoint.x(),0));
    QLineF polygonHeight(QPointF(0,minPoint.y()),QPointF(0,maxPoint.y()));
    size = polygonWidth.length();
    pHeight = polygonHeight.length();
}

QRectF GeoPolygon::boundingRect() const
{
    return QRectF(0, 0, size, pHeight);
}

QPainterPath GeoPolygon::shape()
{
    QPointF telta = iLong->worldToScene(getCenter()) - QPointF(size/2, pHeight/2);
    QPainterPath path;
    QPolygonF polygon;
    for(int i=0; i<list.size(); i++)
        polygon.append(iLong->worldToScene(list.at(i))-telta);
    path.addPolygon(polygon);
    if(closeFlag)
        path.closeSubpath();
    return path;
}

void GeoPolygon::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    //painter->drawRect(boundingRect());
    QPen xPen(QBrush(pen),lineWidth);
    painter->setPen(xPen);
    painter->setRenderHint(QPainter::Antialiasing);
    if(closeFlag)
        painter->setBrush(brush);
    painter->drawPath(shape());
    if(label.length())
    {
        QFont font = painter->font();
        font.setBold(true);
        painter->setFont(font);
        painter->setPen(brush);
        painter->drawText((size-getLabelPixeSize())/2,pHeight/2,label);

    }
}
