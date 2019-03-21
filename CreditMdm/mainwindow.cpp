#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QTabBar>
#include <QDir>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>
#include <QtSql/QSqlTableModel>
#include <QMoveEvent>
#include "itemdelegate.h"
#include "dialogaddoperator.h"
#include <QDate>
#include "popup.h"
#include <QTextDocument>
#include <QPrinter>
#include <QPrintDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //???????? TabBar ? QTabWidget
    ui->tabWidget->tabBar()->hide();

    //?????? accessibleName ???????
    ui->tvAdmin->setAccessibleName("AdminTable");
    ui->tvCreditHistory->setAccessibleName("OperatorTable");


    //??????? ???? ??????
    db = QSqlDatabase::addDatabase("QSQLITE");
    //????????? ???? ? ???? ??????
    db.setDatabaseName(QDir::toNativeSeparators(QApplication::applicationDirPath()) + "/database.db3");
    //????????????? ?????????? ? ??
    if(db.open())
        qDebug() << "Успешное подключение!";

    model = new QSqlTableModel(this,db);
    model->setTable("CreditHistory");
    model->setHeaderData(2, Qt::Horizontal, "ФИО");
    model->setHeaderData(3, Qt::Horizontal, "Паспорт");
    model->setHeaderData(4, Qt::Horizontal, "Сумма кредита");
    model->setHeaderData(5, Qt::Horizontal, "Ежемесячный платёж");
    model->setHeaderData(6, Qt::Horizontal, "Дата оформления");
    model->setHeaderData(7, Qt::Horizontal, "Дата закрытия");
    model->setHeaderData(8, Qt::Horizontal, "Статус");
    //????????? ??????? ?????????
    ui->tvCreditHistory->setModel(model);
    ui->tvCreditHistory->resizeRowsToContents();
    ui->tvCreditHistory->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tvCreditHistory->hideColumn(0);
    ui->tvCreditHistory->hideColumn(1);
    ui->tvCreditHistory->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tvCreditHistory->setSelectionBehavior(QAbstractItemView::SelectRows);
    ItemDelegate *delg = new ItemDelegate();
    for (int j=2; j < ui->tvCreditHistory->horizontalHeader()->count(); j++)
        ui->tvCreditHistory->setItemDelegateForColumn(j, delg);
    //????????? ??????? ??????????????
    ui->tvAdmin->setModel(model);
    ui->tvAdmin->resizeRowsToContents();
    ui->tvAdmin->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tvAdmin->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tvAdmin->hideColumn(0);
    ui->tvAdmin->hideColumn(1);
    ui->tvAdmin->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tvAdmin->setSelectionBehavior(QAbstractItemView::SelectRows);
    for (int j=2; j < ui->tvAdmin->horizontalHeader()->count(); j++)
        ui->tvAdmin->setItemDelegateForColumn(j, delg);
    model->select();

    //????????? ????? ????? ??? ??????? "???????? ???????"
    //??????????? ???????? ??? ????? "?????? ??????"
    rxMail = new QRegExp("^([a-z0-9_-]+\\.)*[a-z0-9_-]+@[a-z0-9_-]+(\\.[a-z0-9_-]+)*\\.[a-z]{2,6}$");
    QRegularExpression rx("^[а-яA-ЯёЁa-zA-Z .]+$");
    QValidator *validatorMail = new QRegularExpressionValidator(QRegularExpression("^[a-zA-Z0-9!#$%&'*+-/=?^_`{|}~.@]+$"), this);
    QValidator *validator = new QRegularExpressionValidator(rx, this);
    QRegularExpression rxDate("^(0[1-9]|[12][0-9]|3[01])[- /.](0[1-9]|1[012])[- /.](19|20)[0-9][0-9]$");
    QValidator *validatorDate = new QRegularExpressionValidator(rxDate);
    ui->leFIO->setValidator(validator);
    ui->leBirthdate->setValidator(validatorDate);
    ui->leMoney->setValidator(new QDoubleValidator(1, 16777215, 2));
    ui->leExp->setValidator(new QIntValidator(2, 150, ui->leExp));
    ui->leNameWork->setValidator(validator);
    ui->leFioParent->setValidator(validator);
    ui->leMail->setValidator(validatorMail);

    //???????? ??? ??????? "??????"
    ui->leMoneyMonth->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]*([\\.]?[0-9]{0,2})?")));
    ui->leFirstPay->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]*([\\.]?[0-9]{0,2})?")));
    ui->leShowMoney->setValidator(new QIntValidator(1, 1000000));
    //???????? ??????? ??????????? ????
    for (int i=0; i < 14; i++) {
        listPopUp.append(new PopUp());
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pbLogin_clicked()
{
    //???? ???? ?? ??????
    if ((ui->leLogin->text() != "") && (ui->lePassword->text() != ""))
    {
        QSqlQuery query;
        query.prepare("SELECT * FROM Auth WHERE Login=?");
        query.addBindValue(ui->leLogin->text());
        //???? ???????????? ? ????? ??????? ??????
        if (query.exec())
        {
            qDebug() << "?????? ????????!";
            qDebug() <<  query.lastError().text();
            query.next();
            //???? ?????? ??????, ?? ????????? ??????
            if (ui->lePassword->text() == query.value(2).toString())
            {
                if (query.value(4).toInt() == 1) {
                    QMessageBox::information(nullptr, QString("Приветствие"), QString("Добро пожаловать " + query.value(3).toString()), QMessageBox::Ok);
                    ui->tabWidget->setCurrentWidget(ui->tabMenu);
                } else if(query.value(4).toInt() == 0) {
                    modelOperator = new QSqlTableModel(this,db);
                    modelOperator->setTable("Auth");
                    modelOperator->setHeaderData(1, Qt::Horizontal, "Логин");
                    modelOperator->setHeaderData(2, Qt::Horizontal, "Пароль");
                    modelOperator->setHeaderData(3, Qt::Horizontal, "ФИО");
                    modelOperator->setHeaderData(4, Qt::Horizontal, "Тип учётной записи");
                    ui->tvListOperator->setModel(modelOperator);
                    ui->tvListOperator->resizeRowsToContents();
                    ui->tvListOperator->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                    ui->tvListOperator->hideColumn(0);
                    ui->tvListOperator->setSelectionMode(QAbstractItemView::SingleSelection);
                    ui->tvListOperator->setSelectionBehavior(QAbstractItemView::SelectRows);
                    modelOperator->select();
                    QMessageBox::information(nullptr, QString("Приветствие"), QString("Добро пожаловать " + query.value(3).toString()), QMessageBox::Ok);
                    ui->tabWidget->setCurrentWidget(ui->tabAdminPanel);
                }
                //??????? ????
                ui->leLogin->setText(QString());
                ui->lePassword->setText(QString());
            } else {
                QMessageBox::information(nullptr, QString("Предупреждение"), QString("Неправильный пароль!"), QMessageBox::Ok);
                ui->lePassword->setText(QString());
            }
        } else {
            QMessageBox::information(nullptr, QString("Предупреждение"), QString("Пользователь не найден!"), QMessageBox::Ok);
            ui->leLogin->setText(QString());
            ui->lePassword->setText(QString());
        }

    } else {
        QMessageBox::information(nullptr, QString("Предупреждение"), QString("Заполните все необходимые поля!"), QMessageBox::Ok);
    }
}

void MainWindow::on_pbAddClient_clicked()
{
    ui->leMail->clear();
    ui->leExp->clear();
    ui->leFIO->clear();
    ui->leBirthdate->clear();
    ui->leHomeAddress->clear();
    ui->lePasport->clear();
    ui->leMoney->clear();
    ui->leHomePhone->clear();
    ui->leWorkAddress->clear();
    ui->leNameWork->clear();
    ui->leWorkPhone->clear();
    ui->leFioParent->clear();
    ui->lePhoneParent->clear();
    currentId=0;
    currentPassport.clear();

    ui->leShowMoney->setText("10000");
    ui->hsCountMoney->setValue(10000);
    countCredit();
    ui->tabWidget->setCurrentWidget(ui->tabCredit);
}

void MainWindow::on_pbCancelAdd_clicked()
{
    ui->tabWidget->setCurrentWidget(ui->tabMenu);
    for (int i=0; i < listPopUp.size(); i++) {
        listPopUp.at(i)->close();
    }
}

void MainWindow::on_pbAcceptAdd_clicked()
{
    ///Проверка полей
    //Если хоть одно условие верно
    bool flag = false;
    //ФИО
    if (ui->leFIO->text().isEmpty()) {
        ui->leFIO->setStyleSheet("border: 1px solid red; min-height: 20px; border-radius: 2px;");
        listPopUp.at(0)->setPopupText("Заполните поле!");
        listPopUp.at(0)->show(MainWindow::geometry().x() + ui->leFIO->x() + ui->leFIO->width() + 10, MainWindow::geometry().y() + ui->leFIO->y() + 13);
        flag=true;
    }
    //Дата рождения
    if (ui->leBirthdate->text().isEmpty()) {
        ui->leBirthdate->setStyleSheet("border: 1px solid red; min-height: 20px; border-radius: 2px;");
        listPopUp.at(1)->setPopupText("Заполните поле!");
        listPopUp.at(1)->show(MainWindow::geometry().x() + ui->leBirthdate->x() + ui->leFIO->width() + 10, MainWindow::geometry().y() + ui->leBirthdate->y() + 13);
        flag=true;
    } else if (ui->leBirthdate->text().size() != 10) {
        ui->leBirthdate->setStyleSheet("border: 1px solid red; min-height: 20px; border-radius: 2px;");
        listPopUp.at(1)->setPopupText("Неверный формат даты!");
        listPopUp.at(1)->show(MainWindow::geometry().x() + ui->leBirthdate->x() + ui->leFIO->width() + 10, MainWindow::geometry().y() + ui->leBirthdate->y() + 13);
        flag=true;
    } else {
        QDate date18 = QDate::currentDate().addYears(-18);
        QDate date60 = QDate::currentDate().addYears(-60);
        QDate date65 = QDate::currentDate().addYears(-65);
        qDebug() << date18;
        if (QDate::fromString(ui->leBirthdate->text(), Qt::LocalDate) > date18) {
            qDebug() << "????";
            ui->leBirthdate->setStyleSheet("border: 1px solid red; min-height: 20px; border-radius: 2px;");
            listPopUp.at(1)->setPopupText("Лицо не достигло 18 лет!");
            listPopUp.at(1)->show(MainWindow::geometry().x() + ui->leBirthdate->x() + ui->leBirthdate->width() + 10, MainWindow::geometry().y() + ui->leBirthdate->y() + 13);
            flag=true;
        } else if ((ui->cbSex->currentText() == "???") && (QDate::fromString(ui->leBirthdate->text(), Qt::LocalDate) < date65)) {
            ui->leBirthdate->setStyleSheet("border: 1px solid red; min-height: 20px; border-radius: 2px;");
            listPopUp.at(1)->setPopupText("Возрастное ограничение для мужчин - 65 лет");
            listPopUp.at(1)->show(MainWindow::geometry().x() + ui->leBirthdate->x() + ui->leBirthdate->width() + 10, MainWindow::geometry().y() + ui->leBirthdate->y() + 13);
            flag=true;
        } else if ((ui->cbSex->currentText() == "???") && (QDate::fromString(ui->leBirthdate->text(), Qt::LocalDate) < date60)) {
            ui->leBirthdate->setStyleSheet("border: 1px solid red; min-height: 20px; border-radius: 2px;");
            listPopUp.at(1)->setPopupText("Возрастное ограничение для женщин - 60 лет");
            listPopUp.at(1)->show(MainWindow::geometry().x() + ui->leBirthdate->x() + ui->leBirthdate->width() + 10, MainWindow::geometry().y() + ui->leBirthdate->y() + 13);
            flag=true;
        }
    }
    //Домашний адрес
    if (ui->leHomeAddress->text().isEmpty()) {
        ui->leHomeAddress->setStyleSheet("border: 1px solid red; min-height: 20px; border-radius: 2px;");
        listPopUp.at(2)->setPopupText("Заполните поле!");
        listPopUp.at(2)->show(MainWindow::geometry().x() + ui->leHomeAddress->x() + ui->leHomeAddress->width() + 10, MainWindow::geometry().y() + ui->leHomeAddress->y() + 13);
        flag=true;
    }
    //Номер паспорта
    if (ui->lePasport->text().size() != 11) {
        ui->lePasport->setStyleSheet("border: 1px solid red; min-height: 20px; border-radius: 2px;");
        listPopUp.at(3)->setPopupText("Неправильный формат! (серия-номер)");
        listPopUp.at(3)->show(MainWindow::geometry().x() + ui->lePasport->x() + ui->lePasport->width() + 10, MainWindow::geometry().y() + ui->lePasport->y() + 13);
        flag=true;
    }
    //Ежемесячный доход
    if (ui->leMoney->text().isEmpty()) {
        ui->leMoney->setStyleSheet("border: 1px solid red; min-height: 20px; border-radius: 2px;");
        listPopUp.at(4)->setPopupText("Заполните поле!");
        listPopUp.at(4)->show(MainWindow::geometry().x() + ui->leMoney->x() + ui->leMoney->width() + 10, MainWindow::geometry().y() + ui->leMoney->y() + 13);
        flag=true;
    } else if (ui->leMoney->text().toDouble() <= 10500) {
        ui->leMoney->setStyleSheet("border: 1px solid red; min-height: 20px; border-radius: 2px;");
        listPopUp.at(4)->setPopupText("Ваш доход ниже прожиточного минимума!");
        listPopUp.at(4)->show(MainWindow::geometry().x() + ui->leMoney->x() + ui->leMoney->width() + 10, MainWindow::geometry().y() + ui->leMoney->y() + 13);
        flag=true;
    }
    //Номер домашнего телефона
    if (ui->leHomePhone->text().size() != 17) {
        ui->leHomePhone->setStyleSheet("border: 1px solid red; min-height: 20px; border-radius: 2px;");
        listPopUp.at(5)->setPopupText("Неправильный формат номера телефона!");
        listPopUp.at(5)->show(MainWindow::geometry().x() + ui->leHomePhone->x() + ui->leHomePhone->width() + 10, MainWindow::geometry().y() + ui->leHomePhone->y() + 13);
        flag=true;
    }
    //Стаж
    if (ui->leExp->text().isEmpty()) {
        ui->leExp->setStyleSheet("border: 1px solid red; min-height: 20px; border-radius: 2px;");
        listPopUp.at(6)->setPopupText("Заполните поле!!");
        listPopUp.at(6)->show(MainWindow::geometry().x() + ui->leExp->x() + ui->leExp->width() + 10, MainWindow::geometry().y() + ui->leExp->y() + 13);
        flag=true;
    } else if (ui->leExp->text().toInt() < 2) {
        ui->leExp->setStyleSheet("border: 1px solid red; min-height: 20px; border-radius: 2px;");
        listPopUp.at(6)->setPopupText("Стаж должен быть более 1 года!");
        listPopUp.at(6)->show(MainWindow::geometry().x() + ui->leExp->x() + ui->leExp->width() + 10, MainWindow::geometry().y() + ui->leExp->y() + 13);
        flag=true;
    }
    //Почта
    if(rxMail->exactMatch(ui->leMail->text()) != true) {
        ui->leMail->setStyleSheet("border: 1px solid red; min-height: 20px; border-radius: 2px;");
        listPopUp.at(7)->setPopupText("Неправильный формат Email!");
        listPopUp.at(7)->show(MainWindow::geometry().x() + ui->leMail->x() + ui->leMail->width() + 10, MainWindow::geometry().y() + ui->leMail->y() + 13);
        flag=true;
    }
    //Место работы
    if (ui->leWorkAddress->text().isEmpty()) {
        ui->leWorkAddress->setStyleSheet("border: 1px solid red; min-height: 20px; border-radius: 2px;");
        listPopUp.at(8)->setPopupText("Заполните поле!");
        listPopUp.at(8)->show(MainWindow::geometry().x() + ui->leWorkAddress->x() + ui->leWorkAddress->width() + 10, MainWindow::geometry().y() + ui->leWorkAddress->y() + 13);
        flag=true;
    }
    //Название организации
    if (ui->leNameWork->text().isEmpty()) {
        ui->leNameWork->setStyleSheet("border: 1px solid red; min-height: 20px; border-radius: 2px;");
        listPopUp.at(9)->setPopupText("Заполните поле!");
        listPopUp.at(9)->show(MainWindow::geometry().x() + ui->leNameWork->x() + ui->leNameWork->width() + 10, MainWindow::geometry().y() + ui->leNameWork->y() + 13);
        flag=true;
    }
    //Рабочий номер телефона
    if (ui->leWorkPhone->text().size() != 17) {
        ui->leWorkPhone->setStyleSheet("border: 1px solid red; min-height: 20px; border-radius: 2px;");
        listPopUp.at(10)->setPopupText("Неправильный формат номера телефона!");
        listPopUp.at(10)->show(MainWindow::geometry().x() + ui->leWorkPhone->x() + ui->leWorkPhone->width() + 10, MainWindow::geometry().y() + ui->leWorkPhone->y() + 13);
        flag=true;
    }
    //ФИО доверенного лица
    if (ui->leFioParent->text().isEmpty()) {
        ui->leFioParent->setStyleSheet("border: 1px solid red; min-height: 20px; border-radius: 2px;");
        listPopUp.at(11)->setPopupText("Заполните поле!");
        listPopUp.at(11)->show(MainWindow::geometry().x() + ui->leFioParent->x() + ui->leFioParent->width() + 10, MainWindow::geometry().y() + ui->leFioParent->y() + 13);
        flag=true;
    }
    //Телефон доверенного лица
    if (ui->lePhoneParent->text().size() != 17) {
        ui->lePhoneParent->setStyleSheet("border: 1px solid red; min-height: 20px; border-radius: 2px;");
        listPopUp.at(12)->setPopupText("Неправильный формат номера телефона!");
        listPopUp.at(12)->show(MainWindow::geometry().x() + ui->lePhoneParent->x() + ui->lePhoneParent->width() + 10, MainWindow::geometry().y() + ui->lePhoneParent->y() + 13);
        flag=true;
    }

 //---------------------------------------------------------------------------------------------------
    //???? ???? ???? ?? ??????? ???? ??????????, ?? return
    if (flag == false) {
        QSqlQuery check;
        check.prepare("SELECT * FROM Clients WHERE Pasport=?");
        check.addBindValue(ui->lePasport->text());
        qDebug() << "Клиент найден: " << check.exec();
        if (check.next()) {
            QSqlQuery queryHistory;
            queryHistory.prepare("SELECT * FROM CreditHistory WHERE ID_CLIENT=?");
            queryHistory.addBindValue(check.value(0).toInt());
            qDebug() << "?????? ???????" << queryHistory.exec();
            if (queryHistory.next()) {
                //?????????? ????????
                int count = 0;
                //????? ??????????? ???????? ?? ???????? ????????
                double countMoney = 0;
                queryHistory.previous();
                //???????? ????? ???? ??????????? ???????? ?? ??????? ???????? ???????? ????????????,
                //? ????? ?????????? ???????? ????????
                while (queryHistory.next()) {
                    if (queryHistory.value(8).toString() == "Открыт") {
                        countMoney += queryHistory.value(5).toDouble();
                        count +=1;
                    }
                }
                //????????? ??????? ??????????? ??????
                countMoney += ui->leMoneyMonth->text().toDouble();
                //??????? ?? ????????, ??????? ??????? ????? ????????? ?? ?????? ????????
                double percentProfit = 0.35 * ui->leMoney->text().toDouble();
                //???? ????? ????? ??????????? ???????? ????????? ???? ???????, ?? ??????? ?????????????? ? ??????? ?? ???????
                if (countMoney > percentProfit) {
                    ui->leMoney->setStyleSheet("border: 1px solid red; min-height: 20px; border-radius: 2px;");
                    listPopUp.at(13)->setPopupText("Превышен лимит ежемесячного платежа!");
                    listPopUp.at(13)->show(MainWindow::geometry().x() + ui->leMoney->x() + ui->leMoney->width() + 10, MainWindow::geometry().y() + ui->leMoney->y() + 13);
                    return;
                }
                //???????? ?????????? ????????
                if (count >= 3){
                     QMessageBox::information(nullptr, QString("Предупреждение"), QString("Пользователь найден, но имеет 3 открытых кредита!"), QMessageBox::Ok);
                     return;
                } else if (count > 0) {
                    addCredit(check.value(0).toInt());
                    ui->tabWidget->setCurrentWidget(ui->tabMenu);
                }

           } else {
                addCredit(check.value(0).toInt());
                ui->tabWidget->setCurrentWidget(ui->tabMenu);
            }
       } else {
            QSqlQuery query;
            query.prepare("INSERT INTO Clients (FIO, Birthdate, HomeAddress, Pasport, MoneyMonth, Sex, HomePhone, Email, Expirience, WorkAddress, NameWork, WorkPhone, FioParent, PhoneParent) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
            query.addBindValue(ui->leFIO->text());
            query.addBindValue(ui->leBirthdate->text());
            query.addBindValue(ui->leHomeAddress->text());
            query.addBindValue(ui->lePasport->text());
            query.addBindValue(ui->leMoneyMonth->text());
            query.addBindValue(ui->cbSex->currentIndex());
            query.addBindValue(ui->leHomePhone->text());
            query.addBindValue(ui->leMail->text());
            query.addBindValue(ui->leExp->text());
            query.addBindValue(ui->leWorkAddress->text());
            query.addBindValue(ui->leNameWork->text());
            query.addBindValue(ui->leWorkPhone->text());
            query.addBindValue(ui->leFioParent->text());
            query.addBindValue(ui->lePhoneParent->text());
            if (query.exec()) {
                query.finish();
                qDebug() << "???????????? ????????!";
                QSqlQuery query1;
                query1.prepare("SELECT * FROM Clients WHERE Pasport=?");
                query1.addBindValue(ui->lePasport->text());
                query1.exec();
                qDebug() << "ID ???????????? ??????? ???????: " << query1.first();
                qDebug() << query1.value(0).toInt();
                addCredit(query1.value(0).toInt());
                ui->tabWidget->setCurrentWidget(ui->tabWidget);
            } else {
                qDebug() << "???????? ? ??????????? ????????????!";
            }
            qDebug() << query.lastError().text();
        }
        qDebug() << check.lastError().text();
    } else {
        return;
    }
}

//??????? ?????????? ??????? ? ??
void MainWindow::addCredit(int id)
{
    QSqlQuery queryCredit;
    queryCredit.prepare("INSERT INTO CreditHistory (ID_CLIENT, FIO, PasportNum, Credit, PayMonth, StartDate, EndDate, StatusCredit) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    queryCredit.addBindValue(id);
    queryCredit.addBindValue(ui->leFIO->text());
    queryCredit.addBindValue(ui->lePasport->text());
    queryCredit.addBindValue(ui->leMoney->text());
    queryCredit.addBindValue(ui->leMoneyMonth->text());
    queryCredit.addBindValue(QDate::currentDate().toString(Qt::LocalDate));
    queryCredit.addBindValue(QString());
    queryCredit.addBindValue("Открыт");
    qDebug() << "Кредит добавлен!" << queryCredit.exec();
    QMessageBox::information(nullptr, QString("Предупреждение"), QString("Кредит оформлен!"), QMessageBox::Ok);
}

void MainWindow::on_pbAcceptCredit_clicked()
{
    countCredit();
    ui->tabWidget->setCurrentWidget(ui->tabAddClient);
}

void MainWindow::on_pbCreditHistory_clicked()
{
    ui->tabWidget->setCurrentWidget(ui->tabCreditHistory);
    model->setFilter("");
    model->select();
    ui->leSearchPasport->clear();
}

void MainWindow::on_pbSave_clicked()
{

}

void MainWindow::on_pbDelete_clicked()
{
    if (ui->tvAdmin->currentIndex().isValid()) {
        int row = ui->tvAdmin->currentIndex().row();
        model->removeRow(row);
        model->submitAll();
        model->select();
    }
}

void MainWindow::on_pbBackCreditHistory_clicked()
{
    ui->tabWidget->setCurrentWidget(ui->tabMenu);
    model->submitAll();
}

void MainWindow::on_pbExit_clicked()
{
    ui->tabWidget->setCurrentWidget(ui->tabAuthorization);
    ui->leLogin->clear();
    ui->lePassword->clear();
}

void MainWindow::on_pbStartSearch_clicked()
{
    if (ui->leSearchPasport->text().size() == 11) {
        model->setFilter("PasportNum='" + QString(ui->leSearchPasport->text()) + "'");
        model->select();
    } else {
        QMessageBox::information(nullptr, QString("Предупреждение"), QString("Заполните поле!"), QMessageBox::Ok);
        model->setFilter("");
    }

}

void MainWindow::on_pbCancelCredit_clicked()
{
    ui->tabWidget->setCurrentWidget(ui->tabMenu);
}

void MainWindow::moveEvent(QMoveEvent *e){
    listPopUp.at(0)->move(MainWindow::geometry().x() + ui->leFIO->x() + ui->leFIO->width() + 10, MainWindow::geometry().y() + ui->leFIO->y() + 13);
    listPopUp.at(1)->move(MainWindow::geometry().x() + ui->leBirthdate->x() + ui->leFIO->width() + 10, MainWindow::geometry().y() + ui->leBirthdate->y() + 13);
    listPopUp.at(2)->move(MainWindow::geometry().x() + ui->leHomeAddress->x() + ui->leHomeAddress->width() + 10, MainWindow::geometry().y() + ui->leHomeAddress->y() + 13);
    listPopUp.at(3)->move(MainWindow::geometry().x() + ui->lePasport->x() + ui->lePasport->width() + 10, MainWindow::geometry().y() + ui->lePasport->y() + 13);
    listPopUp.at(4)->move(MainWindow::geometry().x() + ui->leMoney->x() + ui->leMoney->width() + 10, MainWindow::geometry().y() + ui->leMoney->y() + 13);
    listPopUp.at(5)->move(MainWindow::geometry().x() + ui->leHomePhone->x() + ui->leHomePhone->width() + 10, MainWindow::geometry().y() + ui->leHomePhone->y() + 13);
    listPopUp.at(6)->move(MainWindow::geometry().x() + ui->leExp->x() + ui->leExp->width() + 10, MainWindow::geometry().y() + ui->leExp->y() + 13);
    listPopUp.at(7)->move(MainWindow::geometry().x() + ui->leMail->x() + ui->leMail->width() + 10, MainWindow::geometry().y() + ui->leMail->y() + 13);
    listPopUp.at(8)->move(MainWindow::geometry().x() + ui->leWorkAddress->x() + ui->leWorkAddress->width() + 10, MainWindow::geometry().y() + ui->leWorkAddress->y() + 13);
    listPopUp.at(9)->move(MainWindow::geometry().x() + ui->leNameWork->x() + ui->leNameWork->width() + 10, MainWindow::geometry().y() + ui->leNameWork->y() + 13);
    listPopUp.at(10)->move(MainWindow::geometry().x() + ui->leWorkPhone->x() + ui->leWorkPhone->width() + 10, MainWindow::geometry().y() + ui->leWorkPhone->y() + 13);
    listPopUp.at(11)->move(MainWindow::geometry().x() + ui->leFioParent->x() + ui->leFioParent->width() + 10, MainWindow::geometry().y() + ui->leFioParent->y() + 13);
    listPopUp.at(12)->move(MainWindow::geometry().x() + ui->lePhoneParent->x() + ui->lePhoneParent->width() + 10, MainWindow::geometry().y() + ui->lePhoneParent->y() + 13);
    QWidget::moveEvent(e);
}

void MainWindow::on_leBirthdate_textChanged(const QString &arg1)
{
    //???? ?????? ????????? ? ??????? 1 ??? 4, ?? ??????????? ???? ? ??????????????? ?????????,
    //????? ??????????? ????????? ???????????
    if (ui->leBirthdate->cursorPosition() == 1) {
        flagDot=false;
    } else if (ui->leBirthdate->cursorPosition() == 4) {
        flagDot=true;
    }
    //????????? ??????? ????? ?????? ?????
    ui->leBirthdate->setStyleSheet("");
    //???? ?????? ?????? 2 ??? 5, ? ???? ? ????? ?????????, ?? ?????? ??????????????? ???????????
    if ((ui->leBirthdate->text().size() == 2) && (flagDot==false)) {
        qDebug() << "????????!";
        ui->leBirthdate->setText(ui->leBirthdate->text() + ".");
        flagDot = true;
    } else if ((ui->leBirthdate->text().size() == 5) && (flagDot==true)) {
        ui->leBirthdate->setText(ui->leBirthdate->text() + ".");
        flagDot=false;
    }

}

void MainWindow::on_pbAdminExit_clicked()
{
    on_pbExit_clicked();
    model->submitAll();
}

void MainWindow::on_pbAddOperator_clicked()
{
    DialogAddOperator *form = new DialogAddOperator();
    form->setModal(true);
    form->show();
}

void MainWindow::on_pbDeleteOperator_clicked()
{
    if (ui->tvListOperator->currentIndex().isValid()) {
        int row = ui->tvListOperator->currentIndex().row();
        modelOperator->removeRow(row);
        modelOperator->submitAll();
        modelOperator->select();
    }
}

void MainWindow::countCredit()
{
    QDate date = QDate::currentDate();
    qDebug() << date;
    date = date.addMonths(ui->cbMonth->currentText().toInt());
    qDebug() << date;
    qDebug() << QDate::currentDate().daysTo(date);
    if (ui->leFirstPay->text().isEmpty()) {
        double trueSall = ui->leShowMoney->text().toDouble() * 0.13 *  QDate::currentDate().daysTo(date) /(QDate::currentDate().daysTo(date) / (ui->cbMonth->currentText().toDouble()/12));
        double trueS = trueSall / ui->cbMonth->currentText().toInt();
        double s = (ui->leShowMoney->text().toDouble() / ui->cbMonth->currentText().toDouble()) + trueS;
        ui->leMoneyMonth->setText(QString::number(s,'f',2));
        ui->leOverpaymentMonth->setText(QString::number(trueS,'f',2));
        ui->leOverpaymentAll->setText(QString::number(trueSall,'f',2));
    } else {
        //prev = ui->leShowMoney->text().toDouble() - ui->leFirstPay->text().toDouble();
        double trueSall = prev * 0.13 *  QDate::currentDate().daysTo(date) /(QDate::currentDate().daysTo(date) / (ui->cbMonth->currentText().toDouble()/12));
        double trueS = trueSall / ui->cbMonth->currentText().toInt();
        double s = (prev / ui->cbMonth->currentText().toDouble()) + trueS;
        ui->leMoneyMonth->setText(QString::number(s,'f',2));
        ui->leOverpaymentMonth->setText(QString::number(trueS,'f',2));
        ui->leOverpaymentAll->setText(QString::number(trueSall,'f',2));
    }
}

void MainWindow::on_hsCountMoney_valueChanged(int value)
{
    ui->leShowMoney->setText(QString::number(value));
    countCredit();
}

void MainWindow::on_leShowMoney_editingFinished()
{
    ui->hsCountMoney->setValue(ui->leShowMoney->text().toInt());
    countCredit();
}

void MainWindow::on_cbMonth_currentIndexChanged(const QString &arg1)
{
    countCredit();
}

void MainWindow::on_leFirstPay_editingFinished()
{
    if (ui->leFirstPay->text().toDouble() > ui->leShowMoney->text().toDouble())
        ui->leFirstPay->setText(ui->leShowMoney->text());
    countCredit();
}

void MainWindow::on_leShowMoney_textChanged(const QString &arg1)
{
    if (ui->leShowMoney->text().toInt() > 1000000)
        ui->leShowMoney->setText("1000000");
}

void MainWindow::on_leMoneyMonth_editingFinished()
{
    QDate date = QDate::currentDate();
    qDebug() << date;
    date = date.addMonths(ui->cbMonth->currentText().toInt());
    qDebug() << date;
    qDebug() << QDate::currentDate().daysTo(date);

    qDebug() << ui->leMoneyMonth->text().toDouble();
    if (ui->leMoneyMonth->text().toDouble() < 1324.93)
        ui->leMoneyMonth->setText("1324.93");
        double showFalse = ui->leMoneyMonth->text().toDouble() * ui->cbMonth->currentText().toInt();
        double showTrue = showFalse - ui->leOverpaymentAll->text().toDouble() + ui->leFirstPay->text().toDouble();
        ui->leShowMoney->setText(QString::number(showTrue, 'f', 0));
        countCredit();

}

void MainWindow::on_leFirstPay_textChanged(const QString &arg1)
{
    prev = ui->leShowMoney->text().toDouble() - ui->leFirstPay->text().toDouble();

}

void MainWindow::on_leFIO_textChanged(const QString &arg1)
{
    ui->leFIO->setStyleSheet("");
}

void MainWindow::on_leHomeAddress_textChanged(const QString &arg1)
{
    ui->leHomeAddress->setStyleSheet("");
}

void MainWindow::on_lePasport_textChanged(const QString &arg1)
{
    ui->lePasport->setStyleSheet("");
}

void MainWindow::on_leMoney_textChanged(const QString &arg1)
{
    ui->leMoney->setStyleSheet("");
}

void MainWindow::on_leHomePhone_textChanged(const QString &arg1)
{
    ui->leHomePhone->setStyleSheet("");
}

void MainWindow::on_leMail_textChanged(const QString &arg1)
{
    ui->leMail->setStyleSheet("");
}

void MainWindow::on_leExp_textChanged(const QString &arg1)
{
    ui->leExp->setStyleSheet("");
}

void MainWindow::on_leWorkAddress_textChanged(const QString &arg1)
{
    ui->leWorkAddress->setStyleSheet("");
}

void MainWindow::on_leNameWork_textChanged(const QString &arg1)
{
    ui->leNameWork->setStyleSheet("");
}

void MainWindow::on_leWorkPhone_textChanged(const QString &arg1)
{
    ui->leWorkPhone->setStyleSheet("");
}

void MainWindow::on_leFioParent_textChanged(const QString &arg1)
{
    ui->leFioParent->setStyleSheet("");
}

void MainWindow::on_lePhoneParent_textChanged(const QString &arg1)
{
    ui->lePhoneParent->setStyleSheet("");
}

void MainWindow::on_leMoneyMonth_textChanged(const QString &arg1)
{
    prev = ui->leShowMoney->text().toDouble() - ui->leFirstPay->text().toDouble();
}

void MainWindow::on_cbMonth_currentTextChanged(const QString &arg1)
{
    countCredit();
}

void MainWindow::on_pbPrint_clicked()
{
    model->submitAll();

    const int rowCount =  model->rowCount();
    const int columnCount = model->columnCount();

    if (rowCount == 0) {
        QMessageBox::information(nullptr, QString("Приветствие"), QString("Таблица пустая!"), QMessageBox::Ok);
        return;
    }

    QString strStream;
    QTextStream out(&strStream);

    QString strTitle = "Кредитная история";

    out <<  "<html>\n"
        "<head>\n"
        "<meta Content=\"Text/html; charset=Windows-1251\">\n"
        <<  QString("<title>%1</title>\n").arg(strTitle)
        <<  "</head>\n"
        "<body bgcolor=#ffffff link=#5000A0>\n"
        "<table border=1 cellspacing=0 cellpadding=2 width='100%'>\n";

    // headers
    out << "<thead><tr bgcolor=#f0f0f0>";
    for (int column = 0; column < columnCount; column++) {
        if (column == 1)
            continue;
        if (!ui->tvCreditHistory->isColumnHidden(column))
            out << QString("<th>%1</th>").arg(model->headerData(column, Qt::Horizontal, Qt::DisplayRole ).toString());
    }
    out << "</tr></thead>\n";

    // data table
    for (int row = 0; row < rowCount; row++) {
        out << "<tr>";
        for (int column = 0; column < columnCount; column++) {
            if (column == 1)
                continue;
            if (!ui->tvCreditHistory->isColumnHidden(column)) {
                QString data = model->data(model->index(row,column), Qt::DisplayRole).toString();
                out << QString("<td bkcolor=0>%1</td>").arg((!data.isEmpty()) ? data : QString("&nbsp;"));
            }
        }
        out << "</tr>\n";
    }
    out <<  "</table>\n"
        "</body>\n"
        "</html>\n";

    QTextDocument *document = new QTextDocument();
    document->setHtml(strStream);

    QPrinter printer;

    QPrintDialog *dialog = new QPrintDialog(&printer, nullptr);
    if (dialog->exec() == QDialog::Accepted) {
        document->print(&printer);
    }

    delete document;
}
