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
#include <QListView>

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

    createTable(ui->tableWidget,QStringList() << QString("id")
                << QString("Готовность")
                << QString("ФИО")
                << QString("Должность")
                << QString("Желаяемая ЗП, руб.")
                );
    createUI(ui->tableWidget,QString("Workers"));

    //создаём кнопку "Назад"
    QPushButton *buttonBack = new QPushButton();
    //присваиваем имя кнопки
    buttonBack->setText("Назад");
    //соединяем кнопку со слотом "Вернуться назад"
    connect(buttonBack, SIGNAL(clicked()), this, SLOT(on_pbBackCalendar_clicked()));
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
    ui->leFIO->setValidator(validator);
    ui->lePlaceHome->setValidator(new QRegularExpressionValidator(QRegularExpression("^[а-яА-ЯёЁa-zA-Z0-9 .,]+$"), this));
    ui->leEducation->setValidator(new QRegularExpressionValidator(QRegularExpression("^[а-яА-ЯёЁa-zA-Z ]+$"), this));
    ui->lePost->setValidator(validator);
    ui->leExp->setValidator( new QIntValidator(1, 16777215, this));
    ui->leWantPay->setValidator( new QDoubleValidator(1, 16777215, 2));

    //настраиваем проверку для полей "Добавить организацию"
    ui->leCorpName->setValidator(validator);
    ui->leCorpPost->setValidator(validator);
    ui->leCorpPay->setValidator( new QDoubleValidator(1, 16777215, 2));
}

void MainWindow::showPup()
{

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
                if (query.value(3).toInt() == 0)
                {
                    QMessageBox::information(nullptr, QString("Message"), QString("Добро пожаловать " + query.value(2).toString()), QMessageBox::Ok);
                    ui->tabWidget->setCurrentWidget(ui->tabMenu);
                    ui->pbOrder->hide();
                    ui->pbAddCorp->hide();
                } else {
                    QMessageBox::information(nullptr, QString("Message"), QString("Добро пожаловать " + query.value(2).toString()), QMessageBox::Ok);
                    ui->tabWidget->setCurrentWidget(ui->tabMenu);
                    ui->pbRemoveRow->hide();
                    ui->pbRemoveRow_2->hide();
                    ui->pbSave->hide();
                    ui->pbSave_2->hide();
                }
                //очищаем поля
                ui->leLogin->setText(QString());
                ui->lePassword->setText(QString());
            } else {
                QMessageBox::information(nullptr, QString("Message"), QString("Неправильный пароль!"), QMessageBox::Ok);
                ui->lePassword->setText(QString());
            }
        } else {
            QMessageBox::information(nullptr, QString("Message"), QString("Пользователь не найден!"), QMessageBox::Ok);
            ui->leLogin->setText(QString());
            ui->lePassword->setText(QString());
        }

    } else {
        QMessageBox::information(nullptr, QString("Message"), QString("Заполните все необходимые поля!"), QMessageBox::Ok);
    }

}

