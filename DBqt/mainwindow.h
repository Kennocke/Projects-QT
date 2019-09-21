#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QStandardItemModel"
#include "QStandardItem"
#include "QSqlTableModel"
#include "QSqlRecord"
#include <QDebug>
#include "dialogchange.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    DialogChange *form;
    QSqlTableModel *model;
    QStandardItem *item;
    QSqlDatabase db;

private slots:
    void on_pbConnect_clicked();

    void on_pbRemove_clicked();

    void on_pbAdd_clicked();

    void on_pushButton_clicked();

public slots:
    void on_pbChangeRecord_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
