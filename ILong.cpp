#include "ILong.h"


ILong::ILong(QWidget *parent) : QGraphicsView(parent),itemScale(1),
    currentLevel(DEFAULTZOOMLEVEL),numberOfTiles(tilesOnZoomLevel(currentLevel)),
    defaultLocation(DEFAULTLOCATION),net(new Network(this)),
    tilesCount(0),currentPos(DEFAULTLOCATION),itemLimit(DEFAULTITEMLIMITPERLAYER),
    satellitesCount(0),GPSAltitude(0),GPSDir(0)
{
    QSqlQuery * query = sqlExcute.getDefaultLoaction();
    while (query->next())
    {
        QString fieldName = query->value(0).toString();
        if(fieldName == "X")
        {
            defaultLocation.setX(query->value(1).toDouble());
            currentPos.setX(query->value(1).toDouble());
        }
        if(fieldName == "Y")
        {
            defaultLocation.setY(query->value(1).toDouble());
            currentPos.setY(query->value(1).toDouble());
        }
        if(fieldName == "LEVEL")
        {
            currentLevel = query->value(1).toInt();
            numberOfTiles= tilesOnZoomLevel(currentLevel);
        }
        if(fieldName == "LIMIT")
        {
            itemLimit = query->value(1).toInt();
        }
    }
    delete query;
    //distanceList<<5000000<<2000000<<1000000<<1000000<<1000000<<100000<<100000<<50000<<50000<<10000<<10000<<10000<<1000<<1000<<500<<200<<100<<50<<20;
    distanceList<<5000000<<2000000<<1000000<<500000<<200000<<100000<<50000<<20000<<10000<<5000<<2000<<1000<<500<<200<<100<<50<<20<<10<<5;
    setStyleSheet("background-color:rgb(236,236,236)");
    setScene(new QGraphicsScene(this));
    /*
     * @manager得在QGraphicsScene初始化之后才能使用,所以在这里初始化
     * */
    manager = new Manager(this);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOff );
    setViewportUpdateMode(FullViewportUpdate);
    centerOn(0,0);
    //grabGesture(Qt::PinchGesture);
    //viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
    //viewport()->grabGesture(Qt::PinchGesture);
    setSceneRect(viewport()->rect());
    /*
     * 处理刷新信号
     * */
    connect(this,SIGNAL(viewChangedSignal()),this,SLOT(viewChangedSlot()));
    net->moveToThread(&networkThread);
    /*
     * 处理下载信号
     * */
    connect(this,SIGNAL(downloadImage()),net,SLOT(start()));
    connect(net,SIGNAL(newImage()),this,SLOT(newImage()));
    connect(net,SIGNAL(sendTileCount(int)),this, SLOT(updateTilesCount(int)));

    manager->moveToThread(&updateThread);
    connect(this, SIGNAL(updateLayer()), manager, SLOT(updatLayer()));
    /*
     * 处理当前世界坐标位置信号
     * */
    connect(this,SIGNAL(sendLocationPos(QPointF)),this,SLOT(updateLocationPos(QPointF)));
//    resetMatrix();
//    scale(1,1);
    QList <LayerFormat> fm;
    fm << LayerFormat{"X",ILongNUMBER} << LayerFormat{"Y",ILongNUMBER} << LayerFormat{"VALUE",ILongNUMBER} << LayerFormat{"NAME",ILongTEXT};
    tempLayer = manager->addLayer("iLongio", &fm);
}

ILong::~ILong()
{
    networkThread.exit(0);
    while(networkThread.isRunning())
        this->thread()->usleep(100);
    updateThread.exit(0);
    while(updateThread.isRunning())
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
        zoomOnPos = viewport()->rect().center();
        zoomTo(sceneToWorld(mapToScene(zoomOnPos)),currentLevel,true);
    }
}

void ILong::zoomOut()
{
    if(checkZoomLevel(currentLevel - 1))
    {
        currentLevel--;
        zoomOnPos = viewport()->rect().center();
        zoomTo(sceneToWorld(mapToScene(zoomOnPos)),currentLevel,true);
    }
}

