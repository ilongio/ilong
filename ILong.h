#ifndef ILONG_H
#define ILONG_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QWheelEvent>
#include <QThread>
#include <QtMath>
#include <QList>
#include <QDebug>
#include "ilong_global.h"
#include "Map.h"
#include "Network.h"
#include "ILoveChina.h"
#include "SQLExcute.h"

class Network;
class ILONGSHARED_EXPORT ILong : public QGraphicsView
{
    Q_OBJECT
public:
    ILong(QWidget *parent);
    ~ILong();
    quint8 maxZoomLevel();
    quint8 minZoomLevel();
    quint8 zoomLevel();
    void zoomIn();
    void zoomOut();
    void setDefaultLocation(QPointF worldCoordinate, quint8 zoomLevel);
    QList <QString> * getImageList();
    QString getServer();
    QPoint getMiddlePos();
    QPoint getTopLeftPos();
    QPixmap * getBackground();
protected:
    bool viewportEvent(QEvent *event);
    void drawBackground(QPainter *p, const QRectF &rect);
    void resizeEvent(QResizeEvent *event);
private:
    QPointF worldToScene(QPointF world);
    QPointF sceneToWorld(QPointF scene);

    qreal degreeToRadian(qreal value);
    qreal radianToDegree(qreal value);

    int tilesOnZoomLevel(quint8 zoomLevel);
    void tilesUrlMatrix();

    void zoomTo(QPointF world, quint8 zoomLevel, bool underMouse = false);
    void setSceneLocation(QPointF topLeftPos, bool updateItem = false);

    bool checkZoomLevel(quint8 zoomLevel);
    bool checkWorldCoordinate(QPointF world);

    //item缩放系数
    qreal itemScale;
    //当前地图等级
    quint8 currentLevel;
    //当前地图瓦片数量
    quint32 numberOfTiles;
    //滚动鼠标缩放或鼠标平移地图时候 保存鼠标位置
    QPoint zoomOnPos;
    //保存初始化位置的世界坐标
    QPointF defaultLocation;
    Map map;
    QPixmap background;
    QPoint backgroundPos;
    QPoint middle, leftTop;
    Network * net;
    QList <QString> list;//path,server
    QThread  networkThread;
    SQLExcute sqlExcute;
signals:
    void viewChangedSignal();
    void downloadImage();
public slots:
    void viewChangedSlot();
    void newImage();
};

#endif // ILONG_H
