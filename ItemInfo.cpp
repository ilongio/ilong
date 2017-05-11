#include "ItemInfo.h"
#include "ui_ItemInfo.h"

ItemInfo::ItemInfo(ILong *parent, QString LName, QString Iid) :
    QDialog(parent),iLong(parent),layerName(LName),itemID(Iid),sqlExcute(&parent->sqlExcute),
    ui(new Ui::ItemInfo)
{
    ui->setupUi(this);
    model = new QStandardItemModel();
    model->setColumnCount(2);
    model->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("Key"));
    model->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("Value"));
    ui->tableView->setModel(model);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->selectRow(0);
    Layer * layer = iLong->manager->getLayer(layerName);
    QSqlQuery * query = sqlExcute->getItemInfo(layer->getLayerID(),itemID);
    if(query->next())
    {
        QList<LayerFormat> * formatList = layer->getLayerHead();
        for(int i=0; i<formatList->size(); i++)
        {
            model->setItem(i,0,new QStandardItem(formatList->at(i).name));
            QString value = query->value(i).toString();
            model->setItem(i,1,new QStandardItem(value));
        }
        model->setItem(0,0,new QStandardItem(layerName));
        delete query;
        query = 0;
    }
#ifdef ANDROID
    setWindowState(windowState() | Qt::WindowMaximized);
#endif
    ui->tableView->resizeColumnsToContents();
}

ItemInfo::~ItemInfo()
{
    delete ui;
}
