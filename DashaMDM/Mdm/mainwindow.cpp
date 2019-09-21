#include "mainwindow.h"
#include "ui_mainwindow.h"
//подключаем для относительного пути к БД
#include <QDir>
//подключаем для запросов к БД
#include <QSqlQuery>
//подключаем для вспылающихся окон
#include <QMessageBox>
//для вывода в консоль
#include <QDebug>

#include <QSqlError>
#include <QSqlRecord>
#include <QPainter>
#include <QCheckBox>
#include <QPrinter>
#include <QPrintDialog>

#include <checkboxitemdelegate.h>

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
        qDebug() << "Подключение выполненно!";

    createUI(QStringList() << trUtf8("id")
                                     << trUtf8("Готовность")
                                     << trUtf8("Название")
                                     << trUtf8("Должность")
                                     << trUtf8("Опыт")
                                     << trUtf8("Зарплата")
                                     << trUtf8("Пол")
                                     << trUtf8("Дата начала")
                                     << trUtf8("Дата окончания")
             );

    //создаём кнопку "Назад"
    QPushButton *buttonBack = new QPushButton();
    //присваиваем имя кнопки
    buttonBack->setText("Назад");
    //соединяем кнопку со слотом "Вернуться назад"
    connect(buttonBack, SIGNAL(clicked()),this, SLOT(on_pbBackCalendar_clicked()));
    //создаём виджет "Календарь"
    calendar = new CalendarWidget();
    calendar->setSelectionMode(QCalendarWidget::NoSelection);
    //добавляем календарь на форму
    ui->gridLayout_5->addWidget(calendar);
    //добавляем кнопку на форму
    ui->gridLayout_5->addWidget(buttonBack);

    //настраиваем проверку для полей "Подать заявку"
    QRegularExpression rx("^[а-яА-ЯёЁa-zA-Z .]+$");
    QValidator *validator = new QRegularExpressionValidator(rx, this);
    ui->leCorp->setValidator(validator);
    ui->lePlace->setValidator(validator);
    ui->leExp->setValidator( new QIntValidator(1, 16777215, this));
    ui->lePay->setValidator( new QDoubleValidator(1, 16777215, 2));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pbLogIn_clicked()
{
    //если поля не пустые
    if ((ui->leLogin->text() != "") && (ui->lePassword->text() != ""))
    {
        QSqlQuery query;
        query.prepare("SELECT * FROM Authorization WHERE Login=?");
        query.addBindValue(ui->leLogin->text());
        //если пользователь с таким логином найден
        if (query.exec())
        {
            qDebug() << "Запрос выполнен!";
            qDebug() <<  query.lastError().text();
            query.next();
            //если пароль совпал, то переходим дальше
            if (ui->lePassword->text() == query.value(1).toString())
            {
                QMessageBox::information(0, QString("Message"), QString("Добро пожаловать " + query.value(2).toString()), QMessageBox::Ok);
                ui->tabWidget->setCurrentIndex(1);
                //очищаем поля
                ui->leLogin->setText(QString());
                ui->lePassword->setText(QString());
            } else {
                QMessageBox::information(0, QString("Message"), QString("Неправильный пароль!"), QMessageBox::Ok);
                ui->lePassword->setText(QString());
            }
        } else {
            QMessageBox::information(0, QString("Message"), QString("Пользователь не найден!"), QMessageBox::Ok);
            ui->leLogin->setText(QString());
            ui->lePassword->setText(QString());
        }

    } else {
        QMessageBox::information(0, QString("Message"), QString("Заполните все необходимые поля!"), QMessageBox::Ok);
    }

}

void MainWindow::on_pbApply_clicked()
{
    if ((ui->leCorp->text() == "")||(ui->lePlace->text() == "")||(ui->leExp->text() == "")||(ui->lePay->text() == "")||(ui->deStart->text() == "")||(ui->deEnd->text() == "")) {
        QMessageBox::information(0, QString("Message"), QString("Заполните все поля!"), QMessageBox::Ok);
        return;
    }

    //запрос на добавление записи в БД
    QSqlQuery query;
    query.prepare("INSERT INTO Workers (Check1, Corp, Place, Experience, Pay, Sex, StartDate, EndDate) VALUES(?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(0);
    query.addBindValue(ui->leCorp->text());
    query.addBindValue(ui->lePlace->text());
    query.addBindValue(ui->leExp->text());
    query.addBindValue(ui->lePay->text());
    if (ui->cbSex->currentText() == "Муж")
        query.addBindValue("Муж");
    else
        query.addBindValue("Жен");
    query.addBindValue(ui->deStart->text());
    query.addBindValue(ui->deEnd->text());
    if (query.exec()) {
        qDebug() << "Заявка принята!";
        ui->tabWidget->setCurrentIndex(1);
    } else {
        qDebug() << "Опять какие-то проблемы!";
        qDebug() << query.lastError().text();
    }
}

void MainWindow::on_pbCandidates_clicked()
{
    ui->leSearch->setText(QString());
    connect(ui->tableWidget, SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),this, SLOT(DataChange()));
    ui->tableWidget->setRowCount(0);
    createUI(QStringList() << trUtf8("id")
                                     << trUtf8("Готовность")
                                     << trUtf8("Название")
                                     << trUtf8("Должность")
                                     << trUtf8("Опыт")
                                     << trUtf8("Зарплата")
                                     << trUtf8("Пол")
                                     << trUtf8("Дата начала")
                                     << trUtf8("Дата окончания")
             );
    //переходим на вкладку "Кандидаты"
    ui->tabWidget->setCurrentIndex(3);
}

