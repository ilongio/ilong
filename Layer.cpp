#include "Layer.h"

Layer::Layer(ILong *parent, QString name, QList<LayerFormat> *typeList) : iLong(parent),
    layerLabel(name),visible(true),selectable(false),sqlExcute(&parent->sqlExcute)
{
    /*
     * 这是直接创建新图层并写入数据库
     * */
    QUuid id = QUuid::createUuid();
    layerID =  QString("ILONGIO%1").arg(id.data1);
    LayerFormat f;
    f.name = "ILONGID";
    f.type = ILongNUMBER;
    headType.append(f);
    sqlExcute->initLayer(layerID,layerLabel,typeList, &headType);
    connect(this, SIGNAL(addGeoToScene(Geometry*)), iLong, SLOT(addGeoToScene(Geometry*)));
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
    connect(this, SIGNAL(addGeoToScene(Geometry*)), iLong, SLOT(addGeoToScene(Geometry*)));
}

Layer::~Layer()
{
    sqlExcute->removeLayer(layerID);
}

QSqlQuery *Layer::searchInfo(QString field, QString text)
{
    ILongType t = ILongTEXT;
    for(int i=0; i<headType.size();i++)
    {
        if(headType.at(i).name == field)
        {
            t = headType.at(i).type;
            break;
        }
    }
    if(t == ILongNUMBER)
    {
        bool ok;
        text.toDouble(&ok);
        if(!ok)
            return nullptr;
    }
    return sqlExcute->searchInfo(layerID,field,t,text);
}

void Layer::setViewToItem(QString itemID)
{
    QSqlQuery * query = sqlExcute->setViewToItem(getLayerID(),itemID);
    while(query->next())
    {
        bool ok;
        double x = query->value(0).toDouble(&ok);
        if(!ok)
            break;
        double y = query->value(1).toDouble(&ok);
        if(!ok)
            break;
        iLong->zoomTo(QPointF(x,y),iLong->zoomLevel());
        break;
    }
    delete query;
    query = 0;
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

void Layer::addTempItem(ILongGeoType type, QPointF world, quint32 dir)
{
    tempGeoType = type;
    tempGeoWorldPos = world;
    updateTempItem(dir);
}

void Layer::updateTempItem(quint32 dir)
{
    if(tempGeo)
    {
        tempGeo->rotate(dir);
        tempGeo->setPos(iLong->worldToScene(tempGeoWorldPos));
        return;
    }
    switch (tempGeoType) {
    case iGeoCircle:
        tempGeo = new GeoRect(tempGeoWorldPos);
        break;
    case iGeoRect:
        tempGeo = new GeoRect(tempGeoWorldPos);
        break;
    case iGeoPie:
        tempGeo = new GeoPie(tempGeoWorldPos);
        break;
    case iGeoMouse:
        tempGeo = new GeoMouse(tempGeoWorldPos);
        break;
    case iGeoStar:
        tempGeo = new GeoStar(tempGeoWorldPos);
        break;
    case iGeoTri:
        tempGeo = new GeoTri(tempGeoWorldPos);
        break;
    default:
        break;
    }
    if(tempGeo)
    {
        tempGeo->setPos(iLong->worldToScene(tempGeoWorldPos));
        tempGeo->setScale(iLong->itemScale);
        tempGeo->rotate(dir);
        //iLong->scene()->addItem(tempGeo);
        emit addGeoToScene(tempGeo);
    }
}

void Layer::updatLayer(bool *isUpdate)
{
    list.clear();
    tempGeo = nullptr;
    if(!*isUpdate)
        return;
    QPointF leftTop = iLong->sceneToWorld(iLong->mapToScene(QPoint(0,0)));
    QPointF rightBottom = iLong->sceneToWorld(iLong->mapToScene(QPoint(iLong->viewport()->width(),
                                                                       iLong->viewport()->height())));
    QSqlQuery * query =sqlExcute->updateLayer(layerID,leftTop,rightBottom, iLong->getItemLimit());
    while(query->next() && *isUpdate)
    {
        int type = query->value(1).toInt();
        Geometry * g = nullptr;
        ILongInfo itemInfo = getInfo(query);
        switch (type) {
        case iGeoCircle:
            g = new GeoCircle(itemInfo.center,itemInfo.size,itemInfo.pen,itemInfo.brush);
            break;
        case iGeoRect:
            g = new GeoRect(itemInfo.center,itemInfo.size,itemInfo.pen,itemInfo.brush);
            break;
        case iGeoMouse:
            g = new GeoMouse(itemInfo.center);
            break;
        case iGeoPie:
            g = new GeoPie(itemInfo.center,itemInfo.size,itemInfo.dir,itemInfo.pen,itemInfo.brush);
            break;
        case iGeoStar:
            g = new GeoStar(itemInfo.center,itemInfo.size,itemInfo.pen,itemInfo.brush);
            break;
        case iGeoTri:
            g = new GeoTri(itemInfo.center,itemInfo.size,itemInfo.pen,itemInfo.brush);
            break;
        case iGeoPolygon:
            g = new GeoPolygon(iLong,&itemInfo.list,itemInfo.close,itemInfo.width,itemInfo.pen,itemInfo.brush);
            break;
        default:
            break;
        }
        if(g && *isUpdate)
        {
            if(type == iGeoPolygon)
            {
                g->setPos(iLong->worldToScene(QPointF(g->getRect().minX,g->getRect().maxY)));
                //g->setFlag(QGraphicsItem::ItemIsSelectable);
            }
            else
            {
                g->setPos(iLong->worldToScene(itemInfo.center));
                g->setScale(iLong->itemScale);
                g->rotate(itemInfo.dir);
            }
            if(itemInfo.label != "ILONGNULL")
                g->setLabel(itemInfo.label);
            g->setObjectName(QString("%1_%2").arg(layerID).arg(itemInfo.id));
            //iLong->scene()->addItem(g);
            emit addGeoToScene(g);
            list.append(g);
        }
    }
    delete query;
    query = 0;
    //updateTempItem();
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
    info.size = gis.at(2).toInt();
    QStringList iPen = gis.at(3).split('_');
    info.pen = QColor(iPen.at(0).toInt(),iPen.at(1).toInt(),iPen.at(2).toInt());
    iPen = gis.at(4).split('_');
    info.brush = QColor(iPen.at(0).toInt(),iPen.at(1).toInt(),iPen.at(2).toInt());
    info.dir = gis.at(5).toInt();
    info.close = gis.at(6).toInt();
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

