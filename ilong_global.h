#ifndef ILONG_GLOBAL_H
#define ILONG_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QDir>


#if defined(ILONG_LIBRARY)
#  define ILONGSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ILONGSHARED_EXPORT Q_DECL_IMPORT
#endif

#define PI 3.14159265358979323846264338327950288419717

#define DEFAULTTILESIZE 256
#define DEFAULTZOOMLEVEL 9
#define MAXZOOMLEVEL 18
#define MINZOOMLEVEL 1
#define DEFAULTLOCATION QPointF(0,0)
#define CONFIGPATH QDir::homePath() + "/.ilong.io/"


typedef struct
{
    int x;
    int y;
    int z;
} TPoint;
/*
 * 图元类型就只会为两种,
 * 1,点类图元,可以有很多种点类图元,都是以中心点画图元
 * 2,面类图元,由多个坐标生成的图元,线条也是面类图元的一种
 * */
typedef enum
{
    iGeoNull,    //初始化类型
    //点类图元
    iGeoCircle,  //点 圆
    iGeoRect,    //矩形
    iGeoStar,    //五角星
    iGeoTri,     //三角形
    iGeoMouse,   //老鼠
    iGeoPie,     //扁形
    //面类图元
    iGeoPolygon
} ILongGeoType;
/*
 * 面类图元线类型,是有箭头和没箭头
 * */
typedef enum
{
    LineNull,   //没箭头
    LineStart,  //开始有箭头
    LineEnd,    //结束有箭头
    LineBoth,   //双箭头
} ILongLineType;
//保存图元的边界
typedef struct
{
    qreal minX;
    qreal minY;
    qreal maxX;
    qreal maxY;
} ILongGeoRect;
//数据库字段类型,只设计文本和数值
typedef enum
{
    ILongNUMBER,
    ILongTEXT,
} ILongType;

//图层数据结构,就只有字段名和这字段类型了
typedef struct
{
    QString name;
    ILongType type;
} LayerFormat;

#endif // ILONG_GLOBAL_H
