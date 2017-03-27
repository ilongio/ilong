#ifndef LAYER_H
#define LAYER_H

#include <QObject>
#include <QGraphicsItemGroup>

#include "ILong.h"
#include "SQLExcute.h"

class ILong;
class Layer : public QObject
{
    Q_OBJECT
public:
    explicit Layer(ILong * parent,QString name, QList<LayerFormat> * typeList);
    Layer(ILong * parent, QString id, QString name, bool visible, bool selectable);
    ~Layer();
    void addItem(QGraphicsItem *item);
    void removeItem(QGraphicsItem *item);
    QList<QGraphicsItem *> * getItems();
    QString getLayerName();
    QString getLayerID();
    void setVisible(bool b);
    bool isVisible();
    void setSelectable(bool b);
    bool isSelectable();
private:
    ILong * iLong;
    QGraphicsItemGroup * layer;
    QString layerLabel;
    QList<QGraphicsItem *> list;
    QString layerID;
    bool visible;
    bool selectable;
    SQLExcute * sqlExcute;
    QList <ILongType> headType;
signals:

public slots:
};

#endif // LAYER_H
