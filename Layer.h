#ifndef LAYER_H
#define LAYER_H

#include <QObject>
#include <QGraphicsItemGroup>

#include "ILong.h"
#include "SQLExcute.h"

/*
 * 提供简单的图元管理功能
 * */

class ILong;
class Layer : public QObject
{
    Q_OBJECT
public:
    /*
     * 新增图层
     * @name 图层名称
     * @typeList 图层数据结构
     * */
    explicit Layer(ILong * parent,QString name, QList<LayerFormat> * typeList);
    /*
     * 从数据库里加载图层,所有参数都在数据库管理表里得到
     * */
    Layer(ILong * parent, QString id, QString name, bool visible, bool selectable);
    ~Layer();
    void addItem(QGraphicsItem *item);
    void removeItem(QGraphicsItem *item);
    /*
     * 返回所有图元的地址
     * */
    QList<QGraphicsItem *> * getItems();
    /*
     * 返回图层名称
     * */
    QString getLayerName();
    /*
     * 返回图层ID,和数据库里的表关联
     * */
    QString getLayerID();
    /*
     * 设置和返回图层可视状态
     * */
    void setVisible(bool b);
    bool isVisible();
    /*
     * 设置和返回图层可选状态
     * */
    void setSelectable(bool b);
    bool isSelectable();
private:
    ILong * iLong;
    /*
     * 保存场景里的组指针
     * 一个场景里可以建很多的图元组嘛, 一个组就管理自己的图元 就当时图层了
     * */
    QGraphicsItemGroup * layer;
    QString layerLabel;
    /*
     * 保存当前图层的图元指针
     * */
    QList<QGraphicsItem *> list;
    QString layerID;
    bool visible;
    bool selectable;
    SQLExcute * sqlExcute;
    /*
     * 保存当前图层的字段类型,只为了方便导入数据时数据转换检查
     * */
    QList <ILongType> headType;
signals:

public slots:
};

#endif // LAYER_H
