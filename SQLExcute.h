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
    QSqlQuery * checkImage(QString sql);
    void insertImage(int x, int y, int z, QByteArray ax);
private:

signals:

public slots:
};

#endif // SQLEXCUTE_H
