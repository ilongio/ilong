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
#include "Manager.h"
#include "Layer.h"

/*
 * 提供简单的跨平台的瓦片图层框架功能,反正不会C++,更不会面向对象,用来练手的!
 * */

class Network;
class Manager;
class ILONGSHARED_EXPORT ILong : public QGraphicsView
{
    Q_OBJECT
public:
    friend class Manager;
    friend class Layer;
    friend class Network;
    ILong(QWidget *parent);
    ~ILong();

    /*****************************************************************************
     *                                  API
     * ***************************************************************************/

    /*
     * 最大地图等级
     * */
    quint8 maxZoomLevel();
    /*
     * 最小地图等级
     * */
    quint8 minZoomLevel();
    /*
     * 当前地图等级
     * */
    quint8 zoomLevel();
    /*
     * 放大地图
     * */
    void zoomIn();
    /*
     * 缩小地图
     * */
    void zoomOut();
    /*
     * 设置默认的地图加载
     * @worldCoordinate 位置
     * @zoomLevel       地图等级
     * 和zoomTo差不多,但是zoomTo参数多了点,供内部使用的,这函数也调用zoomTo函数,只想提供一个简单API给外部调用
     * */
    void setDefaultLocation(QPointF worldCoordinate, quint8 zoomLevel);
    /*
     * 返回所有图层
     * */
    QList<Layer *> getLayers() const;
    /*
     * 新增图层
     * @name     图层名称
     * @typeList 图层数据结构
     * */
    Layer *addLayer(QString name, QList<LayerFormat> *typeList) const;
    /*
     * 通过图层名称@name删除图层
     * */
    void removeLayer(QString name);

protected:
    bool viewportEvent(QEvent *event);
    void drawBackground(QPainter *p, const QRectF &rect);
    void drawForeground(QPainter *painter, const QRectF &rect);
    void resizeEvent(QResizeEvent *event);

private:
    /*
     * 世界坐标和场景坐标相与转换
     * */
    QPointF worldToScene(QPointF world);
    QPointF sceneToWorld(QPointF scene);
    /*
     * 角度和弧度相与转换,没上过高中,数学学得不好,真心不理解弧度,但不影响
     * */
    qreal degreeToRadian(qreal value);
    qreal radianToDegree(qreal value);
    /*
     * 计算当前地图等级@zoomLevel单行或单列共有多少张瓦片了
     * */
    int tilesOnZoomLevel(quint8 zoomLevel);
    /*
     * 计算当前场景内的所有瓦片,并在数据库里查找,如果没找到就下载,如果找到了直接打印到场景背景图片里
     * */
    void tilesUrlMatrix();
    /*
     * 缩放在指定的位置@world和指定的地图等级@zoomLevel
     * 因为默认是缩放在场景中心,所以加了个参数@underMouse限制缩放在鼠标下面
     * */
    void zoomTo(QPointF world, quint8 zoomLevel, bool underMouse = false);
    /*
     * 像拿着放大镜看地图一样在地图上移动场景,就当把场景当成是不能放大的放大镜就好
     * @topLeftPos 是场景的左上角位置
     * @updateItem 因为平移的时候不想刷新图元,刷新的话,压力比较大,所以设置这我参数
     * */
    void setSceneLocation(QPointF topLeftPos, bool updateItem = false);
    /*
     * 检查地图等级@zoomLevel是否有效
     * */
    bool checkZoomLevel(quint8 zoomLevel);
    /*
     * 检查世界坐标@world是否有效
     * */
    bool checkWorldCoordinate(QPointF world);
    /*
     * item缩放系数
     * */
    qreal itemScale;
    /*
     * 当前地图等级
     * */
    quint8 currentLevel;
    /*
     * 当前地图瓦片数量
     * */
    quint32 numberOfTiles;
    /*
     * 滚动鼠标缩放或鼠标平移地图时候 保存鼠标位置
     * */
    QPoint zoomOnPos;
    /*
     * 保存初始化位置的世界坐标
     * */
    QPointF defaultLocation;
    /*
     * 地图供应商
     * */
    Map map;
    /*
     * 场景背景图片
     * */
    QPixmap background;
    /*
     * 场景背景图片打印位置
     * */
    QPoint backgroundPos;
    /*
     * @middle  场景中心的那张瓦片的坐标
     * @leftTop 场景左上角的那张瓦片的坐标
     * 保存在这里是因为下载完瓦片后需要知道打印在哪
     * */
    QPoint middle, leftTop;
    /*
     * 管理网络服务
     * */
    Network * net;
    /*
     * 管理图层
     * */
    Manager * manager;
    /*
     * 瓦片path列表,下载的时候直接从列表里读取,刷新的时候直接保存在列表里
     * */
    QList <QString> list;
    /*
     * 下载瓦片线程
     * */
    QThread  networkThread;
    /*
     * 数据库管理
     * */
    SQLExcute sqlExcute;
    /*
     * 下载瓦片时候都触发还剩下多少张瓦片没下,就保存在这里,刷新的时候直接打印到场景前景
     * */
    int tilesCount;
    /*
     * 保存当前鼠标所在的位置对应的世界WGS坐标,刷新的时候直接打印到场景前景
     * */
    QPointF currentPos;

signals:
    void viewChangedSignal();
    void downloadImage();
    void sendLocationPos(QPointF);
public slots:
    void viewChangedSlot();
    void newImage();
    void updateTilesCount(int count);
    void updateLocationPos(QPointF world);
};

#endif // ILONG_H