void MainWindow::on_pbSearch_clicked()
{
    //запуск поиска по кнопке
    sort(ui->leSearch->text());
}

void MainWindow::sort(QString filter)
{
    ui->tableWidget->setRowCount(0);
    QSqlQuery query("SELECT * FROM Workers WHERE Place LIKE '" + filter + "%'");

    qDebug() << "Функция работает!";
    if ((query.exec()) && (ui->leSearch->text() != "")) {
        for(int i = 0; query.next(); i++){
            // Insert row
            ui->tableWidget->insertRow(i);
            ui->tableWidget->setItem(i,0, new QTableWidgetItem(query.value(0).toString()));

            // Create an element, which will serve as a checkbox
            QCheckBox* chk_bx = new QCheckBox();
            QWidget* cell_widget = new QWidget();
            QHBoxLayout* lay = new QHBoxLayout(cell_widget);
            lay->addWidget(chk_bx);
            lay->setAlignment(Qt::AlignCenter);
            lay->setContentsMargins(0,0,0,0);
            cell_widget->setLayout(lay);
            chk_bx->setCheckState(Qt::Checked);

            if(query.value(1).toInt() == 1)
                chk_bx->setCheckState(Qt::Checked);
            else
                chk_bx->setCheckState(Qt::Unchecked);
            ui->tableWidget->setCellWidget(i,1, cell_widget);
            for (int j=2; j <9; j++)
                ui->tableWidget->setItem(i,j, new QTableWidgetItem(query.value(j).toString()));
        }
        ui->tableWidget->resizeColumnsToContents();
        ui->tableWidget->resizeRowsToContents();
    } else {
        ui->tableWidget->setRowCount(0);
        createUI(QStringList() << trUtf8("id")
                                         << trUtf8("Готовность")
                                         << trUtf8("Название")
                                         << trUtf8("Должность")
                                         << trUtf8("Опыт")
                                         << trUtf8("Зарплата")
                                         << trUtf8("Пол")
                                         << trUtf8("Дата начала")
                                         << trUtf8("Дата окончания")
                 );
    }

}

void MainWindow::on_pbBack_clicked()
{
    if (changeData == true) {
        on_pbSave_clicked();
        changeData = false;
    }
    //возвращаемся назад
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::on_pbCalendar_clicked()
{
    //заполняем календарь датами встреч
    for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        QWidget *item = ( ui->tableWidget->cellWidget(i,1));
        QCheckBox *checkB = qobject_cast <QCheckBox*> (item->layout()->itemAt(0)->widget());
        if (checkB->checkState() == Qt::Unchecked)
            calendar->addMeetDate(QDate::fromString(ui->tableWidget->item(i,7)->text(),Qt::SystemLocaleDate), QStringList() << QString::number(0) << ui->tableWidget->item(i,2)->text());
        else
            calendar->addMeetDate(QDate::fromString(ui->tableWidget->item(i,7)->text(),Qt::SystemLocaleDate), QStringList() << QString::number(1) << ui->tableWidget->item(i,2)->text());

    }
    //переходим на вкладку с календарём
    ui->tabWidget->setCurrentIndex(4);
}

void MainWindow::on_pbOrder_clicked()
{
    ui->tabWidget->setCurrentIndex(2);
    //очистить все поля
    ui->leCorp->setText(QString());
    ui->lePlace->setText(QString());
    ui->leExp->setText(QString());
    ui->lePay->setText(QString());
    ui->cbSex->setCurrentIndex(0);
    ui->deStart->setDate(QDate::currentDate());
    ui->deEnd->setDate(QDate::currentDate());
}

