#include "ILong.h"


ILong::ILong(QWidget *parent) : QGraphicsView(parent),itemScale(1),
    currentLevel(DEFAULTZOOMLEVEL),numberOfTiles(tilesOnZoomLevel(currentLevel)),
    defaultLocation(DEFAULTLOCATION),net(new Network(this)),
    tilesCount(0),currentPos(DEFAULTLOCATION),itemLimit(1000),defaultZoomCount(0),
    twoFinger(false)
{
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
//    QList<LayerFormat> xlist;
//    LayerFormat f1;
//    f1.name = "info";
//    f1.type = ILongNUMBER;
//    LayerFormat f2;
//    f2.name = "name";
//    f2.type = ILongTEXT;
//    xlist.append(f1);
//    xlist.append(f2);
//    Layer * l = manager->addLayer("xxx", &xlist);
//    QList<QPointF> p;
//    p.append(QPointF(99.80875,27.72188));
//    Geometry::ILongDataType t;
//    t.data.append(12);
//    t.data.append("ilongio");
//    t.geometry = new GeoPie(QPointF(99.80875,27.72188),80,90);
//    QList<Geometry::ILongDataType> gl;
//    gl.append(t);
//    l->addItem(&gl);

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
        zoomTo(worldCoordinate,zoomLevel);
    }
    else
    {
        zoomTo(defaultLocation,currentLevel);
    }
}

void ILong::zoomOnCenter(int level)
{
    if(!checkZoomLevel(level)) return;
    level > currentLevel ? zoomIn() : zoomOut();
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
//    case QEvent::TouchBegin:
//    case QEvent::TouchUpdate:
//    case QEvent::TouchEnd:
//    {
//        QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
//        QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
//        if (touchPoints.count() == 2) {
//            touchEvent->accept();
//            twoFinger = true;
//            // determine scale factor
//            const QTouchEvent::TouchPoint touchPoint0 = touchPoints.first();
//            const QTouchEvent::TouchPoint touchPoint1 = touchPoints.last();
//            qreal currentScale =
//                    QLineF(touchPoint0.pos(), touchPoint1.pos()).length()
//                    / QLineF(touchPoint0.startPos(), touchPoint1.startPos()).length();
//            zoomOnPos = viewport()->rect().center();
//            currentScale > 1 ? zoomOut() : zoomIn();
////            if (touchEvent->touchPointStates() & Qt::TouchPointReleased) {
////                // if one of the fingers is released, remember the current scale
////                // factor so that adding another finger later will continue zooming
////                // by adding new scale factor to the existing remembered value.
////                defaultZoomCount = 0;
////            }
////            if(currentScale < 1)
////            {
////                defaultZoomCount - currentScale < -12 ? defaultZoomCount = -12 : defaultZoomCount-=currentScale;
////            }
////            else
////            {
////                defaultZoomCount + currentScale > 12 ? defaultZoomCount = 12 : defaultZoomCount+=currentScale;
////            }
////            if(defaultZoomCount == -12)
////            {
////                zoomOut();
////                defaultZoomCount = 0;
////                twoFinger = false;
////                return true;
////            }
////            if(defaultZoomCount == 12)
////            {
////                zoomIn();
////                defaultZoomCount = 0;
////            }
//            twoFinger = false;
//            return true;
//        }
//        break;
//    }
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
            pressEvent->accept();
        }
        return true;
    }
    case QEvent::MouseMove:
    {
        if(!twoFinger)
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
            }
        }
        return true;
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
            }
        }
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
    QPoint p = viewport()->rect().center();
    painter->drawLine(p-QPoint(10,0), p+QPoint(10,0));
    painter->drawLine(p-QPoint(0,10), p+QPoint(0,10));
    QFont font = painter->font();
    font.setBold(true);
    painter->setFont(font);
    painter->drawText(QPoint(0,20),QString("Lng:%1 Lat:%2 Til:%3 Lev:%4").arg(currentPos.x(),0,'g',10)
                      .arg(currentPos.y(),0,'g',10).arg(tilesCount).arg(currentLevel));

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