void ILong::setDefaultLocation(QPointF worldCoordinate, quint8 zoomLevel)
{
    if(checkWorldCoordinate(worldCoordinate) && checkZoomLevel(zoomLevel))
    {
        defaultLocation = worldCoordinate;
        currentLevel = zoomLevel;
    }
    sqlExcute.updateDefaultLoaction(defaultLocation,currentLevel);
    zoomTo(defaultLocation,currentLevel);
}

QPointF ILong::getDefaultLocation()
{
    return defaultLocation;
}

QList<Layer *> ILong::getLayers() const
{
    return manager->getLayers();
}

Layer *ILong::getlayer(QString name) const
{
    return manager->getLayer(name);
}

Layer *ILong::getLayerByID(QString ID) const
{
    return manager->getLayerByID(ID);
}

Layer *ILong::addLayer(QString name, QList<LayerFormat> *typeList) const
{
    if(!typeList->size() || name.isEmpty())
        return nullptr;
    return manager->addLayer(name,typeList);
}

void ILong::removeLayer(QString name)
{
    manager->removeLayer(name);
}

void ILong::addTempGeo(QPointF world, ILongGeoType type)
{

    Geometry * g = nullptr;
    QColor pen = QColor(qrand()%255,qrand()%255,qrand()%255);
    QColor brush = QColor(qrand()%255,qrand()%255,qrand()%255);
    switch (type) {
    case iGeoCircle:
        g = new GeoCircle(world,80,pen,brush);
        break;
    case iGeoRect:
        g = new GeoRect(world,80,pen,brush);
        break;
    case iGeoPie:
        g = new GeoPie(world,80,0,pen,brush);
        break;
    case iGeoStar:
        g = new GeoStar(world,80,pen,brush);
        break;
    case iGeoTri:
        g = new GeoTri(world,80,pen,brush);
        break;
    default:
        break;
    }
    if(g)
    {
        Geometry::ILongDataType t;
        t.geometry = g;
        t.data << world.x() << world.y() << 0 << "iLong";
        tempLayer->addGeo(t);
        zoomTo(world,zoomLevel());
        delete g;
        g = nullptr;
    }
}
QPointF ILong::worldToScene(QPointF world)
{
    world = ILoveChina::wgs84TOgcj02(world);
    return QPointF((world.x()+180) * (numberOfTiles*DEFAULTTILESIZE)/360.,
                  (1-(log(tan(PI/4.+degreeToRadian(world.y())/2.)) /PI)) /2.  * (numberOfTiles*DEFAULTTILESIZE));
}

QPointF ILong::sceneToWorld(QPointF scene)
{

    return ILoveChina::gcj02Towgs84(QPointF((scene.x()*(360./(numberOfTiles*DEFAULTTILESIZE)))-180,
                                            radianToDegree(atan(sinh((1-scene.y()*(2./(numberOfTiles*DEFAULTTILESIZE)))*PI)))));
}

void ILong::setItemLimit(quint32 limit)
{
    itemLimit = limit;
    sqlExcute.updateItemLimit(limit);
}

quint32 ILong::getItemLimit()
{
    return itemLimit;
}

void ILong::goToDefaultLocation()
{
    zoomTo(currentPos,zoomLevel());
}

bool ILong::moveLayerTo(QString name, bool back)
{
    return manager->moveLayer(name, back);
}

