#include "Layer.h"

Layer::Layer(ILong *parent, QString name, QList<LayerFormat> *typeList) : iLong(parent),
    layerLabel(name),visible(true),selectable(true),sqlExcute(&parent->sqlExcute)
{
    /*
     * 这是直接创建新图层并写入数据库
     * */
    layer = iLong->scene()->createItemGroup(list);
    QUuid id = QUuid::createUuid();
    layerID =  QString("ILONGIO%1").arg(id.data1);
    sqlExcute->initLayer(layerID,layerLabel,typeList, &headType);
}

Layer::Layer(ILong * parent,QString id, QString name, bool visible, bool selectable):
    iLong(parent),layerLabel(name),layerID(id),sqlExcute(&parent->sqlExcute)
{
    /*
     * 这得从数据库里读取表加载成图层
     * */
    this->visible = visible;
    this->selectable = selectable;
    layer = iLong->scene()->createItemGroup(list);
    QSqlQuery * query = sqlExcute->checkType(layerID);
    while(query->next())
    {
            QString value = query->value(2).toString();
            headType.append(value == "TEXT" ? ILongTEXT : ILongNUMBER);
    }
    delete query;
    query = 0;
}

Layer::~Layer()
{
    iLong->scene()->destroyItemGroup(layer);
    sqlExcute->removeLayer(layerID);
}

void Layer::addItem(QList<Geometry::ILongDataType> *dataList)
{
    sqlExcute->addItems(dataList,layerID, &headType);
}

void Layer::removeItem(Geometry *item)
{
    layer->removeFromGroup(item);
}

QList<QGraphicsItem *> *Layer::getItems()
{
    return &list;
}

QString Layer::getLayerName()
{
    return layerLabel;
}

QString Layer::getLayerID()
{
    return layerID;
}

void Layer::setVisible(bool b)
{
    visible = b;
    sqlExcute->setLayerVisible(layerID, b);
}

bool Layer::isVisible()
{
    return visible;
}

void Layer::setSelectable(bool b)
{
    selectable = b;
    sqlExcute->setLayerSelectable(layerID, b);
}

bool Layer::isSelectable()
{
    return selectable;
}
