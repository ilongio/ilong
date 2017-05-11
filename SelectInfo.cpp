#include "SelectInfo.h"
#include "ui_SelectInfo.h"
#include "ItemInfo.h"
#include <QDebug>

SelectInfo::SelectInfo(ILong *parent) :
    QDialog(parent),iLong(parent),
    ui(new Ui::SelectInfo)
{
    ui->setupUi(this);
    model = new QStandardItemModel();
    model->setColumnCount(3);
    model->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("Layer"));
    model->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("ID"));
    model->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit("Label"));
    ui->tableView->setModel(model);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->selectRow(0);
    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(viewClicked(QModelIndex)));
#ifdef ANDROID
    setWindowState(windowState() | Qt::WindowMaximized);
#endif
}

SelectInfo::~SelectInfo()
{
    delete ui;
}

QStandardItemModel *SelectInfo::getModel()
{
    return model;
}

void SelectInfo::resizeEvent(QResizeEvent *event)
{
    event->accept();
    ui->tableView->resizeColumnsToContents();
}

void SelectInfo::viewClicked(QModelIndex index)
{
    QString layerName = model->index(index.row(),0).data().toString();
    QString itemID = model->index(index.row(),1).data().toString();
    ItemInfo itemInfo(iLong, layerName, itemID);
    itemInfo.exec();
}
