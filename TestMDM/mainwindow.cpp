#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTabBar>
#include <QDebug>
#include <QDir>
#include <QSqlQuery>
#include <QPixmap>
#include <QRadioButton>
#include <QSqlError>
//подключаем для вспылающихся окон
#include <QMessageBox>
#include <QBuffer>
#include <QTimer>
#include <QTime>
#include <QException>
#include <QFile>
#include <QIODevice>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Скрываем TabBar у QTabWidget
    ui->tabWidget->tabBar()->hide();

    //создаём базу данных
    db = QSqlDatabase::addDatabase("QSQLITE");
    //указываем путь к базе данных
    db.setDatabaseName(QDir::toNativeSeparators(QApplication::applicationDirPath()) + "/database.db3");
    //устанавливаем соединение с БД
    if(db.open())
        qDebug() << "Подключение к бд выполненно успешно!";

    //установка валидатора для вкладки "Меню"
    ui->leName->setValidator(new QRegularExpressionValidator(QRegularExpression("^[а-яA-ЯёЁ .]+$")));

    //установка валидатора для вкладки "Добавить тест"
    ui->leTypeQuestion->setValidator(new QRegularExpressionValidator(QRegularExpression("^[а-яA-ЯёЁ ]+$")));

    //заполняем списки
    insertCB();

    //создание таймера
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerAlarm()));

    //создание модели
    model= new QSqlTableModel(this, db);
    model->setTable("Result");
    model->setHeaderData(1, Qt::Horizontal, "ФИО");
    model->setHeaderData(2, Qt::Horizontal, "Тема");
    model->setHeaderData(3, Qt::Horizontal, "Правильные ответы");
    model->setHeaderData(4, Qt::Horizontal, "Время прохождения");
    ui->tvResult->setModel(model);
    ui->tvResult->hideColumn(0);
    ui->tvResult->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tvResult->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tvResult->resizeRowsToContents();
    model->select();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//функция заполнения выпадающих списков
void MainWindow::insertCB()
{
    //заполнение ComboBox с темами
    ui->cbSelectTheme->clear();
    ui->cbThemeStart->clear();
    ui->cbTypeQuestion->clear();
    QSqlQuery query("SELECT * FROM QuestionType");
    if (query.exec()) {
        qDebug() << "Всё отлично";
        for (int i=0; query.next(); i++) {
            ui->cbSelectTheme->addItem(query.value(1).toString());
            ui->cbThemeStart->addItem(query.value(1).toString());
            ui->cbTypeQuestion->addItem(query.value(1).toString());
        }
    } else {
        qDebug() << "Всё плохо!";
        qDebug() << query.lastError().text();
    }
}

void MainWindow::on_pbStart_clicked()
{
    if (ui->leName->text().isEmpty()) {
        QMessageBox::information(nullptr, QString("Message"), QString("Введите ваше ФИО!"), QMessageBox::Ok);
    } else {
        ui->lbTimer->setText("00:00:00");
        //создание теста с графами
        createTest(ui->cbThemeStart->currentText());
        qDebug() << ui->cbThemeStart->currentIndex();
    }

}

void MainWindow::on_pbCommon_clicked()
{


}

void MainWindow::createTest(QString index)
{
    //флаг отвечающий за задание первого варианта ответа
    bool flag = true;
    //запрос "Выборка всех полей из таблицы по типу вопроса"
    QSqlQuery query;
    query.prepare("SELECT * FROM QuestionImage WHERE TypeQuestion=?");
    query.addBindValue(index);
    qDebug() <<  query.exec();
    if (query.last() != 0) {
        qDebug() << "Тестирование началось!";
        query.first();
        query.previous();
        ui->tabWidget->setCurrentIndex(3);
    } else {
        QMessageBox::information(nullptr, QString("Message"), QString("Добаьте хоть один тест по выбранной теме!"), QMessageBox::Ok);
        return;
    }
    qDebug() << query.lastError().text();

    for (int i=0; query.next(); i++) {
        if (flag) {
            trueAnswer = query.value(7).toInt();
            flag = false;
        }
        QPixmap outPixmap = QPixmap();
        outPixmap.loadFromData(query.value(2).toByteArray());
        listQuestion << outPixmap;

        QStringList list;
        for (int j=3; j<8; j++)
            list << query.value(j).toString();
        mapAnswer.insert(i, list);
        sizeTest++;
    }
    ui->lbQuestion->setPixmap(listQuestion.at(currentAnswer));
    QStringList list(mapAnswer.value(currentAnswer));

    for (int j=1; j < ui->gbAnswer->children().size(); j++) {
        QRadioButton *rbt = qobject_cast<QRadioButton *>(ui->gbAnswer->children().value(j));
        rbt->setText(list.at(j-1));
    }

    //увеличиваем показатель текущего вопроса
    currentAnswer++;
    qDebug() << "Размер теста: " << sizeTest;

    time.setHMS(0,0,0);
    timer->start(1000); // И запустим таймер
}

