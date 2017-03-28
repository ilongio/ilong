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

//几种图元类型
typedef enum
{
    GeoNull,//初始化类型
    GeoPoint,//点
    GeoCircle,//圆
    GeoLine,//线
    GeoRect,//矩形
    GeoStar,//五角星
    GeoTri,//三角形
    GeoMouse,//老鼠
    GeoPie,//扁形
} ILongGeoType;

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
