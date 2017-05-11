#ifndef ITEMINFO_H
#define ITEMINFO_H

#include <QDialog>
#include <QStandardItemModel>
#include <QApplication>
#include <QDesktopWidget>
#include "ILong.h"

namespace Ui {
class ItemInfo;
}

class ItemInfo : public QDialog
{
    Q_OBJECT

public:
    explicit ItemInfo(ILong *parent, QString LName, QString Iid);
    ~ItemInfo();

private:
    ILong * iLong;
    QString layerName;
    QString itemID;
    SQLExcute * sqlExcute;
    Ui::ItemInfo *ui;
    QStandardItemModel  *model;
};

#endif // ITEMINFO_H
