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
    /*
     * 在ILONGIOLAYER建立表索引
     * 一个图层分成两个表
     * 一个表保存数据(数据表)
     * 一个表保存图元信息(信息表)
    */
    QString sql = QString("INSERT INTO ILONGIOLAYER VALUES ( '%1', '%2', '%3', '%4' )").arg(id).arg(name).arg(1).arg(1);
    nonResult(sql, "initLayer1");
    /*
     * 读取表数据结构 并创建表 把结构保存在字段类型headType里,方便插入图元数据使用
     */
    QString saveSql = "";
    for(int i=0; i< typeList->size(); i++)
    {
        QString t = QString(" '%1' '%2' , ").arg(typeList->at(i).name).arg(typeList->at(i).type ? "TEXT" : "REAL");
        headType->append(typeList->at(i).type);
        saveSql += t;
    }
    saveSql = saveSql.left(saveSql.length() - 2);
    /*
     * 创建数据表,专用保存导入的数据
     */
    sql = QString("CREATE TABLE '%1' (ILONGID REAL, %2 )").arg(id).arg(saveSql);
    nonResult(sql, "initLayer create data table ");
    /*
     *创建信息表,专用保存图元的,应该可以直接保存图元,但是现在还不知道怎么弄,就先这样弄吧,以后再想办法改进(个人技术原因),主要信息有:
     * @ILONGID     与数据的ID关联;
     * @TYPE   ILongGeoType 枚举图元类型
     * @X1     图元wgs X1 坐标
     * @Y1     图元wgs Y1 坐标
     * @X2     图元wgs X2 坐标 (点类图元写X1相同)
     * @Y2     图元wgs Y2 坐标 (点类图元写X2相同) 设计两个坐标点只为了非点类图元需要计算边界问题,比如线
     * @WIDTH  图元宽度
     * @HEIGHT 图元高度 基本上,所有设计的图元都有宽和高,除了老鼠类图元的宽和高,现在还没想好怎么设计
     * @LABEL  用来显示图标注的, 如果设置显示标注,就从数据表里面把标注内容填充到该字段
     * @PEN    图元画笔RGB色 本来要直接保存画笔的,但是现在还不会,只能先保存RGB颜色了
     * @BRUSH  图元画刷RGB色
     * @INFO   保存图元GIS信息的,点类数据只保存一个WGS坐标点文本(99.0000,27.1234),
     *         非点类图元保存多点WGS坐标(99.0000,27.1234_100.1234,27.1234),这个功能现在还没想好怎么处理,以后再说
     *
    */
    sql =QString("CREATE TABLE %1INFO (ILONGID REAL, TYPE REAL, X1 REAL, Y1 REAL, X2 REAL, WIDTH REAL,"
                 " HEIGHT REAL, LABEL TEXT, PEN TEXT, BRUSH TEXT, INFO TEXT)").arg(id);
    nonResult(sql, "initLayer create info table ");
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