/* Слот для обработки timeout() таймера
 * */
void MainWindow::slotTimerAlarm()
{
    /* Ежесекундно обновляем данные по текущему времени
     * Перезапускать таймер не требуется
     * */
    time = time.addSecs(1);
    ui->lbTimer->setText(time.toString("hh:mm:ss"));
}

void MainWindow::nextQuestion()
{
    qDebug() << trueAnswer;

    if ((ui->rb1->isChecked()) || (ui->rb2->isChecked()) || (ui->rb3->isChecked()) || (ui->rb4->isChecked())) {
        for (int j=1; j < ui->gbAnswer->children().size(); j++) {
            QRadioButton *rbt = qobject_cast<QRadioButton *>(ui->gbAnswer->children().value(j));
            if ((rbt->isChecked()) && (trueAnswer == j))
                countTrueAnswer++;
        }

        if (sizeTest == currentAnswer) {
            ui->lbFinalFIO->setText("Поздравляю " + ui->leName->text());
            ui->lbCountAnswer->setText(QString::number(countTrueAnswer) + " \\ " + QString::number(sizeTest));
            ui->lbFinalTime->setText(time.toString("hh:mm:ss"));
            timer->stop();

            QSqlQuery queryAdd;
            queryAdd.prepare("INSERT INTO Result (FIO, Theme, TrueAnswer, Time) VALUES (?, ?, ?, ?)");
            queryAdd.addBindValue(ui->leName->text());
            queryAdd.addBindValue(ui->cbThemeStart->currentText());
            queryAdd.addBindValue(ui->lbCountAnswer->text());
            queryAdd.addBindValue(ui->lbFinalTime->text());
            qDebug() << "Добавление результата прошло " << queryAdd.exec();

            ui->tabWidget->setCurrentIndex(4);
            return;
        }

        ui->lbQuestion->setPixmap(listQuestion.at(currentAnswer));
        QStringList list(mapAnswer.value(currentAnswer));

        for (int j=1; j < ui->gbAnswer->children().size(); j++) {
            QRadioButton *rbt = qobject_cast<QRadioButton *>(ui->gbAnswer->children().value(j));
            rbt->setText(list.at(j-1));
            rbt->setAutoExclusive(false);
            rbt->setChecked(false);
            rbt->setAutoExclusive(true);
        }
        //обновляем правильный ответ
        trueAnswer = mapAnswer.value(currentAnswer).last().toInt();

        qDebug() << "Текущий вопрос " << currentAnswer;

        //увеличиваем показатель текущего вопроса
        currentAnswer++;
    } else {
        QMessageBox::information(nullptr, QString("Message"), QString("Выберите вариант ответа!"), QMessageBox::Ok);
    }
}

void MainWindow::on_pbAnswer_clicked()
{
    nextQuestion();
}

void MainWindow::on_pbAddTest_clicked()
{
    //сброс radiobutton
    ui->rbAdd1->setAutoExclusive(false);
    ui->rbAdd1->setChecked(false);
    ui->rbAdd1->setAutoExclusive(true);

    ui->rbAdd2->setAutoExclusive(false);
    ui->rbAdd2->setChecked(false);
    ui->rbAdd2->setAutoExclusive(true);

    ui->rbAdd3->setAutoExclusive(false);
    ui->rbAdd3->setChecked(false);
    ui->rbAdd3->setAutoExclusive(true);

    ui->rbAdd4->setAutoExclusive(false);
    ui->rbAdd4->setChecked(false);
    ui->rbAdd4->setAutoExclusive(true);

    ui->lePath->setReadOnly(true);
    ui->lePath->clear();
    ui->leAdd1->clear();
    ui->leAdd2->clear();
    ui->leAdd3->clear();
    ui->leAdd4->clear();
    ui->lbSelectQuestion->clear();
    ui->leTypeQuestion->clear();
    ui->tabWidget->setCurrentWidget(ui->tabAddTest);
}

