#ifndef DIALOGADDOPERATOR_H
#define DIALOGADDOPERATOR_H

#include <QDialog>
#include "popup.h"

namespace Ui {
class DialogAddOperator;
}

class DialogAddOperator : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddOperator(QWidget *parent = nullptr);
    ~DialogAddOperator();
    void moveEvent(QMoveEvent *event);

private slots:

    void on_pbAdd_clicked();

    void on_pbCancel_clicked();

private:
    Ui::DialogAddOperator *ui;
    //указатель для объекта выпадающего меню
    QList<PopUp *> listPopUp;
};

#endif // DIALOGADDOPERATOR_H
