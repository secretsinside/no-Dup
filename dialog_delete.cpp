#include "dialog_delete.h"
#include "ui_dialog_delete.h"
#include <QListWidgetItem>
#include <QDebug>
#include <QDir>
#include <QFile>

Dialog_delete::Dialog_delete(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_delete)
{
    ui->setupUi(this);
}

Dialog_delete::~Dialog_delete()
{
    delete ui;
}

void Dialog_delete::insertItemInList(QString directory, int flagValue)
{
    QListWidgetItem *li= new QListWidgetItem(directory);
    if(flagValue==1)
    {
        li->setFlags(li->flags() | Qt::ItemIsUserCheckable);
        li->setCheckState(Qt::Unchecked);
    }
    ui->listWidget_dirList->addItem(li);
}

void Dialog_delete::on_pushButton_clicked()
{
    QFile file;
    for(int row=0; row< ui->listWidget_dirList->count(); row++)
    {
        QListWidgetItem *item= ui->listWidget_dirList->item(row);
        if(item->checkState()==2)
        {
            qDebug()<< item->text();
            qDebug()<< file.remove(item->text());
        }
    }
}
