#ifndef GEOPOLYGON_H
#define GEOPOLYGON_H

#include <QObject>
#include "Geometry.h"
#include "ILong.h"

class ILong;

class ILONGSHARED_EXPORT GeoPolygon : public Geometry
{
    Q_OBJECT
public:
    explicit GeoPolygon(ILong *iL, QList<QPointF> *pointList, bool closePath = false, quint8 lineWidth = 1, QColor pen = QColor(Qt::red), QColor brush = QColor(Qt::yellow));
protected:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QPainterPath shape();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) Q_DECL_OVERRIDE;
signals:

public slots:
private:
    ILong *iLong;
    bool needClosePath;
    int pHeight;
};

#endif // GEOPOLYGON_H
