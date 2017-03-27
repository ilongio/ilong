#ifndef SQLEXCUTE_H
#define SQLEXCUTE_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>
#include <QDebug>

#include "ilong_global.h"

class SQLExcute : public QObject
{
    Q_OBJECT
public:
    explicit SQLExcute(QObject *parent = 0);
    QSqlQuery * checkImage(int maxX, int minX, int maxY, int minY, int z);
    void insertImage(int x, int y, int z, QByteArray ax);
    QSqlQuery * initLayerManager();
    QSqlQuery *checkType(QString id);
    void initLayer(QString id, QString name, QList<LayerFormat> *typeList, QList<ILongType> *headType);
    void removeLayer(QString id);
    void setLayerVisible(QString id,bool b);
    void setLayerSelectable(QString id,bool b);
private:
    QSqlQuery *getResult(QString sql, QString position);
    void nonResult(QString sql, QString position);
signals:

public slots:
};

#endif // SQLEXCUTE_H
