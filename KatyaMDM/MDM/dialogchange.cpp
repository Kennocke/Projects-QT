#include "dialogchange.h"
#include "ui_dialogchange.h"

#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QMessageBox>
#include <QDate>

DialogChange::DialogChange(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogChange)
{
    ui->setupUi(this);

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
    ui->lePrice->setValidator(validatorMoney);
    ui->lePriceAll->setValidator( new QIntValidator(1, 16777215, this));
    ui->leLifetime->setValidator( new QIntValidator(1, 150, this));

    //запрещаем редактирование итоговой сумммы. Теперь она рассчитывается автоматически!
    ui->lePriceAll->setReadOnly(true);
}

DialogChange::~DialogChange()
{
    delete ui;
}

void DialogChange::setDB(QSqlTableModel *model, int index)
{
    indexGlobal = index;
    modelGlobal = model;
    //присваиваем поля записи виджетам на форме
    ui->leName->setText(model->index(index,3).data().toString());
    ui->leCount->setText(model->index(index,4).data().toString());
    ui->lePrice->setText(model->index(index,5).data().toString());
    ui->lePriceAll->setText(model->index(index,6).data().toString());
    if (model->index(index,7).data().toString() == "НМА")
        ui->cbArticle->setCurrentIndex(0);
    else
        ui->cbArticle->setCurrentIndex(1);
    ui->leLifetime->setText(model->index(index,8).data().toString());
}

void DialogChange::on_buttonBox_accepted()
{
    if ((ui->leName->text() != "") && (ui->leCount->text() != "") && (ui->lePrice->text() != "") && (ui->lePriceAll->text() != "") && (ui->leLifetime->text() != "")) {
        //объявляем переменную для SQL запроса
        QSqlQuery query;
        //шаблон запроса на обновление записи
        query.prepare("UPDATE Capital SET DateCreate = ?, Name = ?, CountUnit = ?, PriceUnit = ?, PriceAll = ?, Article = ?, Lifetime = ? WHERE ID = ?");
        query.addBindValue(QDate::currentDate().toString(Qt::ISODate));
        query.addBindValue(ui->leName->text());
        query.addBindValue(ui->leCount->text());
        query.addBindValue(ui->lePrice->text());
        query.addBindValue(ui->lePriceAll->text());
        query.addBindValue(ui->cbArticle->currentText());
        query.addBindValue(ui->leLifetime->text());
        query.addBindValue(modelGlobal->index(indexGlobal, 0).data());
        //выполняем запрос
        query.exec();
        //обновление модели после изменения записи
        modelGlobal->select();
        //вызов сигнала для активации метода обновления модели
        emit transmitter();
    } else {
        QMessageBox::information(0, QString("Message"), QString("Заполните все необходимые поля!"), QMessageBox::Ok);
        this->show();
    }
}

void DialogChange::on_leCount_editingFinished()
{
    //если поля кол-во и цена за ед. не равны "" и 0
    if ((ui->leCount->text() != "") && (ui->lePrice->text() != "") && (ui->leCount->text().toDouble() != 0) && (ui->lePrice->text().toDouble() != 0))
    {
        //считаем общую сумму
        double priceAll = ui->leCount->text().toDouble() * ui->lePrice->text().toDouble();
        //выводим результат в виджет
        ui->lePriceAll->setText(QString::number(priceAll));
    }
}

void DialogChange::on_lePrice_editingFinished()
{
    //если поля кол-во и цена за ед. не равны "" и 0
    if ((ui->leCount->text() != "") && (ui->lePrice->text() != "") && (ui->leCount->text().toDouble() != 0) && (ui->lePrice->text().toDouble() != 0))
    {
        //считаем общую сумму
        double priceAll = ui->leCount->text().toDouble() * ui->lePrice->text().toDouble();
        //выводим результат в виджет
        ui->lePriceAll->setText(QString::number(priceAll));
    }
}

//при изменении текста проверяем на наличие нулей и если ноль, то запрещаем ввод
void DialogChange::on_leCount_textEdited(const QString &arg1)
{
    if(arg1=="0")
          ui->leCount->setText(QString());
}

//устанавливаем максимальное кол-во символов
void DialogChange::on_lePrice_textEdited(const QString &arg1)
{
    if((arg1.size() == 14)) {
          ui->lePrice->setText("99999999999.99");
    }
}

//при изменении текста проверяем на наличие нулей и если ноль, то запрещаем ввод
void DialogChange::on_leLifetime_textEdited(const QString &arg1)
{
    if(arg1=="0")
          ui->leLifetime->setText(QString());
}