void MainWindow::on_pbAddPath_clicked()
{
    try {
        ui->lePath->setText(dialog.getOpenFileName(nullptr, "Выберите изображение с вопросом", "", tr("Image Files (*.png *.jpg)")));
        QPixmap img = QPixmap();
        img.load(ui->lePath->text());
        ui->lbSelectQuestion->setPixmap(img.scaled(400,300));
    } catch (QException &e) {
        QMessageBox::information(nullptr, QString("Ошибка"), QString("Критическая ошибка!"), QMessageBox::Ok);
    }
}

void MainWindow::on_pbAddQuestion_clicked()
{
    if ((ui->lePath->text() != "") &&
            (ui->leAdd1->text() != "") && (ui->leAdd2->text() != "") &&
            (ui->leAdd3->text() != "") && (ui->leAdd4->text() != ""))
    {
        if  ((ui->rbAdd1->isChecked()) || (ui->rbAdd2->isChecked()) || (ui->rbAdd3->isChecked()) || (ui->rbAdd4->isChecked())) {
            QSqlQuery query;
            query.prepare("INSERT INTO QuestionImage (TypeQuestion, Question, Answer1, Answer2, Answer3, Answer4, AnswerTrue) VALUES(?, ?, ?, ?, ?, ?, ?)");
            if (ui->leTypeQuestion->text().isEmpty())
                query.addBindValue(ui->cbTypeQuestion->currentText());
            else {
                QSqlQuery zapr;
                zapr.prepare("SELECT * FROM QuestionImage WHERE TypeQuestion=?");
                zapr.addBindValue(ui->leTypeQuestion->text());
                qDebug() << zapr.exec();
                if (zapr.first()) {
                    QMessageBox::information(nullptr, QString("Ошибка"), QString("Такая тема уже существует!"), QMessageBox::Ok);
                    ui->leTypeQuestion->clear();
                    return;
                } else {
                    query.addBindValue(ui->leTypeQuestion->text());
                    QSqlQuery add("INSERT INTO QuestionType (TypeName) VALUES (?)");
                    add.addBindValue(ui->leTypeQuestion->text());
                    if (add.exec()) {
                        qDebug() << "Сработало";
                    } else {
                        qDebug() << "Не сработало";
                    }
                    insertCB();
                }
            }
            QByteArray inByteArray;
            QBuffer inBuffer(&inByteArray);
            inBuffer.open(QIODevice::WriteOnly);
            ui->lbSelectQuestion->pixmap()->save(&inBuffer, "PNG");
            query.addBindValue(inByteArray);
            query.addBindValue(ui->leAdd1->text());
            query.addBindValue(ui->leAdd2->text());
            query.addBindValue(ui->leAdd3->text());
            query.addBindValue(ui->leAdd4->text());

            if (ui->rbAdd1->isChecked()){
                query.addBindValue(1);
                ui->rbAdd1->setAutoExclusive(false);
                ui->rbAdd1->setChecked(false);
                ui->rbAdd1->setAutoExclusive(true);
            } else if (ui->rbAdd2->isChecked()) {
                query.addBindValue(2);
                ui->rbAdd2->setAutoExclusive(false);
                ui->rbAdd2->setChecked(false);
                ui->rbAdd2->setAutoExclusive(true);
            } else if (ui->rbAdd3->isChecked()) {
                query.addBindValue(3);
                ui->rbAdd3->setAutoExclusive(false);
                ui->rbAdd3->setChecked(false);
                ui->rbAdd3->setAutoExclusive(true);
            } else if (ui->rbAdd4->isChecked()) {
                query.addBindValue(4);
                ui->rbAdd4->setAutoExclusive(false);
                ui->rbAdd4->setChecked(false);
                ui->rbAdd4->setAutoExclusive(true);
            }
            qDebug() << "Вопрос добавлен: " << query.exec();
            qDebug() << query.lastError().text();
            ui->tabWidget->setCurrentWidget(ui->tabAdmin);
        } else {
            QMessageBox::information(nullptr, QString("Message"), QString("Заполните все поля!"), QMessageBox::Ok);
        }
     } else {
            QMessageBox::information(nullptr, QString("Message"), QString("Заполните все поля!"), QMessageBox::Ok);
     }
}

