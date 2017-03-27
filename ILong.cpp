#include "ILong.h"


ILong::ILong(QWidget *parent) : QGraphicsView(parent),itemScale(1),
    currentLevel(DEFAULTZOOMLEVEL),numberOfTiles(tilesOnZoomLevel(currentLevel)),
    defaultLocation(DEFAULTLOCATION),net(new Network(this)),
    tilesCount(0),currentPos(DEFAULTLOCATION)
{
    setStyleSheet("background-color:lightGray");
    setScene(new QGraphicsScene(this));
    manager = new Manager(this);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOff );
    setViewportUpdateMode(FullViewportUpdate);
    centerOn(0,0);
    setSceneRect(viewport()->rect());
    connect(this,SIGNAL(viewChangedSignal()),this,SLOT(viewChangedSlot()));
    net->moveToThread(&networkThread);
    connect(this,SIGNAL(downloadImage()),net,SLOT(start()));
    connect(net,SIGNAL(newImage()),this,SLOT(newImage()));
    connect(net,SIGNAL(sendTileCount(int)),this, SLOT(updateTilesCount(int)));
    connect(this,SIGNAL(sendLocationPos(QPointF)),this,SLOT(updateLocationPos(QPointF)));
    QList<LayerFormat> xlist;
    LayerFormat f1;
    f1.name = "id";
    f1.type = ILongNUMBER;
    LayerFormat f2;
    f2.name = "name";
    f2.type = ILongTEXT;
    xlist.append(f1);
    xlist.append(f2);
    manager->addLayer("xxx", &xlist);
}

ILong::~ILong()
{
    networkThread.exit(0);
    while(networkThread.isRunning())
        this->thread()->usleep(100);
}

quint8 ILong::maxZoomLevel()
{
    return MAXZOOMLEVEL;
}

quint8 ILong::minZoomLevel()
{
    return MINZOOMLEVEL;
}

quint8 ILong::zoomLevel()
{
    return currentLevel;
}

void ILong::zoomIn()
{
    if(checkZoomLevel(currentLevel + 1))
    {
        currentLevel++;
        zoomTo(sceneToWorld(mapToScene(zoomOnPos)),currentLevel,true);
    }
}

void ILong::zoomOut()
{
    if(checkZoomLevel(currentLevel - 1))
    {
        currentLevel--;
        zoomTo(sceneToWorld(mapToScene(zoomOnPos)),currentLevel,true);
    }
}

void ILong::setDefaultLocation(QPointF worldCoordinate, quint8 zoomLevel)
{
    if(checkWorldCoordinate(worldCoordinate) && checkZoomLevel(zoomLevel))
    {
        defaultLocation = worldCoordinate;
        currentLevel = zoomLevel;
        zoomTo(worldCoordinate,zoomLevel);
    }
    else
    {
        zoomTo(defaultLocation,currentLevel);
    }
}

QList<QString> *ILong::getImageList()
{
    return &list;
}

QString ILong::getServer()
{
    return map.getServer();
}

QList<Layer *> ILong::getLayers() const
{
    return manager->getLayers();
}

Layer *ILong::addLayer(QString name, QList<LayerFormat> *typeList) const
{
    if(!typeList->size())
        return nullptr;
    return manager->addLayer(name,typeList);
}

void ILong::removeLayer(QString name)
{
    manager->removeLayer(name);
}

bool ILong::viewportEvent(QEvent *event)
{
    switch(event->type())
    {
    case QEvent::Wheel:
    {
        QWheelEvent * wheelEvent = static_cast<QWheelEvent *>(event);;
        zoomOnPos = wheelEvent->pos();
        wheelEvent->delta() > 0 ? zoomIn() : zoomOut();
        wheelEvent->accept();
        return true;
        break;
    }
    case QEvent::MouseButtonPress:
    {
        QMouseEvent * pressEvent = static_cast<QMouseEvent *>(event);
        if(pressEvent->buttons() & Qt::LeftButton)
        {
            zoomOnPos = pressEvent->pos();
            pressEvent->accept();
            return true;
        }
        break;
    }
    case QEvent::MouseMove:
    {
        QMouseEvent * moveEvent = static_cast<QMouseEvent *>(event);
        emit sendLocationPos(sceneToWorld(mapToScene(moveEvent->pos())));
        if(moveEvent->buttons() & Qt::LeftButton)
        {

            QPoint moveDelta = moveEvent->pos() - zoomOnPos;
            setSceneLocation(QPointF(sceneRect().x() - moveDelta.x(),sceneRect().y() - moveDelta.y()));
            backgroundPos = backgroundPos + moveDelta;
            zoomOnPos = moveEvent->pos();
            moveEvent->accept();
            return true;
        }
        break;
    }
    case QEvent::MouseButtonRelease:
    {
        QMouseEvent * releaseEvent = static_cast<QMouseEvent *>(event);
        {
            if(releaseEvent->button() & Qt::LeftButton && zoomOnPos != QPoint(0,0))
            {
                emit viewChangedSignal();
                zoomOnPos = QPoint(0,0);
                releaseEvent->accept();
                return true;
            }
        }
        break;
    }
    default:
        break;
    }
    return QGraphicsView::viewportEvent(event);
}