void MainWindow::on_pbApply_clicked()
{
    if ((ui->leFIO->text().isEmpty())||(ui->deBirthdate->text().isEmpty())||(ui->lePlaceHome->text().isEmpty())||
        (ui->lePhone->text().isEmpty())||(ui->leEducation->text().isEmpty())||(ui->lePost->text().isEmpty()) ||
        (ui->leExp->text().isEmpty()) || (ui->leWantPay->text().isEmpty()))
    {
        QMessageBox::information(nullptr, QString("Message"), QString("Заполните все поля!"), QMessageBox::Ok);
        return;
    }

    //запрос на добавление записи в БД
    QSqlQuery query;
    query.prepare("INSERT INTO Workers (Check1, Name, Birthdate, Sex, PlaceHome, Phone, Education, Post, Experience, WantPay, StartDate) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(0);
    query.addBindValue(ui->leFIO->text());
    query.addBindValue(ui->deBirthdate->text());
    if (ui->cbSex->currentText() == "Муж")
        query.addBindValue("Муж");
    else
        query.addBindValue("Жен");
    query.addBindValue(ui->lePlaceHome->text());
    query.addBindValue(ui->lePhone->text());
    query.addBindValue(ui->leEducation->text());
    query.addBindValue(ui->lePost->text());
    query.addBindValue(ui->leExp->text());
    query.addBindValue(ui->leWantPay->text());
    query.addBindValue(QDate::currentDate().toString(Qt::LocalDate));
    if (query.exec()) {
        qDebug() << "Заявка принята!";
        ui->tabWidget->setCurrentWidget(ui->tabMenu);
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
    createUI(ui->tableWidget,"Workers");
    //переходим на вкладку "Кандидаты"
    ui->tabWidget->setCurrentIndex(3);
}

void MainWindow::on_pbSearch_clicked()
{
    //запуск поиска по кнопке
    sort(ui->tableWidget, QString("Workers"), ui->leSearch, ui->leSearch->text());
}

void MainWindow::sort(QTableWidget* parent, QString tableName, QLineEdit* edit, QString filter)
{
    parent->setRowCount(0);
    QString prep = "SELECT * FROM " + tableName + " WHERE CorpPost LIKE '" + filter + "%'";
    QSqlQuery query(prep);

    qDebug() << "Функция работает!";
    if ((query.exec()) && (edit->text() != "")) {
        for(int i = 0; query.next(); i++){
            // Insert row
            parent->insertRow(i);
            parent->setItem(i,0, new QTableWidgetItem(query.value(0).toString()));

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
            parent->setCellWidget(i,1, cell_widget);
            for (int j=2; j < parent->rowCount(); j++)
                parent->setItem(i,j, new QTableWidgetItem(query.value(j).toString()));
        }
        parent->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        parent->resizeRowsToContents();
    } else {
        parent->setRowCount(0);
        createUI(parent,tableName);
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
    QSqlQuery add("SELECT * FROM Workers");
    //заполняем календарь датами встреч
    for (int i = 0; add.next(); ++i) {
        QWidget *item = ( ui->tableWidget->cellWidget(i,1));
        QCheckBox *checkB = qobject_cast <QCheckBox*> (item->layout()->itemAt(0)->widget());
        if (checkB->checkState() == Qt::Unchecked) {
            calendar->addMeetDate(QDate::fromString(add.value(11).toString(),Qt::SystemLocaleDate), QStringList() << QString::number(0) << ui->tableWidget->item(i,2)->text());
        } else {
            calendar->addMeetDate(QDate::fromString(add.value(11).toString(),Qt::SystemLocaleDate), QStringList() << QString::number(1) << ui->tableWidget->item(i,2)->text());
        }

    }
    //переходим на вкладку с календарём
    ui->tabWidget->setCurrentWidget(ui->tabCalendar);
}

void MainWindow::on_pbOrder_clicked()
{
    ui->tabWidget->setCurrentWidget(ui->tabOrder);
    //очистить все поля
    ui->leFIO->setText(QString());
    ui->deBirthdate->setDate(QDate(1,1,2000));
    ui->cbSex->setCurrentIndex(0);
    ui->lePlaceHome->setText(QString());
    ui->lePhone->clear();
    ui->leEducation->clear();
    ui->lePost->setText(QString());
    ui->leExp->setText(QString());
    ui->leWantPay->setText(QString());
}

void MainWindow::on_pbBackCalendar_clicked()
{
    //возвращаемся назад в меню
    ui->tabWidget->setCurrentIndex(1);
}

//функция создания таблицы
void MainWindow::createTable(QTableWidget *parent, const QStringList &headers)
{
    parent->setColumnCount(headers.count());
    parent->setShowGrid(true);
    parent->setSelectionMode(QAbstractItemView::SingleSelection);
    parent->setSelectionBehavior(QAbstractItemView::SelectRows);
    parent->setHorizontalHeaderLabels(headers);
    parent->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    parent->hideColumn(0);
    //parent->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    parent->resizeRowsToContents();
    parent->setFocusPolicy(Qt::NoFocus);
}

void MainWindow::createUI(QTableWidget* parent,QString tableName)
{
    QString prep = "SELECT * FROM " + tableName;
    QSqlQuery query(prep);

    for(int i = 0; query.next(); i++){
        parent->insertRow(i);
        parent->setItem(i,0, new QTableWidgetItem(query.value(0).toString()));
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
        parent->setCellWidget(i,1, cell_widget);
        //если таблица "Кандидаты"
        if (parent == ui->tableWidget) {
            parent->setItem(i, 2, new QTableWidgetItem(query.value(2).toString()));
            parent->setItem(i, 3, new QTableWidgetItem(query.value(8).toString()));
            parent->setItem(i, 4, new QTableWidgetItem(query.value(10).toString()));
        } else {
            for (int j=2; j < parent->horizontalHeader()->count(); j++)
                parent->setItem(i,j, new QTableWidgetItem(query.value(j).toString()));
        }

//        CheckBoxItemDelegate* deleg = new CheckBoxItemDelegate();
//        for (int j=2; j < parent->horizontalHeader()->count(); j++)
//            parent->setItemDelegateForColumn(j, deleg);
    }
    parent->resizeColumnsToContents();
    parent->resizeRowsToContents();

}

//функция создания второй таблицы
void MainWindow::createSecondTable()
{
    //очищаем строку
    listTable1.clear();
    listTable2.clear();
    listTable3.clear();
    listTable4.clear();
    listTable5.clear();
    ui->twListOrg->setRowCount(0);

    QStringList headers;
    headers <<"ID" << "Компания" << "Вакансия" << "ФИО Кандидата" << "Пол" << "Возраст" << "Уровень дохода, запрашиваемый кандидатом"
            << "Заявленный доход от компании" << "Образование" << "Дополнительные условия";
    ui->twListOrg->setColumnCount(headers.count());
    ui->twListOrg->setShowGrid(true);
   // ui->twListOrg->setSelectionMode(QAbstractItemView::NoSelection);
    ui->twListOrg->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->twListOrg->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->twListOrg->setHorizontalHeaderLabels(headers);
    ui->twListOrg->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->twListOrg->hideColumn(0);
    ui->twListOrg->resizeRowsToContents();
    ui->twListOrg->setFocusPolicy(Qt::NoFocus);

    QString prep = "SELECT * FROM CorpsTable";
    QSqlQuery query(prep);

    for(int i = 0; query.next(); i++){
        ui->twListOrg->insertRow(i);
        ui->twListOrg->setItem(i,0, new QTableWidgetItem(query.value(0).toString()));
        ui->twListOrg->setItem(i,1, new QTableWidgetItem(query.value(1).toString()));
        ui->twListOrg->setItem(i,2, new QTableWidgetItem(query.value(2).toString()));
        ui->twListOrg->setItem(i,7, new QTableWidgetItem(query.value(3).toString()));
        ui->twListOrg->setItem(i,9, new QTableWidgetItem(query.value(4).toString()));
        QSqlQuery ins;
        ins.prepare("SELECT * FROM Workers WHERE Post=?");
        ins.addBindValue(query.value(2).toString());
        if (ins.exec()){
            qDebug() << "Запрос INS выполнен!";
            int count = 0;
            listTable1.append(new QTableWidget());
            listTable1.at(i)->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            listTable1.at(i)->setStyleSheet("QTableView {border: none;}");
            listTable1.at(i)->setSelectionMode(QAbstractItemView::NoSelection);
            listTable1.at(i)->setEditTriggers(QAbstractItemView::NoEditTriggers);
            listTable2.append(new QTableWidget());
            listTable2.at(i)->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            listTable2.at(i)->setStyleSheet("QTableView {border: none;}");
            listTable2.at(i)->setSelectionMode(QAbstractItemView::NoSelection);
            listTable2.at(i)->setEditTriggers(QAbstractItemView::NoEditTriggers);
            listTable3.append(new QTableWidget());
            listTable3.at(i)->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            listTable3.at(i)->setStyleSheet("QTableView {border: none;}");
            listTable3.at(i)->setSelectionMode(QAbstractItemView::NoSelection);
            listTable3.at(i)->setEditTriggers(QAbstractItemView::NoEditTriggers);
            listTable4.append(new QTableWidget());
            listTable4.at(i)->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            listTable4.at(i)->setStyleSheet("QTableView {border: none;}");
            listTable4.at(i)->setSelectionMode(QAbstractItemView::NoSelection);
            listTable4.at(i)->setEditTriggers(QAbstractItemView::NoEditTriggers);
            listTable5.append(new QTableWidget());
            listTable5.at(i)->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            listTable5.at(i)->setStyleSheet("QTableView {border: none;}");
            listTable5.at(i)->setSelectionMode(QAbstractItemView::NoSelection);
            listTable5.at(i)->setEditTriggers(QAbstractItemView::NoEditTriggers);
            for (int j=0; ins.next(); j++) {
                listTable1.at(i)->setColumnCount(1);
                listTable1.at(i)->verticalHeader()->hide();
                listTable1.at(i)->horizontalHeader()->hide();
                listTable1.at(i)->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                listTable1.at(i)->setWordWrap(false);
                listTable1.at(i)->insertRow(j);
                listTable1.at(i)->setItem(j,0, new QTableWidgetItem(ins.value(2).toString()));
                qDebug() << ins.value(2).toString();
                listTable1.at(i)->resizeColumnsToContents();
                listTable1.at(i)->resizeRowsToContents();

                listTable2.at(i)->setColumnCount(1);
                listTable2.at(i)->verticalHeader()->hide();
                listTable2.at(i)->horizontalHeader()->hide();
                listTable2.at(i)->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                listTable2.at(i)->insertRow(j);
                listTable2.at(i)->setItem(j,0, new QTableWidgetItem(ins.value(4).toString()));
                listTable2.at(i)->resizeColumnsToContents();
                listTable2.at(i)->resizeRowsToContents();

                listTable3.at(i)->setColumnCount(1);
                listTable3.at(i)->verticalHeader()->hide();
                listTable3.at(i)->horizontalHeader()->hide();
                listTable3.at(i)->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                listTable3.at(i)->insertRow(j);
                listTable3.at(i)->setItem(j,0, new QTableWidgetItem(ins.value(3).toString()));
                listTable3.at(i)->resizeColumnsToContents();
                listTable3.at(i)->resizeRowsToContents();

                listTable4.at(i)->setColumnCount(1);
                listTable4.at(i)->verticalHeader()->hide();
                listTable4.at(i)->horizontalHeader()->hide();
                listTable4.at(i)->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                listTable4.at(i)->insertRow(j);
                listTable4.at(i)->setItem(j,0, new QTableWidgetItem(ins.value(10).toString()));
                listTable4.at(i)->resizeColumnsToContents();
                listTable4.at(i)->resizeRowsToContents();

                listTable5.at(i)->setColumnCount(1);
                listTable5.at(i)->verticalHeader()->hide();
                listTable5.at(i)->horizontalHeader()->hide();
                listTable5.at(i)->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                listTable5.at(i)->insertRow(j);
                listTable5.at(i)->setItem(j,0, new QTableWidgetItem(ins.value(7).toString()));
                listTable5.at(i)->resizeColumnsToContents();
                listTable5.at(i)->resizeRowsToContents();
                count = count + listTable5.at(i)->rowHeight(j);
            }
            listTable1.at(i)->setMaximumHeight(count+15);
            listTable2.at(i)->setMaximumHeight(count+15);
            listTable3.at(i)->setMaximumHeight(count+15);
            listTable4.at(i)->setMaximumHeight(count+15);
            listTable5.at(i)->setMaximumHeight(count+15);
        } else {
        qDebug() << "Что-то пошло не так!";
        qDebug() << ins.lastError().text();
        }
        ui->twListOrg->setCellWidget(i, 3, listTable1.at(i));
        ui->twListOrg->setCellWidget(i, 4, listTable2.at(i));
        ui->twListOrg->setCellWidget(i, 5, listTable3.at(i));
        ui->twListOrg->setCellWidget(i, 6, listTable4.at(i));
       // ui->twListOrg->setCellWidget(i, )
        ui->twListOrg->setCellWidget(i, 8, listTable5.at(i));
    }
    ui->twListOrg->resizeRowsToContents();
    ui->twListOrg->resizeColumnsToContents();

    CheckBoxItemDelegate* deleg = new CheckBoxItemDelegate();
        for (int j=0; j < ui->twListOrg->horizontalHeader()->count(); j++)
            ui->twListOrg->setItemDelegateForColumn(j, deleg);

}

//функция для срхранения таблицы
void MainWindow::saveTable(QTableWidget *parent, QString tableName)
{
    for (int i=0; i < parent->rowCount(); i++)
    {
        QString prep = "UPDATE " + tableName + " SET CorpName=?, CorpPost=?, CorpPay=?, CordAddReq=? WHERE ID=?";
        QSqlQuery query;
        query.prepare(prep); 
        query.addBindValue(parent->item(i,1)->text());
        query.addBindValue(parent->item(i,2)->text());
        query.addBindValue(parent->item(i,7)->text());
        query.addBindValue(parent->item(i,9)->text());
        query.addBindValue(parent->item(i,0)->text());
        qDebug() << query.exec();
    }
}

//функция удаления строки из таблицы
void MainWindow::removeRow(QTableWidget *parent, QString tableName, QLineEdit* edit)
{
    if (parent->currentIndex().isValid()) {
        int row = parent->currentIndex().row();
        QString prep = "DELETE FROM " + tableName + " WHERE ID=?";
        QSqlQuery query;
        query.prepare(prep);
        query.addBindValue(parent->item(row, 0)->text());
        qDebug() << query.exec();
        qDebug() << query.lastError().text();
        //sort(parent, QString(tableName), edit, "w6f1w6f6");
        return;
    }
}

void MainWindow::on_pbSave_clicked()
{
    for (int i=0; i < ui->tableWidget->rowCount(); i++)
    {
        QString prep = "UPDATE Workers SET Check1=?, Name=?, Post=?, WantPay=? WHERE ID=?";
        QSqlQuery query;
        query.prepare(prep);
        QWidget *item = ( ui->tableWidget->cellWidget(i,1));
        QCheckBox *checkB = qobject_cast <QCheckBox*> (item->layout()->itemAt(0)->widget());
        if (checkB->checkState() == Qt::Unchecked)
            query.addBindValue(0);
        else
            query.addBindValue(1);
        for (int j=2; j<ui->tableWidget->columnCount(); j++)
            query.addBindValue(ui->tableWidget->item(i,j)->text());
        query.addBindValue(ui->tableWidget->item(i,0)->text());
        qDebug() << query.exec();
    }
}

void MainWindow::on_leSearch_editingFinished()
{
    //запуск поиска
    sort(ui->tableWidget, QString("Workers"), ui->leSearch, ui->leSearch->text());
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

    QPrintDialog *dialog = new QPrintDialog(&printer, nullptr);
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
        removeRow(ui->tableWidget, "Workers", ui->leSearch);
        sort(ui->tableWidget, "Workers", ui->leSearch, "w6f1w6f6");
    }

}

void MainWindow::on_pbApply_2_clicked()
{
    if ((ui->leCorpName->text().isEmpty()) || (ui->leCorpPost->text().isEmpty()) || (ui->leCorpPay->text().isEmpty()) || (ui->leCorpAdd->text().isEmpty())) {
        QMessageBox::information(nullptr, QString("Message"), QString("Заполните все поля!"), QMessageBox::Ok);
        return;
    }
    //запрос на добавление записи в БД
    QSqlQuery query;
    query.prepare("INSERT INTO CorpsTable (CorpName, CorpPost, CorpPay, CordAddReq) VALUES(?, ?, ?, ?)");
    query.addBindValue(ui->leCorpName->text());
    query.addBindValue(ui->leCorpPost->text());
    query.addBindValue(ui->leCorpPay->text());
    query.addBindValue(ui->leCorpAdd->text());
    if (query.exec()) {
        qDebug() << "Заявка принята!";
        ui->tabWidget->setCurrentWidget(ui->tabMenu);
    } else {
        qDebug() << "Опять какие-то проблемы!";
        qDebug() << query.lastError().text();
    }
}

void MainWindow::on_pbAddCorp_clicked()
{
    ui->tabWidget->setCurrentWidget(ui->tabAddCorp);
    //очистить все поля
    ui->leCorpName->setText(QString());
    ui->leCorpPay->setText(QString());
    ui->leCorpAdd->setText(QString());
    ui->leCorpPost->setText(QString());
}

void MainWindow::on_pbCancel_2_clicked()
{
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::on_pbListCorps_clicked()
{
    ui->leSearch_2->hide();
    ui->pbSearch_2->hide();
    createSecondTable();
    ui->tabWidget->setCurrentWidget(ui->tabCorps);
}

void MainWindow::on_pbBack_2_clicked()
{
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::on_pbSave_2_clicked()
{
    saveTable(ui->twListOrg, QString("CorpsTable"));
}

void MainWindow::on_pbRemoveRow_2_clicked()
{
    removeRow(ui->twListOrg, "CorpsTable", ui->leSearch_2);
    createSecondTable();
    return;
}

void MainWindow::on_pbSearch_2_clicked()
{
    //запуск поиска по кнопке
    sort(ui->twListOrg, QString("CorpsTable"), ui->leSearch_2, ui->leSearch_2->text());
}

void MainWindow::on_pbMoreBack_clicked()
{
    ui->tabWidget->setCurrentWidget(ui->tabCandidates);
}

void MainWindow::on_tableWidget_itemDoubleClicked(QTableWidgetItem *item)
{
    //очищаем поля
    ui->leMoreFIO->clear();
    ui->leMoreBirthdate->clear();
    ui->leMoreSex->clear();
    ui->leMorePlaceHome->clear();
    ui->leMorePhone->clear();
    ui->leMoreEducation->clear();
    ui->leMorePost->clear();
    ui->leMoreExp->clear();
    ui->leMoreWantPay->clear();
    ui->leDateStart->clear();

    QSqlQuery query;
    query.prepare("SELECT * FROM Workers WHERE ID=?");
    query.addBindValue(ui->tableWidget->item(item->row(), 0)->text());
    qDebug() << ui->tableWidget->item(item->row(), 0)->text();
    if (query.exec()) {
        qDebug() << "Запрос выполнен!";
        qDebug() << query.first();
        ui->leMoreFIO->setText(query.value(2).toString());
        ui->leMoreBirthdate->setText(query.value(3).toString());
        ui->leMoreSex->setText(query.value(4).toString());
        ui->leMorePlaceHome->setText(query.value(5).toString());
        ui->leMorePhone->setText(query.value(6).toString());
        ui->leMoreEducation->setText(query.value(7).toString());
        ui->leMorePost->setText(query.value(8).toString());
        ui->leMoreExp->setText(query.value(9).toString());
        ui->leMoreWantPay->setText(query.value(10).toString());
        ui->leDateStart->setText(query.value(11).toString());

        ui->tabWidget->setCurrentWidget(ui->tabMore);
    } else {
        qDebug() << "Что-то пошло не так!";
        qDebug() << query.lastError().text();
    }

}