void ILong::setViewOffset(int deltaX, int deltaY)
{
    setSceneLocation(QPointF(sceneRect().x() + deltaX, sceneRect().y() + deltaY));
    emit viewChangedSignal();
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
    case QEvent::MouseButtonDblClick:
    {
        QMouseEvent * doubleClickEvent = static_cast<QMouseEvent *>(event);
        emit doubleClicked(doubleClickEvent->pos());
        doubleClickEvent->accept();
        return true;
    }
    case QEvent::MouseButtonPress:
    {
        QMouseEvent * pressEvent = static_cast<QMouseEvent *>(event);
        pressEvent->accept();
        if(pressEvent->buttons() & Qt::LeftButton)
        {
            zoomOnPos = pressEvent->pos();
            mouseMove = false;

            QPointF point = mapToScene(pressEvent->pos());
            if (scene()->items(point).count() != 0)
            {
                QList<QGraphicsItem *> l = scene()->items(point);
                for(int i= l.size() - 1; i>=0; i--)
                {
                    Geometry * g = (Geometry *)l.at(i);
                    QStringList nameList = g->objectName().split('_');
                    if(g->objectName().isEmpty() || nameList.size() != 2)
                    {
                        l.removeOne(l.at(i));
                        continue;
                    }
                    Layer * layer = manager->getLayerByID(nameList.at(0));
                    if(!layer->isSelectable())
                        l.removeOne(l.at(i));

                }
                emit sendItemList(l);
            }
        }
        return true;
    }
    case QEvent::MouseMove:
    {
        QMouseEvent * moveEvent = static_cast<QMouseEvent *>(event);
        emit sendLocationPos(sceneToWorld(mapToScene(moveEvent->pos())));
        if(moveEvent->buttons() & Qt::LeftButton)
        {

            QPoint moveDelta = moveEvent->pos() - zoomOnPos;
            scene()->clear();
            setSceneLocation(QPointF(sceneRect().x() - moveDelta.x(),sceneRect().y() - moveDelta.y()));
            backgroundPos = backgroundPos + moveDelta;
            zoomOnPos = moveEvent->pos();
            mouseMove = true;
            moveEvent->accept();
        }
        return true;
    }
    case QEvent::MouseButtonRelease:
    {
        QMouseEvent * releaseEvent = static_cast<QMouseEvent *>(event);
        {
            if(releaseEvent->button() & Qt::LeftButton && zoomOnPos != QPoint(0,0) && mouseMove)
            {
                emit viewChangedSignal();
                zoomOnPos = QPoint(0,0);
                releaseEvent->accept();
            }
        }
        mouseMove = false;
        return true;
    }
    case QEvent::GraphicsSceneMousePress:
    {
        qDebug() << event->type();
        return true;
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
    p->setRenderHint(QPainter::Antialiasing);
    p->drawPixmap(backgroundPos,background);
    p->restore();
}

void ILong::drawForeground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);
    painter->save();
    painter->resetTransform();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QColor(Qt::green));
    QLabel lb;
    QPoint p = viewport()->rect().center();
    int line = distanceList.at( zoomLevel()-1) / pow(2.0, MAXZOOMLEVEL-zoomLevel() -1) / 0.597164;
    int telta = ((viewport()->width() > viewport()->height() ? viewport()->width() : viewport()->height()) / 2) / line;
    for(int i=0; i<=telta; i++)
    {
        painter->drawLine(QPoint(p.x()+i*line,p.y()-5),QPoint(p.x()+i*line,p.y()+5));
        painter->drawLine(QPoint(p.x()-i*line,p.y()-5),QPoint(p.x()-i*line,p.y()+5));
        painter->drawLine(QPoint(p.x()-5,p.y()+i*line),QPoint(p.x()+5,p.y()+i*line));
        painter->drawLine(QPoint(p.x()-5,p.y()-i*line),QPoint(p.x()+5,p.y()-i*line));
    }
    //painter->setPen(QColor(Qt::green));
    painter->setBrush(QColor(Qt::green));
    QFont font = painter->font();
    font.setBold(true);
    painter->setFont(font);
    QString distance = QVariant( distanceList.at(zoomLevel()-1)/1000 ).toString();
    painter->drawText(QPoint(10,height()-15), QString("(%1) %2km").arg(currentLevel).arg(distance));
    QString copyRight("iLong.io");
    painter->drawText(QPoint((width()-lb.fontMetrics().width(copyRight)-15),height()-15),copyRight);
    QString north = currentPos.x() >= 0 ? "N" : "S";
    QString east = currentPos.y() >= 0 ? "E" : "W";
    painter->resetTransform();
    painter->translate(width(),0);
    painter->rotate(90);
    painter->drawText(QPoint(15,10+lb.fontMetrics().height()),QString("%1%2 %3%4").arg(north)
                      .arg(fabs(currentPos.x()),0,'g',10).arg(east)
                      .arg(fabs(currentPos.y()),0,'g',10));
    painter->resetTransform();
    painter->translate(15+lb.fontMetrics().height()/2,15);
    painter->rotate(90);
    //painter->translate(0,width()-15-lb.fontMetrics().height());
    painter->drawText(QPoint(0,10),QString("A:%1 D:%2 T:%3 S:%4")
                      .arg(GPSAltitude).arg(GPSDir).arg(tilesCount)
                      .arg(satellitesCount) );
    painter->restore();
}

