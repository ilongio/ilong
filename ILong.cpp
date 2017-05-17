#include "ILong.h"


ILong::ILong(QWidget *parent) : QGraphicsView(parent),itemScale(1),
    currentLevel(DEFAULTZOOMLEVEL),numberOfTiles(tilesOnZoomLevel(currentLevel)),
    defaultLocation(DEFAULTLOCATION),net(new Network(this)),
    tilesCount(0),currentPos(DEFAULTLOCATION),itemLimit(200),
    satellitesCount(0),GPSAltitude(0),GPSDir(0)
{
    distanceList<<5000000<<2000000<<1000000<<1000000<<1000000<<100000<<100000<<50000<<50000<<10000<<10000<<10000<<1000<<1000<<500<<200<<100<<50<<25;
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
    /*
     * 处理当前世界坐标位置信号
     * */
    connect(this,SIGNAL(sendLocationPos(QPointF)),this,SLOT(updateLocationPos(QPointF)));
//    resetMatrix();
//    scale(1,1);
    QList <LayerFormat> fm;
    fm.append(LayerFormat{"X",ILongNUMBER});
    fm.append(LayerFormat{"Y",ILongNUMBER});
    fm.append(LayerFormat{"VALUE",ILongNUMBER});
    fm.append(LayerFormat{"NAME",ILongTEXT});
    tempLayer = manager->addLayer("TempILong", &fm);

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
    zoomTo(defaultLocation,currentLevel);
}

QList<Layer *> ILong::getLayers() const
{
    return manager->getLayers();
}

Layer *ILong::getlayer(QString name) const
{
    return manager->getLayer(name);
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
}

quint32 ILong::getItemLimit()
{
    return itemLimit;
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
        if(pressEvent->buttons() & Qt::LeftButton)
        {
            zoomOnPos = pressEvent->pos();
            mouseMove = false;
            QPointF point = mapToScene(pressEvent->pos());
            if (scene()->items(point).count() != 0)
            {
                SelectInfo info(this);
                Geometry * item;
                QList<QGraphicsItem *> l = scene()->items(point);
                for(int i=0; i< l.count(); i++)
                {
                    item = (Geometry *)l.at(i);
                    if(item->objectName().isEmpty())
                        continue;
                    QStringList nameList = item->objectName().split('_');
                    info.getModel()->setItem(i,0,new QStandardItem(manager->getLayerByID(nameList.at(0))->getLayerName()));
                    info.getModel()->setItem(i,1,new QStandardItem(nameList.at(1)));
                    info.getModel()->setItem(i,2,new QStandardItem(item->getLabel()));
                }
                info.exec();
            }
            pressEvent->accept();
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
    p->drawPixmap(backgroundPos,background);
    p->restore();
}

void ILong::drawForeground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);
    painter->save();
    painter->resetTransform();
    painter->setPen(QColor(Qt::green));
    painter->setBrush(QColor(Qt::green));
    QPoint p = viewport()->rect().center();
    painter->drawLine(p-QPoint(10,0), p+QPoint(10,0));
    painter->drawLine(p-QPoint(0,10), p+QPoint(0,10));
    QFont font = painter->font();
    font.setBold(true);
    painter->setFont(font);
    double line = distanceList.at( zoomLevel() ) / pow(2.0, MAXZOOMLEVEL-zoomLevel() ) / 0.597164;
    QPoint p1(10,10);
    QPoint p2((int)line,10);
    QPoint p3(10,(int)line);
    painter->drawLine(p1,p2);
    painter->drawLine(p2,QPoint((int)line,15));
    painter->drawLine(p1,p3);
    painter->drawLine(QPoint(15,(int)line),p3);
//    painter->drawLine(10,height()-15, 10,height()-25);
//    painter->drawLine((int)line,height()-15, (int)line,height()-25);
    QString distance = QVariant( distanceList.at(zoomLevel())/1000 ).toString();
    painter->drawText(QPoint(10,height()-15), QString("(%1) %2km").arg(currentLevel).arg(distance));
    QLabel lb;
    QString copyRight("iLong.io");
    painter->drawText(QPoint((width()-lb.fontMetrics().width(copyRight))/2,height()-15),copyRight);
    painter->translate(width()-12,10);
    painter->rotate(90);
    QString north = currentPos.x() >= 0 ? "N" : "S";
    QString east = currentPos.y() >= 0 ? "E" : "W";
    painter->drawText(QPoint(0,10),QString("%1%2 %3%4 A:%5 D:%6 T:%7 S:%8").arg(north)
                      .arg(fabs(currentPos.x()),0,'g',10).arg(east)
                      .arg(fabs(currentPos.y()),0,'g',10).arg(GPSAltitude).arg(GPSDir).arg(tilesCount)
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
    case Qt::Key_VolumeUp:
        zoomIn();
        break;
    case Qt::Key_VolumeDown:
        zoomOut();
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

void ILong::viewChangedSlot()
{
    scene()->clear();
    tilesUrlMatrix();
    manager->updatLayer();
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
    tempLayer->addTempItem(iGeoMouse,GPSPos, dir);
    currentPos = GPSPos;
    GPSAltitude = altitude;
}

void ILong::updateSatellitesCount(int count)
{
    satellitesCount = count;
}

