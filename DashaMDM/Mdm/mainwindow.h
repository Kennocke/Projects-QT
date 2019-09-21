#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QSqlTableModel>
#include "calendarwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    //список дат
    QList<QDate> *listDate;

private slots:

    void on_pbLogIn_clicked();

    void on_pbApply_clicked();

    void on_pbCandidates_clicked();

    void on_pbSearch_clicked();

    void on_pbBack_clicked();

    void on_pbCalendar_clicked();

    void on_pbOrder_clicked();

    void on_pbBackCalendar_clicked();

    void on_pbSave_clicked();

    void on_leSearch_editingFinished();

    void on_pbPrint_clicked();

    void on_pbLogOut_clicked();

    void DataChange() {
        changeData = true;
    }

    void on_pbCancel_clicked();

    void on_pbRemoveRow_clicked();

private:
    Ui::MainWindow *ui;
    //переменная для бд
    QSqlDatabase db;
    //перенная-указатель для модели
    QSqlTableModel *model;
    //функция для создания QTableWidgets
    void createUI(const QStringList &headers);
    //флаг отсановки создания новых столбцов и строк
    bool StopFlag = true;
    //указатель для календаря
    CalendarWidget *calendar;
    //функция для сортировки
    void sort(QString filter);
    //флаг изменений
    bool changeData = false;
};

#endif // MAINWINDOW_H
