#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QList>

#include "ILong.h"
#include "Layer.h"
#include "SQLExcute.h"

class ILong;
class Layer;
class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(ILong *parent);
    QList<Layer *> getLayers();
    Layer *addLayer(QString name, QList<LayerFormat> *typeList);
    void removeLayer(QString name);
private:
    QString checkLayerName(QString name);
    void loadLayer(QString id, QString name, bool visible, bool selectable);
    ILong * iLong;
    QList<Layer *> list;
    SQLExcute * sqlExcute;
signals:

public slots:
};

#endif // MANAGER_H
