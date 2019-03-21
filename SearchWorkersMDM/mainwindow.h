#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QSqlTableModel>
#include <QTableWidget>
#include <QLineEdit>
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

    void on_pbApply_2_clicked();

    void on_pbAddCorp_clicked();

    void on_pbCancel_2_clicked();

    void on_pbListCorps_clicked();

    void on_pbBack_2_clicked();

    void on_pbSave_2_clicked();

    void on_pbRemoveRow_2_clicked();

    void on_pbSearch_2_clicked();

private:
    Ui::MainWindow *ui;
    //переменная для бд
    QSqlDatabase db;
    //функция для создания QTableWidgets
    void createUI(QTableWidget* parent, QString tableName);
    //фнкция создания для второй таблицы
    bool StopFlag = true;
    //указатель для календаря
    CalendarWidget *calendar;
    //функция для сортировки
    void sort(QTableWidget* parent, QString tableName, QLineEdit* edit, QString filter);
    //флаг изменений
    bool changeData = false;
    //функция для сохранения таблицы
    void saveTable(QTableWidget* parent, QString tableName);
    //функция удаления строки в таблице
    void removeRow(QTableWidget* parent, QString tableName, QLineEdit* edit);
    //функция создания таблицы
    void createTable(QTableWidget* parent, const QStringList &headers);
    //функция создания таблицы
    void createSecondTable();
    //список таблиц
    QList<QTableWidget *> listTable1;
    QList<QTableWidget *> listTable2;
    QList<QTableWidget *> listTable3;
    QList<QTableWidget *> listTable4;
    QList<QTableWidget *> listTable5;

private slots:
    void showPup();


    void on_pbMoreBack_clicked();
    void on_tableWidget_itemDoubleClicked(QTableWidgetItem *item);
};

#endif // MAINWINDOW_H