void ILong::drawBackground(QPainter *p, const QRectF &rect)
{
    Q_UNUSED(rect);
    p->save();
    p->resetTransform();
    p->drawPixmap(backgroundPos,background);
    p->restore();
}

void ILong::drawForeground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);
    painter->save();
    painter->resetTransform();
    painter->setPen(QColor(Qt::red));
    QPoint p = viewport()->rect().center();
    painter->drawLine(p-QPoint(10,0), p+QPoint(10,0));
    painter->drawLine(p-QPoint(0,10), p+QPoint(0,10));
    QFont font = painter->font();
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(QColor(Qt::yellow));
    painter->drawText(QPoint(0,10),QString("Lng: %1").arg(currentPos.x(),0,'g',10));
    painter->drawText(QPoint(0,25),QString("Lat: %1").arg(currentPos.y(),0,'g',10));
    painter->drawText(QPoint(0,40),QString("Tile: %1").arg(tilesCount));
    painter->drawText(QPoint(0,55),QString("Level: %1").arg(currentLevel));

    painter->restore();
}

void ILong::resizeEvent(QResizeEvent *event)
{
    event->accept();
    zoomTo(defaultLocation,currentLevel);
}

QPointF ILong::worldToScene(QPointF world)
{
    world = ILoveChina::wgs84TOgcj02(world);
    return QPointF((world.x()+180) * (numberOfTiles*DEFAULTTILESIZE)/360.,
                  (1-(log(tan(PI/4.+degreeToRadian(world.y())/2.)) /PI)) /2.  * (numberOfTiles*DEFAULTTILESIZE));
}

QPointF ILong::sceneToWorld(QPointF scene)
{
    QPointF gcj = QPointF((scene.x()*(360./(numberOfTiles*DEFAULTTILESIZE)))-180,
                          radianToDegree(atan(sinh((1-scene.y()*(2./(numberOfTiles*DEFAULTTILESIZE)))*PI))));
    return ILoveChina::gcj02Towgs84(gcj);
}

qreal ILong::degreeToRadian(qreal value)
{
    return value * (PI/180.);
}

qreal ILong::radianToDegree(qreal value)
{
    return value * (180./PI);
}

int ILong::tilesOnZoomLevel(quint8 zoomLevel)
{
    return int(pow(2.0, zoomLevel));
}

