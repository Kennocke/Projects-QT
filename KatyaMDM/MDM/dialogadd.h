#ifndef DIALOGADD_H
#define DIALOGADD_H

#include <QDialog>

namespace Ui {
class DialogAdd;
}

class DialogAdd : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAdd(QWidget *parent = 0, QString login="admin");

    ~DialogAdd();

public slots:
    void on_buttonBox_accepted();


signals:
    //сигнал-триггер для вызова метода обновления модели
    void transmitter();

private slots:
    void on_leCount_editingFinished();

    void on_lePrice_editingFinished();

    void on_leCount_textEdited(const QString &arg1);

    void on_lePrice_textEdited(const QString &arg1);

    void on_leLifetime_textEdited(const QString &arg1);


private:
    Ui::DialogAdd *ui;
    //логин текущего пользователя
    QString addCurrentLogin;
};

#endif // DIALOGADD_H
