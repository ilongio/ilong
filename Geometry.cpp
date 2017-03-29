#include "Geometry.h"

Geometry::Geometry(ILongGeoType gType, ILongLineType lType, quint8 lWidth, quint8 pSize,QColor iPen,QColor iBrush)
{
    geoType = gType;
    lineType = lType;
    lineWidth = lWidth;
    pen = iPen;
    brush = iBrush;
    size = pSize;
    QUuid id = QUuid::createUuid();
    itemID = id.data1;
    label = "";
}

ILongGeoRect Geometry::getRect()
{
    return rect;
}

ILongGeoType Geometry::getGeoType()
{
    return geoType;
}

ILongLineType Geometry::getLineType()
{
    return lineType;
}

QPointF Geometry::getCenter()
{
    return list.at(0);
}

QString Geometry::getPen()
{
    return QString("%1_%2_%3").arg(pen.red()).arg(pen.green()).arg(pen.blue());
}

QString Geometry::getBrush()
{
    return QString("%1_%2_%3").arg(brush.red()).arg(brush.green()).arg(brush.blue());
}

QString Geometry::getPoints()
{
    QString result = "";
    for(int i=0; i<list.size(); i++)
    {
        QPointF p = list.at(i);
        result += QString("%1,%2_").arg(p.x()).arg(p.y());
    }
    return result.left(result.length()-1);
}

quint8 Geometry::getLineWidth()
{
    return lineWidth;
}

quint8 Geometry::getSize()
{
    return size;
}

quint32 Geometry::getID()
{
    return itemID;
}

void Geometry::rotate(int dir)
{
    setTransform(QTransform().rotate(dir), true);
}

void Geometry::setLabel(QString lb)
{
    label = lb;
}

int Geometry::getLabelPixeSize()
{
    QLabel lb;
    return lb.fontMetrics().width(label);
}

void Geometry::checkRect()
{
    if(list.size() == 0)
        list.append(QPointF(0,0));
    if(list.size() == 1)
    {
        if(geoType != iGeoPolygon)
        {
            rect.minX = list.at(0).x();
            rect.minY = list.at(0).y();
            rect.maxX = list.at(0).x();
            rect.maxY = list.at(0).y();
            return;
        }
        list.append(QPointF(0,0));
    }
    for(int i=1; i<list.size(); i++)
    {
        QPointF p1 = list.at(i-1);
        QPointF p2 = list.at(i);
        if(p1.x() > p2.x())
        {
            rect.maxX = p1.x();
            rect.minX = p2.x();
        }
        else
        {
            rect.maxX = p2.x();
            rect.minX = p1.x();
        }
        if(p1.y() > p2.y())
        {
            rect.maxY = p1.y();
            rect.minY = p2.y();
        }
        else
        {
            rect.maxY = p2.y();
            rect.minY = p1.y();
        }
    }
}
