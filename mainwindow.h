#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_browse_clicked();

    void on_radioButton_only_clicked();

    void on_radioButton_all_clicked();

    void on_radioButton_custom_clicked();

    void on_pushButton_start_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
