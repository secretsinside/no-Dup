#ifndef DIALOG_DELETE_H
#define DIALOG_DELETE_H

#include <QDialog>

namespace Ui {
class Dialog_delete;
}

class Dialog_delete : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_delete(QWidget *parent = 0);
    void insertItemInList(QString directory, int flagValue);
    ~Dialog_delete();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Dialog_delete *ui;
};

#endif // DIALOG_DELETE_H
