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

void SQLExcute::addItems(QList<Geometry::ILongDataType> *dataList,
                         QString id, QList<ILongType> *headType)
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
            qDebug() << db.lastError().text();
            return;
        }
    }
    QSqlQuery query(db);
    db.transaction();
    for(int i=0; i<dataList->size(); i++)
    {
        Geometry::ILongDataType data = dataList->at(i);
        if(data.data.size() < headType->size())
        {
            qDebug() << "data error";
            continue;
        }
        /*
         * 先把数据插入到数据表先
         * */
        QString sqlT = "";
        for(int j=0; j<headType->size(); j++)
        {
            if(headType->at(j) == ILongNUMBER)
            {
                /*
                 * 其实感觉没必要检查是不是能转换成数字了,但是我人好嘛,慢点就慢点了
                 * 等有好办法再说
                 * */
                bool ok;
                qreal result = data.data.at(j).toReal(&ok);
                if(!ok)
                    result = 0;
                sqlT += QString(" '%1',").arg(result);
            }
            else
            {
                sqlT += " '" + data.data.at(j).toString() + "' ";
            }
        }
        sqlT = sqlT.left(sqlT.length()-1);
        QString sql = QString("INSERT INTO '%1' VALUES ( '%2', %3 )").arg(id).arg(data.geometry->getID()).arg(sqlT);
        if(!query.exec(sql))
        {
            /*
             * 如果插入数据表失败就没必要插入信息表了,直接跳过处理这个图元了
             * */
            qDebug() << query.lastError().text() << sql ;
            continue;
        }
        /*
         *现在再开始插入图元信息到信息表里 好像还不晚嘛
         * @ILONGID     与数据的ID关联;
         * @TYPE        ILongGeoType 枚举图元类型
         * @CenterX     图元wgs CenterX 坐标
         * @CenterY     图元wgs CenterX 坐标
         * @MINX        图元最小wgs X坐标 (点类图元写CenterX相同)
         * @MINY        图元最小wgs X坐标 (点类图元写CenterY相同)
         * @MAXX        图元最大wgs X坐标 (点类图元写CenterX相同)
         * @MAXY        图元最大wgs Y坐标 (点类图元写CenterY相同) 设计两个坐标点只为了非点类图元需要计算边界问题,比如线
         * @LABEL       用来显示图标注的, 如果设置显示标注,就从数据表里面把标注内容填充到该字段
         * @INFO        保存图元GIS信息
         *              格式: WGSx1,WGSy1_WGSx2,WGSy2_..._WGSxN,WGSyN-线宽-箭头方向-大小-画笔(R_G_B)-画刷(R_G_B)
         *              线宽 箭头方向  只对面类图元有影响
         *              大小         只对点类图元有影响
         *
        */
        QPointF cen = data.geometry->getCenter();
        ILongGeoRect rect = data.geometry->getRect();
        QString info = QString("%1-%2-%3-%4-%5-%6").arg(data.geometry->getPoints())
                .arg(data.geometry->getLineWidth()).arg(data.geometry->getLineType())
                .arg(data.geometry->getSize()).arg(data.geometry->getPen()).arg(data.geometry->getBrush());
        sql = QString("INSERT INTO '%1INFO' VALUES ( '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10', '%11' )")
                .arg(id).arg(data.geometry->getID()).arg(data.geometry->getGeoType()).arg(cen.x())
                .arg(cen.y()).arg(rect.minX)
                .arg(rect.minY).arg(rect.maxX).arg(rect.maxY).arg("ILONGNULL").arg(info);
        if(!query.exec(sql))
        {
            /*
             * 如果图元信息表插入失败,得删除数据表里对应的数据
             * */
            qDebug() << query.lastError().text() << sql ;
            sql = QString("DELETE FROM '%1' WHERE ILONGID = %2 ").arg(id).arg(data.geometry->getID());
            if(!query.exec(sql))
            {
                /*
                 * 如果删除失败,那就出现数据混乱了,可以把图层删除了再导入图层吧
                 * */
                qDebug() << query.lastError().text() << sql ;
            }
        }
    }
    db.commit();
    db.transaction();
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
     * @TYPE        ILongGeoType 枚举图元类型
     * @CenterX     图元wgs CenterX 坐标
     * @CenterY     图元wgs CenterX 坐标
     * @MINX        图元最小wgs X坐标 (点类图元写CenterX相同)
     * @MINY        图元最小wgs X坐标 (点类图元写CenterY相同)
     * @MAXX        图元最大wgs X坐标 (点类图元写CenterX相同)
     * @MAXY        图元最大wgs Y坐标 (点类图元写CenterY相同) 设计两个坐标点只为了非点类图元需要计算边界问题,比如线
     * @LABEL       用来显示图标注的, 如果设置显示标注,就从数据表里面把标注内容填充到该字段
     * @INFO        保存图元GIS信息
     *              格式: WGSx1,WGSy1_WGSx2,WGSy2_..._WGSxN,WGSyN-线宽-箭头方向-大小-画笔(R_G_B)-画刷(R_G_B)
     *              箭头方向 只对面类图元有影响
     *              大小    只对点类图元有影响
     *
    */
    sql =QString("CREATE TABLE %1INFO (ILONGID REAL, TYPE REAL, CenterX REAL, CenterY REAL, "
                 "MINX REAL, MINY REAL, MAXX REAL, MAXY REAL, LABEL TEXT, INFO TEXT)").arg(id);
    nonResult(sql, "initLayer create info table ");
}

void SQLExcute::removeLayer(QString id)
{
    QString sql = QString("DELETE FROM ILONGIOLAYER WHERE ID = '%1' ").arg(id);
    nonResult(sql, "removeLayer on ILONGIOLAYER " + id);
    sql = QString("DROP TABLE '%1' ").arg(id);
    nonResult(sql, "DROP TABLE " + id);
    sql = QString("DROP TABLE '%1INFO' ").arg(id);
    nonResult(sql, "DROP INFO TABLE " + id);
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



