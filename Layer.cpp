#include "Layer.h"

Layer::Layer(ILong *parent, QString name, QList<LayerFormat> *typeList) : iLong(parent),
    layerLabel(name),visible(true),selectable(true),sqlExcute(&parent->sqlExcute)
{
    /*
     * 这是直接创建新图层并写入数据库
     * */
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
    QSqlQuery * query = sqlExcute->checkType(layerID);
    while(query->next())
    {
        QString value = query->value(1).toString();
        QString type = query->value(2).toString();
        LayerFormat t;
        t.name = value;
        t.type = type == "TEXT" ? ILongTEXT : ILongNUMBER;
        headType.append(t);
    }
    delete query;
    query = 0;
}

Layer::~Layer()
{
    sqlExcute->removeLayer(layerID);
}

void Layer::addItem(QList<Geometry::ILongDataType> *dataList)
{
    sqlExcute->addItems(dataList,layerID, &headType);
}

QList<Geometry *> * Layer::getItems()
{
    return &list;
}

void Layer::removeItem(Geometry *item)
{
    iLong->scene()->removeItem(item);
}

void Layer::updatLayer()
{
    list.clear();
    QPointF leftTop = iLong->sceneToWorld(iLong->mapToScene(QPoint(0,0)));
    QPointF rightBottom = iLong->sceneToWorld(iLong->mapToScene(QPoint(iLong->viewport()->width(),
                                                                       iLong->viewport()->height())));
    QSqlQuery * query =sqlExcute->updateLayer(layerID,leftTop,rightBottom, iLong->getItemLimit());
    while(query->next())
    {
        int type = query->value(1).toInt();
        switch (type) {
        case iGeoPie:
            addGeoPie(query);
            break;
        default:
            break;
        }
    }
    delete query;
    query = 0;
}

void Layer::setLabel(QString field)
{
    sqlExcute->setLabel(layerID, field);
}


QString Layer::getLayerName()
{
    return layerLabel;
}

QString Layer::getLayerID()
{
    return layerID;
}

QList<LayerFormat> *Layer::getLayerHead()
{
    return &headType;
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

Layer::ILongInfo Layer::getInfo(QSqlQuery *query)
{
    ILongInfo info;
    info.id = query->value(0).toDouble();
    info.center = QPointF(query->value(2).toDouble(),query->value(3).toDouble());
    /*
     * 默认 ILONGNULL
     * */
    info.label =  query->value(8).toString();
    QStringList gis = query->value(9).toString().split('-');
    info.list = getGisList(gis.at(0));
    info.width = gis.at(1).toInt();
    info.lineDir = gis.at(2).toInt();
    info.size = gis.at(3).toInt();
    QStringList iPen = gis.at(4).split('_');
    info.pen = QColor(iPen.at(0).toInt(),iPen.at(1).toInt(),iPen.at(2).toInt());
    iPen = gis.at(5).split('_');
    info.brush = QColor(iPen.at(0).toInt(),iPen.at(1).toInt(),iPen.at(2).toInt());
    info.dir = gis.at(6).toInt();
    info.close = gis.at(7).toInt();
    return info;
}

QList<QPointF> Layer::getGisList(QString gis)
{
    QList<QPointF> l;
    QStringList tl = gis.split('_');
    while (!tl.isEmpty())
    {
        QString str = tl.first();
        QStringList tmp = str.split(',');
        l.append(QPointF(tmp.at(0).toDouble(),tmp.at(1).toDouble()));
        tl.removeFirst();
    }
    return l;
}

void Layer::addGeoPie(QSqlQuery *query)
{
    /*
     *创建信息表,专用保存图元的,应该可以直接保存图元,但是现在还不知道怎么弄,就先这样弄吧,以后再想办法改进(个人技术原因),主要信息有:
     * @ILONGID  0   与数据的ID关联;
     * @TYPE     1   ILongGeoType 枚举图元类型
     * @CenterX  2   图元wgs CenterX 坐标
     * @CenterY  3   图元wgs CenterX 坐标
     * @MINX     4   图元最小wgs X坐标 (点类图元写CenterX相同)
     * @MINY     5   图元最小wgs X坐标 (点类图元写CenterY相同)
     * @MAXX     6   图元最大wgs X坐标 (点类图元写CenterX相同)
     * @MAXY     7   图元最大wgs Y坐标 (点类图元写CenterY相同) 设计两个坐标点只为了非点类图元需要计算边界问题,比如线
     * @LABEL    8   用来显示图标注的, 如果设置显示标注,就从数据表里面把标注内容填充到该字段
     * @INFO     9   保存图元GIS信息
     * 格式: 0WGSx1,WGSy1_WGSx2,WGSy2_..._WGSxN,WGSyN-1线宽-2箭头方向-3大小-4画笔(R_G_B)-5画刷(R_G_B)-6旋转角度-7闭环
     *              箭头方向 闭环    只对面类图元有影响 闭环(如果true 就是多边行, false 就是多段线)
     *              旋转角度 大小    只对点类图元有影响
     *
    */
    ILongInfo itemInfo = getInfo(query);
    GeoPie * p = new GeoPie(itemInfo.center,itemInfo.size,itemInfo.dir,itemInfo.pen,itemInfo.brush);
    p->setPos(iLong->worldToScene(itemInfo.center));
    if(itemInfo.label != "ILONGNULL")
        p->setLabel(itemInfo.label);
    p->setObjectName(QString("%1_%2").arg(layerID).arg(itemInfo.id));
    p->setScale(iLong->itemScale);
    p->rotate(itemInfo.dir);
    p->setFlag(QGraphicsItem::ItemIsFocusable);
    iLong->scene()->addItem(p);
    list.append(p);

}