void ILong::tilesUrlMatrix()
{
    QPointF sceneCenter = mapToScene(viewport()->rect().center());
    QPointF leftTopDelta = sceneCenter - mapToScene(QPoint(0,0));
    QPointF rightBottomDelta = mapToScene(QPoint(viewport()->width(),viewport()->height())) - sceneCenter;
    middle = QPoint(sceneCenter.x() / DEFAULTTILESIZE,sceneCenter.y() / DEFAULTTILESIZE);
    leftTop = QPoint(leftTopDelta.x() / DEFAULTTILESIZE + 1,leftTopDelta.y() / DEFAULTTILESIZE + 1);
    int rightTiles = rightBottomDelta.x() / DEFAULTTILESIZE + 1;
    int bottomTiles = rightBottomDelta.y() / DEFAULTTILESIZE + 1;
    background = QPixmap((leftTop.x() + rightTiles + 1) * DEFAULTTILESIZE,
                         (leftTop.y() + bottomTiles + 1) * DEFAULTTILESIZE);
    background.fill(QColor(Qt::lightGray));
    backgroundPos = mapFromScene((-leftTop.x()+middle.x())*DEFAULTTILESIZE,
                                 (-leftTop.y()+middle.y())*DEFAULTTILESIZE);
    QMultiMap<int, int> tList;
    for(int x=-leftTop.x()+middle.x(); x<=rightTiles+middle.x(); x++)
    {
        for(int y=-leftTop.y()+middle.y(); y<=bottomTiles+middle.y(); y++)
        {
            if(map.isTileValid(x,y,currentLevel))
            {
                tList.insertMulti(x,y);
            }

        }
    }
    QSqlQuery * query = sqlExcute.checkImage(leftTop.x()+middle.x(), -rightTiles+middle.x(),
                                             leftTop.y()+middle.y(), -bottomTiles+middle.y(), currentLevel);
    bool checkImageError = false;
    if(query == nullptr)
    {
        while(!tList.isEmpty())
        {
            int key = tList.firstKey();
            QString path = map.queryTile(key,tList.first(),currentLevel);
            list.contains(path) ? list.move(list.indexOf(path),0) : list.insert(0,path);
            tList.remove(key,tList.first());
        }
        checkImageError = true;
    }
    while (query->next() && !checkImageError)
    {
        int x = query->value(0).toInt();
        int y = query->value(1).toInt();
        //int z = query->value(2).toInt();
        QPixmap pm;
        pm.loadFromData(query->value(3).toByteArray());
        QPainter painter(&background);
        painter.drawPixmap((x+leftTop.x()-middle.x())*DEFAULTTILESIZE
                           ,(y+leftTop.y()-middle.y())*DEFAULTTILESIZE
                           ,DEFAULTTILESIZE,DEFAULTTILESIZE,pm);
        painter.end();
        tList.remove(x,y);
    }
    if(query)
    {
        delete query;
        query = 0;
    }
    while(!tList.isEmpty() && !checkImageError)
    {
        int key = tList.firstKey();
        int value = tList.first();
        QString path = map.queryTile(key, value, currentLevel);
        list.contains(path) ? list.move(list.indexOf(path),0) : list.insert(0,path);
        tList.remove(key, value);
    }
    if(!networkThread.isRunning())
        networkThread.start();
    if(!net->getDownloadState())
        emit downloadImage();
}

void ILong::zoomTo(QPointF world, quint8 zoomLevel, bool underMouse)
{
    currentLevel = zoomLevel;
    numberOfTiles = tilesOnZoomLevel(currentLevel);
    itemScale = currentLevel * 1. / MAXZOOMLEVEL ;
    setSceneLocation(worldToScene(world) -
                     (underMouse ? zoomOnPos : viewport()->rect().center()),
                     true);

}

void ILong::setSceneLocation(QPointF topLeftPos, bool updateItem)
{
    setSceneRect(topLeftPos.x(),topLeftPos.y(),viewport()->width(), viewport()->height());
    defaultLocation = sceneToWorld(mapToScene(viewport()->rect().center()));
    if(updateItem)
        emit viewChangedSignal();
}

bool ILong::checkZoomLevel(quint8 zoomLevel)
{
    return (zoomLevel>=MINZOOMLEVEL && zoomLevel<=MAXZOOMLEVEL);
}

bool ILong::checkWorldCoordinate(QPointF world)
{
    return (world.x() >= -180 && world.x() <= 180 && world.y() >= -85 && world.y() <= 85);
}

void ILong::viewChangedSlot()
{
    scene()->clear();
    tilesUrlMatrix();
    QGraphicsRectItem * r = new QGraphicsRectItem(0,0,10,20);
    r->setPen(QColor(Qt::red));
    r->setPos(worldToScene(QPointF(99.70875,27.82188)));
    r->setScale(itemScale);
    scene()->addItem(r);
    QGraphicsTextItem * tpx = new QGraphicsTextItem("YYYYY");
    tpx->setDefaultTextColor(QColor(Qt::yellow));
    tpx->setPos(worldToScene(QPointF(99.70875,27.82188)));
    tpx->setScale(itemScale);
    scene()->addItem(tpx);
    QGraphicsTextItem * tx = new QGraphicsTextItem("mmmm");
    tx->setPos(worldToScene(QPointF(99.80875,27.72188)));
    tx->setScale(itemScale);
    tx->setDefaultTextColor(QColor(Qt::green));
    scene()->addItem(tx);
//    QGraphicsTextItem * px = new QGraphicsTextItem("center");
//    px->setDefaultTextColor(QColor(Qt::red));
//    px->setPos(worldToScene(defaultLocation));
//    px->setScale(itemScale);
//    scene()->addItem(px);
}

void ILong::newImage()
{
    viewport()->update();
}

void ILong::updateTilesCount(int count)
{
    tilesCount = count;
    viewport()->update();
}

void ILong::updateLocationPos(QPointF world)
{
    currentPos = world;
    viewport()->update();
}

