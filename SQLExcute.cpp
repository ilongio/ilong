#include "SQLExcute.h"

SQLExcute::SQLExcute(QObject *parent) : QObject(parent)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QDir dir;
    if(!dir.exists(CONFIGPATH))
    {
        dir.mkdir(CONFIGPATH);
    }
    db.setDatabaseName(CONFIGPATH + "ilong.io");
    if(!db.open())
    {
        qDebug() << "Init SQLITE Open " << db.lastError().text();
    }
    QString sql = "CREATE TABLE IF NOT EXISTS ILONGIO(X INTEGER, Y INTEGER, Z INTEGER, IMAGE LONGBLOB, primary key(X,Y,Z))";
    QSqlQuery query(db);
    if(!query.exec(sql))
    {
        qDebug() << "CREATE TABLE ILONGIO " << query.lastError().text();
    }
}

QSqlQuery *SQLExcute::checkImage(int maxX, int minX, int maxY, int minY, int z)
{
    QSqlDatabase db;
    if(QSqlDatabase::contains("qt_sql_default_connection"))
      db = QSqlDatabase::database("qt_sql_default_connection");
    else
      db = QSqlDatabase::addDatabase("QSQLITE");
    if(!db.isOpen())
    {
        if(!db.open())
        {
            qDebug() << "checkImage Open " << db.lastError().text();
            return nullptr;
        }
    }
    QSqlQuery * query = new QSqlQuery(db);
    QString sql = QString("SELECT * FROM ILONGIO WHERE X >= %1 and X <= %2 and Y <= %3 and Y >= %4 and Z = %5")
            .arg(minX).arg(maxX).arg(maxY).arg(minY).arg(z);
    if(!query->exec(sql))
    {
        qDebug() << "checkImage " << query->lastError().text();
        return nullptr;
    }
    return query;
}

void SQLExcute::insertImage(int x, int y, int z, QByteArray ax)
{
    QSqlDatabase db;
    if(QSqlDatabase::contains("qt_sql_default_connection"))
      db = QSqlDatabase::database("qt_sql_default_connection");
    else
      db = QSqlDatabase::addDatabase("QSQLITE");
    if(!db.isOpen())
    {
        if(!db.open())
        {
            qDebug() << "insertImage Open " << db.lastError().text();
            return;
        }
    }
    QSqlQuery query(db);
    query.prepare("REPLACE INTO ILONGIO VALUES (?,?,?,?)");
    query.addBindValue(x);
    query.addBindValue(y);
    query.addBindValue(z);
    query.addBindValue(ax);
    if(!query.exec())
    {
        qDebug() << "insertImage query.exec() " << query.lastError().text();
    }
}