void ILong::resizeEvent(QResizeEvent *event)
{
    event->accept();
    zoomTo(defaultLocation,currentLevel);
}

void ILong::keyPressEvent(QKeyEvent *event)
{
    //QMessageBox::information(this,"x",QString::number( event->key()));
    switch (event->key()) {
    case Qt::Key_J:
    case Qt::Key_VolumeUp:
        zoomIn();
        break;
    case Qt::Key_K:
    case Qt::Key_VolumeDown:
        zoomOut();
        break;
    case Qt::Key_W:
        setViewOffset(0,-10);
        break;
    case Qt::Key_S:
        setViewOffset(0,10);
        break;
    case Qt::Key_A:
        setViewOffset(-10,0);
        break;
    case Qt::Key_D:
        setViewOffset(10,0);
        break;
    default:
        break;
    }
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
    background.fill(QColor::fromRgb(236,236,236));
    backgroundPos = mapFromScene((-leftTop.x()+middle.x())*DEFAULTTILESIZE,
                                 (-leftTop.y()+middle.y())*DEFAULTTILESIZE);
    /*
     * 先把所有有效的瓦片坐标保存到tList里
     * */
    QList<QPoint> tList;
    for(int x=-leftTop.x()+middle.x(); x<=rightTiles+middle.x(); x++)
    {
        for(int y=-leftTop.y()+middle.y(); y<=bottomTiles+middle.y(); y++)
        {
            if(map.isTileValid(x,y,currentLevel))
            {
                tList.append(QPoint(x,y));
            }

        }
    }
    /*
     * 再从数据库里读取当前场景有效的瓦片,如果读取失败,直接把@tList里的所有坐标生成path保存到@list里,
     * 然后跳到最后启动下载线程
     * */
    QSqlQuery * query = sqlExcute.checkImage(leftTop.x()+middle.x(), -rightTiles+middle.x(),
                                             leftTop.y()+middle.y(), -bottomTiles+middle.y(), currentLevel);
    bool checkImageError = false;
    if(query == nullptr)
    {
        while(!tList.isEmpty())
        {
            QPoint p = tList.first();
            QString path = map.queryTile(p.x(),p.y(),currentLevel);
            list.contains(path) ? list.move(list.indexOf(path),0) : list.insert(0,path);
            tList.removeFirst();
        }
        checkImageError = true;
    }
    /*
     * 如果读取成功,得把瓦片打印到场景的背景图里,并删除@tList里的对就瓦片的坐标
     * */
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
        tList.removeOne(QPoint(x,y));
    }
    if(query)
    {
        delete query;
        query = 0;
    }
    /*
     * 到了这里,如果tList还没空,那就说明有此瓦片是在数据库里没有的,需要从@tList里把坐标转成path保存到@list里然后启动下载线程
     * 之所以用随机数,就是为了影响瓦片的下载顺序,让人看着感觉不是只能从一个方向刷新场景背景的了,没多大用处
     * */
    qsrand(QDateTime::currentDateTime().time().second());
    while(!tList.isEmpty() && !checkImageError)
    {
        int index = qrand() % tList.size();
        QPoint value = tList.at(index);
        QString path = map.queryTile(value.x(), value.y(), currentLevel);
        list.contains(path) ? list.move(list.indexOf(path),0) : list.insert(0,path);
        tList.removeOne(value);
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

void ILong::addGeoToScene(Geometry *g)
{
    if(g)
        scene()->addItem(g);
}

void ILong::viewChangedSlot()
{
    //scene()->clear();
    manager->stopUpdateLayer();
    tilesUrlMatrix();
    if(!updateThread.isRunning())
        updateThread.start();
    emit updateLayer();
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

void ILong::updateInfo(QPointF GPSPos, qreal speed, qreal dir, qreal altitude)
{
    Q_UNUSED(speed);
    GPSDir = dir;
    currentPos = GPSPos;
    GPSAltitude = altitude;
    manager->addTempItem(GPSPos);
}

void ILong::updateSatellitesCount(int count)
{
    satellitesCount = count;
}

