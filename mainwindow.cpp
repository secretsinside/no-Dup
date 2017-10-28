#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDirIterator>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QFileInfo>
#include <QVector>
#include "dialog_delete.h"
#include <QListWidgetItem>
#include <QAbstractAnimation>
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QRect rec= QApplication::desktop()->screenGeometry();
    int h= rec.height()/2;
    int w= rec.width()/2;
    this->setFixedSize(w, h);
    h-=this->height();
    w-=this->width()/2;
    this->move(h, w);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_browse_clicked()
{
    QString dir, dummy="";
    dir=ui->lineEdit_dir->text();
    dir= QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                   dir,
                                                   QFileDialog::ShowDirsOnly
                                                   | QFileDialog::DontResolveSymlinks);
    ui->lineEdit_dir->setText(dir);
}

void MainWindow::on_radioButton_only_clicked()
{
    ui->checkBox_image->setEnabled(true);
    ui->checkBox_text->setEnabled(true);
    ui->checkBox_video->setEnabled(true);
    ui->lineEdit_customExtension->setEnabled(false);
}

void MainWindow::on_radioButton_all_clicked()
{
    ui->checkBox_image->setEnabled(false);
    ui->checkBox_text->setEnabled(false);
    ui->checkBox_video->setEnabled(false);
    ui->lineEdit_customExtension->setEnabled(false);
}

void MainWindow::on_radioButton_custom_clicked()
{
    ui->checkBox_image->setEnabled(false);
    ui->checkBox_text->setEnabled(false);
    ui->checkBox_video->setEnabled(false);
    ui->lineEdit_customExtension->setEnabled(true);
}

void MainWindow::on_pushButton_start_clicked()
{
    ui->textBrowser_filename->setLineWrapMode(QTextEdit::NoWrap);
    ui->textBrowser_filename->setText(" ");     //for every time start button is pressed
    QString dir= ui->lineEdit_dir->text();
    QVector<QString> textFile={"*.txt", "*.pdf", "*.doc", "*.docx", "*.xlx", "*.ppt", "*.pptx"};
    QVector<QString> imageFile={"*.jpg", "*.jpeg", "*.png", "*.svg"};
    QVector<QString> videoFile={"*.mp4", "*.flv", "*.3gp", "*.avi"};
    QStringList filters;
    if(ui->radioButton_all->isChecked())
    {
        for(int i=0; i<textFile.size(); i++)
        {
            filters<<textFile[i];
        }
        for(int i=0; i<imageFile.size(); i++)
        {
            filters<<imageFile[i];
        }
        for(int i=0; i<videoFile.size(); i++)
        {
            filters<<videoFile[i];
        }
    }
    else if(ui->radioButton_only->isChecked())
    {
        if(ui->checkBox_text->isChecked())
        {
            for(int i=0; i<textFile.size(); i++)
            {
                filters<<textFile[i];
            }
        }
        if(ui->checkBox_image->isChecked())
        {
            for(int i=0; i<imageFile.size(); i++)
            {
                filters<<imageFile[i];
            }
        }
        if(ui->checkBox_video->isChecked())
        {
            for(int i=0; i<videoFile.size(); i++)
            {
                filters<<videoFile[i];
            }
        }
    }
    else if(ui->radioButton_custom->isChecked())
    {
        filters<<QString("*"+QString(ui->lineEdit_customExtension->text()));
    }

    //DATABASE
    QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE");
    if(!db.isOpen())
    {
        db.setDatabaseName("sample");
        db.open();
    }
    QSqlQuery query;
    query.exec("create table dataAndDirectory(dir varchar(1000), id int, duplicacy int, filename varchar(1000), PRIMARY KEY(filename))");
    query.exec("create table duplicateDir(dir varchar(1000), id int)");

    QDirIterator it(dir, filters, QDir::NoDotAndDotDot | QDir::AllEntries , QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
    int id=0;
    while(it.hasNext())
    {
        it.next();
        if(QFileInfo(it.filePath()).isFile())
        {

            QString name=it.fileName();
            ui->textBrowser_filename->append(name);
            QCoreApplication::processEvents();

            if(query.exec(QString(QString("insert into dataAndDirectory values ('")+ it.filePath()+ QString("',")+ QString::number(id)+ QString(",0,'")+ it.fileName() + QString("')"))))
            {
                qDebug()<< "okay";
                id++;
            }
            else{
                query= db.exec(QString(QString("select id from dataAndDirectory where filename='")+ it.fileName()+ QString("'")));
                query.first();
                //insert the directory and id into duplicateDir table
                query.exec(QString(QString("insert into duplicateDir values ('")+
                                             it.filePath()+
                                             QString("',")+
                                             QString::number(query.value(0).toInt())+
                                             QString(")")));
                qDebug()<< query.exec(QString(QString("UPDATE dataAndDirectory "
                                   "SET duplicacy=1 "
                                   "WHERE filename='")+
                                   QString(it.fileName())+
                                   QString("'")));
            }
        }
    }
    Dialog_delete dialogDelete;
    query= db.exec(QString("select id, filename, dir from dataAndDirectory where duplicacy>=1"));
    while(query.next())
    {
        dialogDelete.insertItemInList(query.value(1).toString(), 0);
        dialogDelete.insertItemInList(query.value(2).toString(), 1);
        QSqlQuery fetchDuplicate;
        fetchDuplicate= db.exec(QString(QString("SELECT dir FROM duplicateDir WHERE id=")+ query.value(0).toString()));
        while(fetchDuplicate.next())
        {
            dialogDelete.insertItemInList(fetchDuplicate.value(0).toString(), 1);
        }
    }

    dialogDelete.exec();
    query.exec("drop table duplicateDir");
    query.exec("drop table dataAndDirectory");
    db.close();
}
