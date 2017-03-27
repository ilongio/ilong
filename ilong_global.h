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

typedef enum
{
    ILongNUMBER,
    ILongTEXT,
} ILongType;

typedef struct
{
    QString name;
    ILongType type;
} LayerFormat;
#endif // ILONG_GLOBAL_H
