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
    void setDB(QSqlTableModel *model1, int index);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::DialogChange *ui;
    int indexGlobal;
    QSqlTableModel *modelGlobal;
};

#endif // DIALOGCHANGE_H
