#include "dialogaddoperator.h"
#include "ui_dialogaddoperator.h"
#include <QSqlQuery>
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

#include "popup.h"

DialogAddOperator::DialogAddOperator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAddOperator)
{
    ui->setupUi(this);
    QValidator *validator = new QRegularExpressionValidator(QRegularExpression("^[a-zA-Z0-9]+$"), this);
    ui->leLoginAdd->setValidator(validator);
    ui->lePasswordAdd->setValidator(validator);
    ui->leRepitPasswordAdd->setValidator(validator);
    ui->leFIOAdd->setValidator(new QRegularExpressionValidator(QRegularExpression("^[а-яА-ЯёЁa-zA-Z .]+$"), this));

    //создание объекта выпадающего меню
    for (int i=0; i < 4; i++) {
        listPopUp.append(new PopUp());
    }
}

DialogAddOperator::~DialogAddOperator()
{
    delete ui;
}

void DialogAddOperator::on_pbAdd_clicked()
{
    bool flag = false;
    if (ui->leLoginAdd->text().isEmpty()) {
        ui->leLoginAdd->setStyleSheet("border: 1px solid red; min-height: 20px; border-radius: 2px;");
        listPopUp.at(0)->setPopupText("Заполните поле!");
        listPopUp.at(0)->show(DialogAddOperator::geometry().x() + ui->leLoginAdd->x() + ui->leLoginAdd->width() + 10, DialogAddOperator::geometry().y() + ui->leLoginAdd->y() - 13);
        flag=true;
    }
    if (ui->lePasswordAdd->text().isEmpty()) {
        ui->lePasswordAdd->setStyleSheet("border: 1px solid red; min-height: 20px; border-radius: 2px;");
        listPopUp.at(1)->setPopupText("Заполните поле!");
        listPopUp.at(1)->show(DialogAddOperator::geometry().x() + ui->lePasswordAdd->x() + ui->lePasswordAdd->width() + 10, DialogAddOperator::geometry().y() + ui->lePasswordAdd->y() - 13);
        flag=true;
    }
    if (ui->leRepitPasswordAdd->text().isEmpty()) {
        ui->leRepitPasswordAdd->setStyleSheet("border: 1px solid red; min-height: 20px; border-radius: 2px;");
        listPopUp.at(2)->setPopupText("Заполните поле!");
        listPopUp.at(2)->show(DialogAddOperator::geometry().x() + ui->leRepitPasswordAdd->x() + ui->leRepitPasswordAdd->width() + 10, DialogAddOperator::geometry().y() + ui->leRepitPasswordAdd->y() - 13);
        flag=true;
    } else if (ui->lePasswordAdd->text() != ui->leRepitPasswordAdd->text()) {
        ui->leRepitPasswordAdd->setStyleSheet("border: 1px solid red; min-height: 20px; border-radius: 2px;");
        listPopUp.at(2)->setPopupText("Пароли не совпадают!");
        listPopUp.at(2)->show(DialogAddOperator::geometry().x() + ui->leRepitPasswordAdd->x() + ui->leRepitPasswordAdd->width() + 10, DialogAddOperator::geometry().y() + ui->leRepitPasswordAdd->y() - 13);
        flag=true;
    }
    if (ui->leFIOAdd->text().isEmpty()) {
        ui->leFIOAdd->setStyleSheet("border: 1px solid red; min-height: 20px; border-radius: 2px;");
        listPopUp.at(3)->setPopupText("Заполните поле!");
        listPopUp.at(3)->show(DialogAddOperator::geometry().x() + ui->leFIOAdd->x() + ui->leFIOAdd->width() + 10, DialogAddOperator::geometry().y() + ui->leFIOAdd->y() - 13);
        flag=true;
    }

    if (flag == false) {
    QSqlQuery query;
        query.prepare("INSERT INTO Auth (Login, Password, FIO, Type) VALUES(?, ?, ?, ?)");
        query.addBindValue(ui->leLoginAdd->text());
        query.addBindValue(ui->lePasswordAdd->text());
        query.addBindValue(ui->leFIOAdd->text());
        query.addBindValue(1);
        qDebug() <<query.exec();
    } else {
        return;
    }
}

void DialogAddOperator::moveEvent(QMoveEvent *e){
    listPopUp.at(0)->move(DialogAddOperator::geometry().x() + ui->leLoginAdd->x() + ui->leLoginAdd->width() + 10, DialogAddOperator::geometry().y() + ui->leLoginAdd->y() - 13);
    listPopUp.at(1)->move(DialogAddOperator::geometry().x() + ui->lePasswordAdd->x() + ui->lePasswordAdd->width() + 10, DialogAddOperator::geometry().y() + ui->lePasswordAdd->y() - 13);
    listPopUp.at(2)->move(DialogAddOperator::geometry().x() + ui->leRepitPasswordAdd->x() + ui->leRepitPasswordAdd->width() + 10, DialogAddOperator::geometry().y() + ui->leRepitPasswordAdd->y() - 13);
    listPopUp.at(3)->move(DialogAddOperator::geometry().x() + ui->leFIOAdd->x() + ui->leFIOAdd->width() + 10, DialogAddOperator::geometry().y() + ui->leFIOAdd->y() - 13);
    QWidget::moveEvent(e);
}

void DialogAddOperator::on_pbCancel_clicked()
{
    close();
    listPopUp.at(0)->close();
    listPopUp.at(1)->close();
    listPopUp.at(2)->close();
    listPopUp.at(3)->close();
}
