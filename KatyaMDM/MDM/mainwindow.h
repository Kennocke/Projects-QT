#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "dialogadd.h"
#include "dialogchange.h"

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QSqlTableModel>
#include <QTextDocument>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    DialogAdd *formAdd;
    DialogChange *formChange;
    QSqlDatabase db;
    QSqlTableModel *model;
    //метод для обновления модели
    void changeModel();
    //метод обновления итога
    void updateResult();

private slots:
    void on_pbAdd_clicked();

    void on_pbRemove_clicked();

    void on_pbChange_clicked();

    void on_pbPrint_clicked();

    void on_pbLogin_clicked();

    void on_pbExit_clicked();

    void on_pbPrintRow_clicked();

    void on_pbSearch_clicked();

    void on_leSearch_editingFinished();

    void on_pushButton_clicked();

    void on_pbPrintReports_clicked();

    void on_pbReportBack_clicked();

    void on_leSearch_textChanged(const QString &arg1);

    void on_lePassword_returnPressed();

private:
    Ui::MainWindow *ui;

    //текущий логин сотрудника
    QString currentLogin;
    //текстовый документ для печати
    QTextDocument *document;

};

#endif // MAINWINDOW_H
