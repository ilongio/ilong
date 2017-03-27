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
    QString sql = QString("SELECT * FROM ILONGIO WHERE X >= %1 and X <= %2 and Y <= %3 and Y >= %4 and Z = %5")
            .arg(minX).arg(maxX).arg(maxY).arg(minY).arg(z);
    return getResult(sql, "checkImage");
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

QSqlQuery *SQLExcute::initLayerManager()
{
    QString sql = "CREATE TABLE IF NOT EXISTS ILONGIOLAYER(ID TEXT, NAME TEXT, VISIBLE INTEGER, SELECTABLE INTEGER)";
    nonResult(sql, "initLayerManager");
    sql = "SELECT * FROM ILONGIOLAYER";
    return getResult(sql,"initLayerManager check data");
}

QSqlQuery *SQLExcute::checkType(QString id)
{
    QString sql = QString(" PRAGMA table_info('%1') ").arg(id);
    return getResult(sql,"checkType " + id);
}

void SQLExcute::initLayer(QString id, QString name, QList<LayerFormat> * typeList, QList <ILongType> * headType)
{
    QString sql = QString("INSERT INTO ILONGIOLAYER VALUES ( '%1', '%2', '%3', '%4' )").arg(id).arg(name).arg(1).arg(1);
    nonResult(sql, "initLayer1");
    QString saveSql = "";
    for(int i=0; i< typeList->size(); i++)
    {
        QString t = QString(" '%1' '%2' , ").arg(typeList->at(i).name).arg(typeList->at(i).type ? "TEXT" : "REAL");
        headType->append(typeList->at(i).type);
        saveSql += t;
    }
    saveSql = saveSql.left(saveSql.length() - 2);
    sql = QString("CREATE TABLE '%1' ( %2 )").arg(id).arg(saveSql);
    nonResult(sql, "initLayer2");
}

void SQLExcute::removeLayer(QString id)
{
    QString sql = QString("DELETE FROM ILONGIOLAYER WHERE ID = '%1' ").arg(id);
    nonResult(sql, "removeLayer on ILONGIOLAYER " + id);
    sql = QString("DROP TABLE '%1' ").arg(id);
    nonResult(sql, "DROP TABLE " + id);
}

void SQLExcute::setLayerVisible(QString id, bool b)
{
    int result = b ? 1 : 0;
    QString sql = QString("UPDATE ILONGIOLAYER SET VISIBLE = '%1' WHERE ID = '%2' ").arg(result).arg(id);
    nonResult(sql, "setLayerVisible " + id);
}

void SQLExcute::setLayerSelectable(QString id, bool b)
{
    int result = b ? 1 : 0;
    QString sql = QString("UPDATE ILONGIOLAYER SET SELECTABLE = '%1' WHERE ID = '%2' ").arg(result).arg(id);
    nonResult(sql, "setLayerVisible " + id);
}

QSqlQuery *SQLExcute::getResult(QString sql, QString position)
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
            qDebug() << position << db.lastError().text();
            return nullptr;
        }
    }
    QSqlQuery * query = new QSqlQuery(db);
    if(!query->exec(sql))
    {
        qDebug() << position << query->lastError().text();
        return nullptr;
    }
    return query;
}

void SQLExcute::nonResult(QString sql, QString position)
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
            qDebug() << position << db.lastError().text();
            return;
        }
    }
    QSqlQuery query(db);
    if(!query.exec(sql))
    {
        qDebug() << position << query.lastError().text();
    }
}



