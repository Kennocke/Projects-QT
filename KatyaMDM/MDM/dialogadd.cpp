#include "dialogadd.h"
#include "ui_dialogadd.h"
#include "mainwindow.h"

#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QSql>
#include <QDate>
#include <QSqlError>

DialogAdd::DialogAdd(QWidget *parent, QString login) :
    QDialog(parent),
    ui(new Ui::DialogAdd)
{
    ui->setupUi(this);
    addCurrentLogin = login;
    QRegularExpression rx("^[а-яА-ЯёЁa-zA-Z]+$");
    QRegularExpression rxDate("^(0[1-9]|[12][0-9]|3[01])[- /.](0[1-9]|1[012])[- /.](19|20)[0-9][0-9]$");
    QRegularExpression rxMoney("^(-?)(0|([1-9][0-9]*))(\\.[0-9][0-9]?)?$");
    QValidator *validatorDate = new QRegularExpressionValidator(rxDate, this);
    QValidator *validator = new QRegularExpressionValidator(rx, this);
    QValidator *validatorMoney = new QRegularExpressionValidator(rxMoney, this);
    //доступны только буквы
    ui->leName->setValidator(validator);
    //доступны только цифры
    ui->leCount->setValidator( new QIntValidator(1, 16777215, this));
    //ui->lePrice->setValidator( new QIntValidator(1, 16777215, this));
    ui->lePrice->setValidator(validatorMoney);
    ui->lePriceAll->setValidator( new QIntValidator(1, 16777215, this));
    ui->leLifetime->setValidator( new QIntValidator(1, 150, this));
    //запрещаем редактирование итоговой сумммы. Теперь она рассчитывается автоматически!
    ui->lePriceAll->setReadOnly(true);

}

DialogAdd::~DialogAdd()
{
    delete ui;
}

void DialogAdd::on_buttonBox_accepted()
{
    if ((ui->leName->text() != "") && (ui->leCount->text() != "") && (ui->lePrice->text() != "") && (ui->lePriceAll->text() != "") && (ui->leLifetime->text() != "")) {
        //запрос на добавление записи в БД
        QSqlQuery query;
        query.prepare("INSERT INTO Capital (Login, DateCreate, Name, CountUnit, PriceUnit, PriceAll, Article, Lifetime) VALUES(?, ?, ?, ?, ?, ?, ?, ?)");
        query.bindValue(0, addCurrentLogin);
        query.bindValue(1, QDate::currentDate().toString(Qt::ISODate));
        query.bindValue(2, ui->leName->text());
        query.bindValue(3, ui->leCount->text());
        query.bindValue(4, ui->lePrice->text());
        query.bindValue(5, ui->lePriceAll->text());
        query.bindValue(6, ui->cbArticle->currentText());
        query.bindValue(7, ui->leLifetime->text().toInt());

        if (query.exec()) {
            qDebug() << "Запись добавлена!";
            //вызов сигнала для активации метода обновления модели
            emit transmitter();
        } else {
            qDebug() << query.lastError().text();
        }
    } else {
        QMessageBox::information(0, QString("Message"), QString("Заполните все необходимые поля!"), QMessageBox::Ok);
        this->show();
    }
}

void DialogAdd::on_leCount_editingFinished()
{
    //если поля кол-во и цена за ед. не равны "" и 0
    if ((ui->leCount->text() != "") && (ui->lePrice->text() != "") && (ui->leCount->text().toDouble() != 0) && (ui->lePrice->text().toDouble() != 0))
    {
        //считаем общую сумму
        double priceAll = ui->leCount->text().toDouble() * ui->lePrice->text().toDouble();
        //выводим результат в виджет
        ui->lePriceAll->setText(QString::number(priceAll,'f',2));
    }
}

void DialogAdd::on_lePrice_editingFinished()
{
    //если поля кол-во и цена за ед. не равны "" и 0
    if ((ui->leCount->text() != "") && (ui->lePrice->text() != "") && (ui->leCount->text().toDouble() != 0) && (ui->lePrice->text().toDouble() != 0))
    {
        //считаем общую сумму
        double priceAll = ui->leCount->text().toDouble() * ui->lePrice->text().toDouble();
        //выводим результат в виджет
        ui->lePriceAll->setText(QString::number(priceAll,'f',2));
    }
}

//при изменении текста проверяем на наличие нулей и если ноль, то запрещаем ввод
void DialogAdd::on_leCount_textEdited(const QString &arg1)
{
    if(arg1=="0")
          ui->leCount->setText(QString());
}

//устанавливаем максимальное кол-во символов
void DialogAdd::on_lePrice_textEdited(const QString &arg1)
{
    if((arg1.size() == 14)) {
          ui->lePrice->setText("99999999999.99");
    }
}

//при изменении текста проверяем на наличие нулей и если ноль, то запрещаем ввод
void DialogAdd::on_leLifetime_textEdited(const QString &arg1)
{
    if(arg1=="0")
          ui->leLifetime->setText(QString());
}