void MainWindow::on_pbBackCalendar_clicked()
{
    //возвращаемся назад в меню
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::createUI(const QStringList &headers)
{
    if (StopFlag == true) {
        ui->tableWidget->setColumnCount(9);
        ui->tableWidget->setShowGrid(true);
        ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableWidget->setHorizontalHeaderLabels(headers);
        ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
        ui->tableWidget->hideColumn(0);
        ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        ui->tableWidget->resizeRowsToContents();
        ui->tableWidget->setFocusPolicy(Qt::NoFocus);
        StopFlag = false;
    }

    QSqlQuery query("SELECT * FROM Workers");

    for(int i = 0; query.next(); i++){
        // Insert row
        ui->tableWidget->insertRow(i);
        /* Set the id column in the first taking it from the result of the SQL-query.
         * This column will be hidden
         * */
        ui->tableWidget->setItem(i,0, new QTableWidgetItem(query.value(0).toString()));
        QCheckBox* chk_bx = new QCheckBox();
        QWidget* cell_widget = new QWidget();
        QHBoxLayout* lay = new QHBoxLayout(cell_widget);
        lay->addWidget(chk_bx);
        lay->setAlignment(Qt::AlignCenter);
        lay->setContentsMargins(0,0,0,0);
        cell_widget->setLayout(lay);
        chk_bx->setCheckState(Qt::Checked);
        if(query.value(1).toInt() == 1)
            chk_bx->setCheckState(Qt::Checked);
         else
            chk_bx->setCheckState(Qt::Unchecked);
        // Set the checkbox in the second column
        ui->tableWidget->setCellWidget(i,1, cell_widget);
        // Next, pick up all the data from a result set in other fields
        for (int j=2; j <9; j++)
            ui->tableWidget->setItem(i,j, new QTableWidgetItem(query.value(j).toString()));
        CheckBoxItemDelegate* deleg = new CheckBoxItemDelegate();
        for (int j=2; j <9; j++)
            ui->tableWidget->setItemDelegateForColumn(j, deleg);
    }
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeRowsToContents();

}

void MainWindow::on_pbSave_clicked()
{
    for (int i=0; i < ui->tableWidget->rowCount(); i++)
    {
        QSqlQuery query;
        query.prepare("UPDATE Workers SET Check1=?, Corp=?, Place=?, Experience=?, Pay=?, Sex=?, StartDate=?, EndDate=? WHERE ID=?");
        QWidget *item = ( ui->tableWidget->cellWidget(i,1));
        QCheckBox *checkB = qobject_cast <QCheckBox*> (item->layout()->itemAt(0)->widget());
        if (checkB->checkState() == Qt::Unchecked)
            query.addBindValue(0);
        else
            query.addBindValue(1);
        query.addBindValue(ui->tableWidget->item(i,2)->text());
        query.addBindValue(ui->tableWidget->item(i,3)->text());
        query.addBindValue(ui->tableWidget->item(i,4)->text());
        query.addBindValue(ui->tableWidget->item(i,5)->text());
        query.addBindValue(ui->tableWidget->item(i,6)->text());
        query.addBindValue(ui->tableWidget->item(i,7)->text());
        query.addBindValue(ui->tableWidget->item(i,8)->text());
        query.addBindValue(ui->tableWidget->item(i,0)->text());
        qDebug() << query.exec();
    }
}

void MainWindow::on_leSearch_editingFinished()
{
    //запуск поиска
    sort(ui->leSearch->text());
}

void MainWindow::on_pbPrint_clicked()
{
    QString strStream;
    QTextStream out(&strStream);

    const int rowCount = ui->tableWidget->rowCount();
    const int columnCount = ui->tableWidget->columnCount();
    QString strTitle = "Отчёт о встречах";

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
        if (!ui->tableWidget->isColumnHidden(column))
            out << QString("<th>%1</th>").arg(ui->tableWidget->horizontalHeaderItem(column)->text());
    }
    out << "</tr></thead>\n";

    // data table
    for (int row = 0; row < rowCount; row++) {
        out << "<tr>";
        for (int column = 0; column < columnCount; column++) {
            if (column == 1)
                continue;
            if (!ui->tableWidget->isColumnHidden(column)) {
                QString data = ui->tableWidget->item(row,column)->text();
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

    QPrintDialog *dialog = new QPrintDialog(&printer, NULL);
    if (dialog->exec() == QDialog::Accepted) {
        document->print(&printer);
    }

    delete document;
}

void MainWindow::on_pbLogOut_clicked()
{
    //выход на экарн "Логин"
    ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::on_pbCancel_clicked()
{
    //переход в главное меню
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::on_pbRemoveRow_clicked()
{
    if (ui->tableWidget->currentIndex().isValid()) {
        int row = ui->tableWidget->currentIndex().row();
        QSqlQuery query;
        query.prepare("DELETE FROM Workers WHERE ID=?");
        query.addBindValue(ui->tableWidget->item(row, 0)->text());
        qDebug() << query.exec();
        qDebug() << query.lastError().text();
        sort("efoef");
        return;
    }
    return;
}
