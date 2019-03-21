#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlTableModel>
#include "popup.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void moveEvent(QMoveEvent *event);

private slots:
    void on_pbLogin_clicked();

    void on_pbAddClient_clicked();


    void on_pbCancelAdd_clicked();

    void on_pbAcceptAdd_clicked();

    void on_pbAcceptCredit_clicked();

    void on_pbCreditHistory_clicked();

    void on_pbSave_clicked();

    void on_pbDelete_clicked();

    void on_pbBackCreditHistory_clicked();

    void on_pbExit_clicked();

    void on_pbStartSearch_clicked();

    void on_pbCancelCredit_clicked();

    void on_leBirthdate_textChanged(const QString &arg1);

    void on_pbAdminExit_clicked();

    void on_pbAddOperator_clicked();

    void on_pbDeleteOperator_clicked();

    void on_hsCountMoney_valueChanged(int value);

    void on_leShowMoney_editingFinished();

    void on_cbMonth_currentIndexChanged(const QString &arg1);

    void on_leFirstPay_editingFinished();

    void on_leShowMoney_textChanged(const QString &arg1);

    void on_leMoneyMonth_editingFinished();

    void on_leFirstPay_textChanged(const QString &arg1);

    void on_leFIO_textChanged(const QString &arg1);

    void on_leHomeAddress_textChanged(const QString &arg1);

    void on_lePasport_textChanged(const QString &arg1);

    void on_leMoney_textChanged(const QString &arg1);

    void on_leHomePhone_textChanged(const QString &arg1);

    void on_leMail_textChanged(const QString &arg1);

    void on_leExp_textChanged(const QString &arg1);

    void on_leWorkAddress_textChanged(const QString &arg1);

    void on_leNameWork_textChanged(const QString &arg1);

    void on_leWorkPhone_textChanged(const QString &arg1);

    void on_leFioParent_textChanged(const QString &arg1);

    void on_lePhoneParent_textChanged(const QString &arg1);

    void on_leMoneyMonth_textChanged(const QString &arg1);

    void on_cbMonth_currentTextChanged(const QString &arg1);

    void on_pbPrint_clicked();

private:
    Ui::MainWindow *ui;
    //переменная для бд
    QSqlDatabase db;
    //модель для бд
    QSqlTableModel* model;
    //модель для таблицы с аккаунтами
    QSqlTableModel* modelOperator;
    //текущий id клиента
    int currentId = 0;
    //текущий номер паспорт
    QString currentPassport;
    //указатель для объекта выпадающего меню
    QList<PopUp *> listPopUp;
    //флаг для точек в поле дата
    bool flagDot = false;
    QRegExp *rxMail;
    //функция расчёта кредита
    void countCredit();
    //функция-запрос на добавление кредита в базу данных
    void addCredit(int id);
    //переменная отвечающая за первый платеж
    double prev;
};

#endif // MAINWINDOW_H
