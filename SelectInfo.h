#ifndef SELECTINFO_H
#define SELECTINFO_H

#include <QDialog>
#include <QStandardItemModel>
#include <QApplication>
#include <QDesktopWidget>
#include "ILong.h"

namespace Ui {
class SelectInfo;
}

class SelectInfo : public QDialog
{
    Q_OBJECT

public:
    explicit SelectInfo(ILong *parent);
    ~SelectInfo();
    QStandardItemModel * getModel();
protected:
    void resizeEvent(QResizeEvent *event);
public slots:
    void viewClicked(QModelIndex index);
private:
    ILong * iLong;
    Ui::SelectInfo *ui;
    QStandardItemModel  *model;
};

#endif // SELECTINFO_H
