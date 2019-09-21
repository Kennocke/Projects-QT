#include "dialogchange.h"
#include "ui_dialogchange.h"
#include <QDebug>
#include <QSqlQuery>

DialogChange::DialogChange(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogChange)
{
    ui->setupUi(this);
}

DialogChange::~DialogChange()
{
    delete ui;
}

void DialogChange::setDB(QSqlTableModel *model1, int index)
{
    indexGlobal = index;
    modelGlobal = model1;
    ui->leName->setText(model1->index(index, 1).data().toString());
    ui->leAge->setText(model1->index(index, 2).data().toString());
}

void DialogChange::on_buttonBox_accepted()
{
    QSqlQuery query;
    QString test = "UPDATE People SET Name='" +
            modelGlobal->index(indexGlobal, 1).data().toString() + "', Age='" +
            modelGlobal->index(indexGlobal, 2).data().toString() + "' WHERE id=" +
            modelGlobal->index(indexGlobal, 0).data().toString() + "";
    bool flag = query.exec("UPDATE People SET Name='" +
                           ui->leName->text() + "',Age='" +
                           ui->leAge->text() + "' WHERE _id=" +
                           modelGlobal->index(indexGlobal, 0).data().toString() + ""
                           );
    qDebug() << flag << " " <<  modelGlobal->index(indexGlobal, 0).data().toString();
    qDebug() << test;
    modelGlobal->select();
}
