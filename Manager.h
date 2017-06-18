#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QList>

#include "ILong.h"
#include "Layer.h"
#include "SQLExcute.h"

/*
 * 提供简单的图层管理功能
 * */

class ILong;
class Layer;
class ILONGSHARED_EXPORT Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(ILong *iL, QObject * parent = 0);
    /*
     * 返回所有图层的指针
     * */
    QList<Layer *> getLayers();
    /*
     * 新增图层,名称@name
     * @typeList图层数据结构
     * */
    Layer *addLayer(QString name, QList<LayerFormat> *typeList);
    /*
     * 通过图层名称@name直接删除图层
     * */
    Layer *getLayer(QString name);
    Layer *getLayerByID(QString id);
    void removeLayer(QString name);
    void stopUpdateLayer();
    void addTempItem(QPointF world, ILongGeoType type = iGeoCircle);
    bool moveLayer(QString name, bool up = true);
private:
    /*
     * 检查图层名称@name是否在图层管理表里,如果有,就自动在@name后面加*号,暂时这样处理导入多个同名图层
     * */
    QString checkLayerName(QString name);
    /*
     * 从管理表里加载数据库里的图层,所有参数都在管理表里
     * */
    void loadLayer(QString id, QString name, bool visible, bool selectable);
    ILong * iLong;
    /*
     * 所有图层列表,不用每次去图层管理表读取图层信息
     * */
    QList<Layer *> list;
    SQLExcute * sqlExcute;
    bool isUpdate;

    QPointF tempGeoWorldPos;
    ILongGeoType tempGeoType;
signals:
    void addGeoToScene(Geometry *);
public slots:
    void updatLayer();
};

#endif // MANAGER_H