void ILong::mouseDoubleClickEvent(QMouseEvent *)
{
//    zoomOnPos = viewport()->rect().center();
//    e->pos().y() < viewport()->rect().center().y() ? zoomIn() : zoomOut();
    //e->accept();
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
//    QGraphicsRectItem * r = new QGraphicsRectItem(0,0,10,20);
//    r->setPen(QColor(Qt::red));
//    r->setPos(worldToScene(QPointF(99.70875,27.82188)));
//    r->setScale(itemScale);
//    scene()->addItem(r);
//    QGraphicsTextItem * tpx = new QGraphicsTextItem("YYYYY");
//    tpx->setDefaultTextColor(QColor(Qt::yellow));
//    tpx->setPos(worldToScene(QPointF(99.70875,27.82188)));
//    tpx->setScale(itemScale);
//    scene()->addItem(tpx);
//    QGraphicsTextItem * tx = new QGraphicsTextItem("mmmm");
//    tx->setPos(worldToScene(QPointF(99.80875,27.72188)));
//    tx->setScale(itemScale);
//    tx->setDefaultTextColor(QColor(Qt::green));
//    scene()->addItem(tx);
//    QGraphicsTextItem * px = new QGraphicsTextItem("center");
//    px->setDefaultTextColor(QColor(Qt::red));
//    px->setPos(worldToScene(defaultLocation));
//    px->setScale(itemScale);
//    scene()->addItem(px);
//    GeoPie * gm = new GeoPie(QPointF(99.70875,27.82188),120,QColor(Qt::green),QColor(Qt::red));
//    gm->setPos(worldToScene(QPointF(99.70875,27.82188)));
//    gm->setScale(itemScale);
//    gm->setLabel("第一小区");
//    gm->setZValue(1);
//    gm->rotate(0);
//    gm->setZValue(0);
//    gm->setFlag(QGraphicsItem::ItemIsSelectable);

//    scene()->addItem(gm);
//    GeoPie * gm1 = new GeoPie(QPointF(99.70875,27.82188),120,QColor(Qt::red),QColor(Qt::blue));
//    gm1->setPos(worldToScene(QPointF(99.70875,27.82188)));
//    gm1->setScale(itemScale);
//    gm1->setLabel("在算是第2小区");
//    gm1->rotate(120);

//    scene()->addItem(gm1);

//    GeoPie * gm2 = new GeoPie(QPointF(99.70875,27.82188),120,QColor(Qt::blue),QColor(Qt::green));
//    gm2->setPos(worldToScene(QPointF(99.70875,27.82188)));
//    gm2->setLabel("the last 小区");
//    gm2->setScale(itemScale);
//    gm2->rotate(240);

//    scene()->addItem(gm2);

//    GeoPie * gm4 = new GeoPie(QPointF(99.70875,27.82188),80,QColor(Qt::green),QColor(Qt::yellow));
//    gm4->setPos(worldToScene(QPointF(99.70875,27.82188)));
//    gm4->setScale(itemScale);
//    gm4->setLabel("xxxxx");
//    gm4->rotate(0);
//    gm4->setFlag(QGraphicsItem::ItemIsSelectable);

//    scene()->addItem(gm4);
//    GeoPie * gm5 = new GeoPie(QPointF(99.70875,27.82188),80,QColor(Qt::red),QColor(Qt::gray));
//    gm5->setPos(worldToScene(QPointF(99.70875,27.82188)));
//    gm5->setScale(itemScale);
//    gm5->setLabel("在算是第dfdfd2小区");
//    gm5->rotate(120);

//    scene()->addItem(gm5);

//    GeoPie * gm6 = new GeoPie(QPointF(99.70875,27.82188),80,QColor(Qt::blue),QColor(Qt::white));
//    gm6->setPos(worldToScene(QPointF(99.70875,27.82188)));
//    gm6->setLabel("the sssslast 小区");
//    gm6->setScale(itemScale);
//    gm6->rotate(240);

//    scene()->addItem(gm6);
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