void MainWindow::on_pushButton_clicked()
{
    ui->tabWidget->setCurrentWidget(ui->tabMenu);
    sizeTest = 0;
    ui->lbTimer->setText("00:00:00");
    currentAnswer = 0;
    countTrueAnswer = 0;
    time.setHMS(0,0,0);
    mapAnswer.clear();
    listQuestion.clear();
    ui->lbFinalFIO->clear();
    ui->leName->clear();

    for (int j=1; j < ui->gbAnswer->children().size(); j++) {
        QRadioButton *rbt = qobject_cast<QRadioButton *>(ui->gbAnswer->children().value(j));
        rbt->setAutoExclusive(false);
        rbt->setChecked(false);
        rbt->setAutoExclusive(true);
    }
}

void MainWindow::on_pbExit_clicked()
{
    ui->leLogin->clear();
    ui->lePassword->clear();
    ui->tabWidget->setCurrentWidget(ui->tabLogin);
}

void MainWindow::on_pbBackTheory_clicked()
{
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::on_pbExtraExit_clicked()
{
    on_pushButton_clicked();
}

void MainWindow::on_pbTheory_clicked()
{
    QFile file(QDir::toNativeSeparators(QApplication::applicationDirPath()) + "/text/" + ui->cbThemeStart->currentText() + ".txt");
    if(file.exists()) {
        if (!file.open(QIODevice::ReadOnly))
            qDebug() << "Ошибка!";
        ui->lbTheory->setText(file.readAll());
        ui->tabWidget->setCurrentWidget(ui->tabTheory);
    } else {
        QMessageBox::information(nullptr, QString("Предупреждение"), QString("Нет теории по этой теме!"), QMessageBox::Ok);
    }
    file.close();
}

void MainWindow::on_pbCancel_clicked()
{
    ui->tabWidget->setCurrentWidget(ui->tabAdmin);
}

void MainWindow::on_pbAddTheory_clicked()
{
    QFileDialog dialogTheory;
    QFile file(dialogTheory.getOpenFileName(nullptr, "Выберите текст с теорией", "", tr("Text Files (*.txt)")));
    if (file.exists()) {
        QMessageBox::information(nullptr, QString("Предупреждение"), QString("Теория по выбранной теме уже существует!"), QMessageBox::Ok);
    } else {
        qDebug() << file.copy(QDir::toNativeSeparators(QApplication::applicationDirPath()) + "/text/" + ui->cbSelectTheme->currentText() + ".txt");
    }
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
                    QMessageBox::information(nullptr, QString("Приветствие"), QString("Добро пожаловать " + query.value(3).toString()), QMessageBox::Ok);
                    ui->tabWidget->setCurrentWidget(ui->tabAdmin);
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

void MainWindow::on_pbExitAdmin_clicked()
{
    ui->leLogin->clear();
    ui->lePassword->clear();
    ui->tabWidget->setCurrentWidget(ui->tabLogin);
}

void MainWindow::on_pbDeleteTheme_clicked()
{
    QSqlQuery query;
    query.prepare("DELETE FROM QuestionImage WHERE TypeQuestion=?");
    query.addBindValue(ui->cbSelectTheme->currentText());
    qDebug() << "Удаление из таблицы с вопросами прошло " << query.exec();

    query.clear();
    query.prepare("DELETE FROM QuestionType WHERE TypeName=?");
    query.addBindValue(ui->cbSelectTheme->currentText());
    qDebug() << "Удаление из таблицы с типами прошло " << query.exec();

    QFile file(QDir::toNativeSeparators(QApplication::applicationDirPath()) + "/text/" + ui->cbSelectTheme->currentText() + ".txt");
    if (file.exists()) {
        qDebug() << "Удаление файла прошло " << file.remove();
    } else {
        qDebug() << "Файл не найден!";
    }

    insertCB();
}

void MainWindow::on_pbResult_clicked()
{
    model->select();
    ui->tabWidget->setCurrentWidget(ui->tabResult);
}

void MainWindow::on_pbBackMenu_clicked()
{
    ui->tabWidget->setCurrentWidget(ui->tabMenu);
}
