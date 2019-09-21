#ifndef DIALOGCHANGE_H
#define DIALOGCHANGE_H

#include <QDialog>
#include <QSqlTableModel>

namespace Ui {
class DialogChange;
}

class DialogChange : public QDialog
{
    Q_OBJECT

public:
    explicit DialogChange(QWidget *parent = 0);
    ~DialogChange();
    //функция для установки полей записи в поля формы
    void setDB(QSqlTableModel *model, int index);

signals:
    //сигнал-триггер для вызова метода обновления модели
    void transmitter();

private slots:
    void on_buttonBox_accepted();

    void on_leCount_editingFinished();

    void on_lePrice_editingFinished();

    void on_leCount_textEdited(const QString &arg1);

    void on_lePrice_textEdited(const QString &arg1);

    void on_leLifetime_textEdited(const QString &arg1);

private:
    Ui::DialogChange *ui;
    //переменная для хранения ID записи
    int indexGlobal;
    //переменная для хранения модели
    QSqlTableModel *modelGlobal;
};

#endif // DIALOGCHANGE_H
