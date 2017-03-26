#include "ILong.h"


ILong::ILong(QWidget *parent) : QGraphicsView(parent),itemScale(1),
    currentLevel(DEFAULTZOOMLEVEL),numberOfTiles(tilesOnZoomLevel(currentLevel)),
    defaultLocation(DEFAULTLOCATION),net(new Network(this))
{
    setScene(new QGraphicsScene(this));
    //setMouseTracking(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOff );
    //setTransformationAnchor(AnchorUnderMouse);
    //setDragMode(ScrollHandDrag);
    setViewportUpdateMode(FullViewportUpdate);
    centerOn(0,0);
    setSceneRect(viewport()->rect());
    connect(this,SIGNAL(viewChangedSignal()),this,SLOT(viewChangedSlot()));
    net->moveToThread(&networkThread);
    connect(this,SIGNAL(downloadImage()),net,SLOT(start()));
    connect(net,SIGNAL(newImage()),this,SLOT(newImage()));
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

QPoint ILong::getMiddlePos()
{
    return middle;
}

QPoint ILong::getTopLeftPos()
{
    return leftTop;
}

QPixmap *ILong::getBackground()
{
    return &background;
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
    background.fill(QColor(Qt::black));
    backgroundPos = mapFromScene((-leftTop.x()+middle.x())*DEFAULTTILESIZE,
                                 (-leftTop.y()+middle.y())*DEFAULTTILESIZE);
    QList<QString> newList;
    QString sql = "SELECT * FROM ILONGIO WHERE ";
    for(int y=leftTop.y()+middle.y(); y>=-bottomTiles+middle.y(); y--)
    {
        for(int x=leftTop.x()+middle.x(); x>=-rightTiles+middle.x(); x--)
        {
            if(map.isTileValid(x,y,currentLevel))
            {
                QString serverPath = map.queryTile(x,y,currentLevel);
                newList.append(serverPath);
                QString tmp = QString(" ( X = %1 AND Y = %2 AND Z = %3 ) OR ").arg(x).arg(y).arg(currentLevel);
                sql.append(tmp);
            }

        }
    }
    sql = sql.left(sql.length() - 3);
    QSqlQuery * query = sqlExcute.checkImage(sql);
    bool checkImageError = false;
    if(query == nullptr)
    {
        while(!newList.isEmpty())
        {
            QString path = newList.first();
            list.contains(path) ? list.move(list.indexOf(path),0) : list.insert(0,path);
            newList.removeFirst();
        }
        checkImageError = true;
    }
    while (query->next() && !checkImageError)
    {
        int x = query->value(0).toInt();
        int y = query->value(1).toInt();
        int z = query->value(2).toInt();
        QString path = map.queryTile(x, y, z);
        QPixmap pm;
        pm.loadFromData(query->value(3).toByteArray());
        QPainter painter(&background);
        painter.drawPixmap((x+leftTop.x()-middle.x())*DEFAULTTILESIZE
                           ,(y+leftTop.y()-middle.y())*DEFAULTTILESIZE
                           ,DEFAULTTILESIZE,DEFAULTTILESIZE,pm);
        painter.end();
        newList.removeOne(path);
    }
    if(query)
    {
        delete query;
        query = 0;
    }
    while(!newList.isEmpty() && !checkImageError)
    {
        QString path = newList.first();
        list.contains(path) ? list.move(list.indexOf(path),0) : list.insert(0,path);
        newList.removeFirst();
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
    scene()->addItem(tx);
    QGraphicsTextItem * px = new QGraphicsTextItem("center");
    px->setDefaultTextColor(QColor(Qt::red));
    px->setPos(worldToScene(defaultLocation));
    px->setScale(itemScale);
    scene()->addItem(px);
}

void ILong::newImage()
{
    viewport()->update();
}

