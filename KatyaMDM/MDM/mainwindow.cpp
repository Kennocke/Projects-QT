#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QMessageBox>
#include <QTextDocument>
#include <QTextStream>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tabWidget->tabBar()->hide();

    ui->pbPrint->hide();

    //создание базы данных SQLITE
    db = QSqlDatabase::addDatabase("QSQLITE");
    //указываем путь к базе данных
    db.setDatabaseName(QDir::toNativeSeparators(QApplication::applicationDirPath()) + "/database.db3");
    //устанавливаем соединение с БД
    db.open();
    //создание новой модели
    model = new QSqlTableModel(this, db);
    //устанавливаем таблицу Capital в модель
    model->setTable("Capital");
    //создаём список заголовком
    QStringList horizontalHeader = {"ID", "Сотрудник", "Дата операции", "Наименование", "Кол-во", "Цена за ед.(руб.)", "Стоимость", "Статья вложений", "Срок эксплуатации(год)"};
    //устанавливем заголовки в нашу модель
    for (int i = 0; i < 9; ++i) {
        model->setHeaderData(i, Qt::Horizontal, horizontalHeader[i]);
    }
    model->sort(8, Qt::AscendingOrder);
    //заполняем модель из указанной таблицы
    model->select();

    //вставить ComboBox

    //устанавливаем нашу модель в виджет TableView
    ui->tableView->setModel(model);
    //устанавливаем выделение целой строки
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    //задаём ширину столбцов равную ширине таблице
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //устанавливаем ширину последнего столбца равную контенту
    ui->tableView->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents);
    //задаём высоту ячеек равную высоте контента
    ui->tableView->resizeRowsToContents();
    //запрет на редактирования ячеек таблицы
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //режим выделения только одной строки
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    //вызываем обновление итога
    updateResult();

    //валидатор для вкладки отчёты
    ui->leReportMoney->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]*([\\.]?[0-9]{0,2})?")));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pbAdd_clicked()
{
    //создание нового окна
    formAdd = new DialogAdd(0, currentLogin);
    //соединяем сигнал со слотом, который обновляет модель
    connect(formAdd, &DialogAdd::transmitter, this, &MainWindow::changeModel);
    //устанавливает модальность нашему окну. Теперь нельзя использовать основное окно пока открыто это.
    formAdd->setModal(true);
    //отображение нового окна
    formAdd->show();
}

void MainWindow::on_pbRemove_clicked()
{
    qDebug() << ui->tableView->currentIndex().row();
    qDebug() << model->rowCount();
    if ((ui->tableView->currentIndex().isValid()) && (ui->tableView->currentIndex().row() != model->rowCount()-1)) {
        //получаем выделенную строку
        int row = ui->tableView->currentIndex().row();
        //удаление строки
        model->removeRows(row, 1);
        //принять все изменения
        model->submitAll();
        //обновить модель в таблице
        model->select();
        return;
    }
    return;
}

void MainWindow::changeModel()
{
    //вызываем обновление итога
    updateResult();
    //применяет все изменения к модели
    model->submitAll();
    //установка выбранной таблицы в модель. Здесь выстпает в качестве обновления после изменения таблицы.
    model->select();

}

//метод обновления итога
void MainWindow::updateResult()
{
//    double countPrice = 0;
//    QSqlQuery queryGet("SELECT PriceAll FROM Capital");
//    if (queryGet.exec())
//    {
//        qDebug() << "queryGet выполнен успешно!";
//        while (queryGet.next()){
//            countPrice += queryGet.value(0).toDouble();
//        }
//    } else {
//        qDebug() << "queryGet не выполнен!";
//        qDebug() << queryGet.lastError().text();
//    }
//    qDebug() << countPrice;
//    ui->leMainSum->setText(QString::number(countPrice, 'f', 2));

    double countMoney = 0;

    for (int i =0; i < model->rowCount(); i++) {
        countMoney += model->index(i,6).data().toDouble();
    }
    ui->leMainSum->setText(QString::number(countMoney, 'f', 2));
}

void MainWindow::on_pbChange_clicked()
{
    if (ui->tableView->currentIndex().isValid()) {
        //создание нового окна
        formChange = new DialogChange();
        //соединяем сигнал со слотом, который обновляет модель
        connect(formChange, &DialogChange::transmitter, this, &MainWindow::changeModel);
        //устанавливает модальность нашему окну. Теперь нельзя использовать основное окно пока открыто это.
        formChange->setModal(true);
        //отображение окна
        formChange->show();
        //вызов функции установки полей в виджеты
        formChange->setDB(model, ui->tableView->currentIndex().row());
    } else {
        QMessageBox::information(0, QString("Предупреждение"), QString("Выберите запись!"), QMessageBox::Ok);
    }
}

void MainWindow::on_pbPrint_clicked()
{
    QString strStream;
    QTextStream out(&strStream);

    const int rowCount = ui->tableView->model()->rowCount();
    const int columnCount = ui->tableView->model()->columnCount();
    QString strTitle = "Учёт капитальных вложений";

    out <<  "<html>\n"
        "<head>\n"
        "<meta Content=\"Text/html; charset=Windows-1251\">\n"
        <<  QString("<title>%1</title>\n").arg(strTitle)
        <<  "</head>\n"
        "<body bgcolor=#ffffff link=#5000A0>\n"
        "<table border=1 cellspacing=0 cellpadding=2 width='100%'>\n";

    // headers
    out << "<thead><tr bgcolor=#f0f0f0>";
    for (int column = 0; column < columnCount; column++)
        if (!ui->tableView->isColumnHidden(column))
            out << QString("<th>%1</th>").arg(ui->tableView->model()->headerData(column, Qt::Horizontal).toString());
    out << "</tr></thead>\n";

    // data table
    for (int row = 0; row < rowCount; row++) {
        out << "<tr>";
        for (int column = 0; column < columnCount; column++) {
            if (!ui->tableView->isColumnHidden(column)) {
                QString data = ui->tableView->model()->data(ui->tableView->model()->index(row, column)).toString();
                if ((column == 4) && (row == rowCount-1)) {
                    data = ui->tableView->model()->data(ui->tableView->model()->index(row, column)).toString();
                    qDebug() << "Сработало!";
                } else if ((column == 5) || (column == 4)) {
                    data = QString::number(ui->tableView->model()->data(ui->tableView->model()->index(row, column)).toString().toFloat(),'f',2);
                }
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

void MainWindow::on_pbLogin_clicked()
{
    //если поля не пустые
    if ((ui->leLogin->text() != "") && (ui->lePassword->text() != ""))
    {
        QSqlQuery query;
        query.prepare("SELECT * FROM Authorization WHERE Login=?");
        query.addBindValue(ui->leLogin->text());
        //выполняем запрос
        query.exec();
        //если пользователь с таким логином найден
        if (query.first()){
            qDebug() << "Запрос выполнен!";

            //если пароль совпал, то переходим дальше
            if (ui->lePassword->text() == query.value(1).toString())
            {
                if (query.value(3) == 0) {
                    qDebug() << "Зашёл админ!";
                    ui->pbRemove->setVisible(true);
                    currentLogin = query.value(0).toString();
                    QMessageBox::information(0, QString("Message"), QString("Добро пожаловать " + query.value(2).toString()), QMessageBox::Ok);
                    ui->tabWidget->setCurrentWidget(ui->tabMain);
                } else {
                    //присваиваем текущий логин
                    currentLogin = query.value(0).toString();
                    ui->pbRemove->hide();
                    QMessageBox::information(0, QString("Message"), QString("Добро пожаловать " + query.value(2).toString()), QMessageBox::Ok);
                    ui->tabWidget->setCurrentIndex(0);
                }
                //очищаем поля
                ui->leLogin->setText(QString());
                ui->lePassword->setText(QString());
                ui->leSearch->setText("");
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

void MainWindow::on_pbExit_clicked()
{
    ui->tabWidget->setCurrentWidget(ui->tabAuthorization);
    ui->leLogin->clear();
    ui->lePassword->clear();
}

void MainWindow::on_pbPrintRow_clicked()
{
    if (ui->tableView->currentIndex().isValid())
    {
        QString strStream;
        QTextStream out(&strStream);

        int row = ui->tableView->currentIndex().row();
        const int rowCount = ui->tableView->model()->rowCount();
        const int columnCount = ui->tableView->model()->columnCount();
        QString strTitle = "Учёт капитальных вложений";

        out <<  "<html>\n"
            "<head>\n"
            "<meta Content=\"Text/html; charset=Windows-1251\">\n"
            <<  QString("<title>%1</title>\n").arg(strTitle)
            <<  "</head>\n"
            "<body bgcolor=#ffffff link=#5000A0>\n"
            "<table border=1 cellspacing=0 cellpadding=2 width='100%'>\n";

        // headers
        out << "<thead><tr bgcolor=#f0f0f0>";
        for (int column = 0; column < columnCount; column++)
            if (!ui->tableView->isColumnHidden(column))
                out << QString("<th>%1</th>").arg(ui->tableView->model()->headerData(column, Qt::Horizontal).toString());
        out << "</tr></thead>\n";

        // data table
            out << "<tr>";
            for (int column = 0; column < columnCount; column++) {
                if (!ui->tableView->isColumnHidden(column)) {
                        QString data = ui->tableView->model()->data(ui->tableView->model()->index(row, column)).toString();
                            if ((row == model->rowCount() - 1) && (column == 4)) {
                                data = ui->tableView->model()->data(ui->tableView->model()->index(row, column)).toString();
                            } else if ((column == 5) || (column == 4)) {
                                data = QString::number(ui->tableView->model()->data(ui->tableView->model()->index(row, column)).toString().toFloat(),'f',2);
                            }
                    out << QString("<td bkcolor=0>%1</td>").arg((!data.isEmpty()) ? data : QString("&nbsp;"));
                }
            }
            out << "</tr>\n";
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
    } else {
        QMessageBox::information(0, QString("Осторожно(><)"), QString("Выберите строку!"), QMessageBox::Ok);
    }
}

void MainWindow::on_pbSearch_clicked()
{
    if (ui->leSearch->text() != "") {
        model->setFilter("Name LIKE '" + ui->leSearch->text() + "%'");
    } else {
        model->setFilter("");
    }
    model->select();
    updateResult();
}

void MainWindow::on_leSearch_editingFinished()
{
    on_pbSearch_clicked();
}

void MainWindow::on_pushButton_clicked()
{
    QSqlQuery query;
    double sum = 0;
    int rowCount = 0;
    int columnCount = 9;
    if (ui->rbReportDate->isChecked()) {
        query.prepare("SELECT * FROM Capital WHERE DateCreate BETWEEN ? AND ?");
        query.addBindValue(ui->deStart->date().toString(Qt::ISODate));
        query.addBindValue(ui->deEnd->date().toString(Qt::ISODate));
    } else if (ui->rbReportType->isChecked()) {
        query.prepare("SELECT * FROM Capital WHERE Article=?");
        query.addBindValue(ui->cbType->currentText());
    } else {
        query.prepare("SELECT * FROM Capital WHERE PriceAll " + ui->cbOperator->currentText() + ui->leReportMoney->text());
    }
    qDebug() << ui->deStart->date().toString(Qt::ISODate);

    if (query.exec()) {
        qDebug() << "Запрос выполнен!";
        if(query.last())
        {
            rowCount =  query.at() + 1;
            query.first();
            query.previous();
        }
    } else {
        qDebug() << query.lastError().text();
        return;
    }

    if (rowCount == 0) {
        QMessageBox::information(nullptr, QString("Приветствие"), QString("Таблица пустая!"), QMessageBox::Ok);
        return;
    }

    QString strStream;
    QTextStream out(&strStream);

    QString strTitle = "Отчёт";
    QStringList list;
    list << "ID" << "Сотрудник" << "Дата создания" << "Название" << "Кол-во" << "Цена за ед., руб." << "Сумма, руб."
         << "Статья вложений" << "Срок эксплуатации";

    out <<  "<html>\n"
        "<head>\n"
        "<meta Content=\"Text/html; charset=Windows-1251\">\n"
        <<  QString("<title>%1</title>\n").arg(strTitle)
        <<  "</head>\n"
        "<body bgcolor=#ffffff link=#5000A0>\n"
        "<table border=1 cellspacing=0 cellpadding=2 width='100%'>\n";

    // headers
    out << "<thead><tr bgcolor=#f0f0f0>";
    for (int column = 0; column < list.size(); column++) {
        if (!list.at(column).isEmpty())
            out << QString("<th>%1</th>").arg(list.at(column));
    }
    out << "</tr></thead>\n";

    // data table
    for (int i=0; query.next(); i++) {
        out << "<tr>";
        for (int column = 0; column < columnCount; column++) {
            if (column == 6) {
                sum += query.value(column).toDouble();
            }
            QString data = query.value(column).toString();
            out << QString("<td bkcolor=0>%1</td>").arg((!data.isEmpty()) ? data : QString("&nbsp;"));
        }
        out << "</tr>\n";
    }
    out << "<tr>";
        for (int i=0; i < 7; i++) {
            if (i==5)
                out << QString("<td bkcolor=0>%1</td>").arg("Итого: ");
            else if (i==6)
                out << QString("<td bkcolor=0>%1</td>").arg(QString::number(sum, 'f', 2));
            else
                out << QString("<td bkcolor=0>%1</td>").arg("");
        }
        out << "</tr>\n";
    out <<  "</table>\n"
        "</body>\n"
        "</html>\n";

    document = new QTextDocument();
    document->setHtml(strStream);

    QPrinter printer;
    printer.setOrientation(QPrinter::Landscape);

    QPrintDialog *dialog = new QPrintDialog(&printer, NULL);
    if (dialog->exec() == QDialog::Accepted) {
        document->print(&printer);
    }

    delete document;
}

void MainWindow::on_pbPrintReports_clicked()
{
    ui->leReportMoney->clear();
    ui->tabWidget->setCurrentWidget(ui->tabReports);
}

void MainWindow::on_pbReportBack_clicked()
{
    ui->tabWidget->setCurrentWidget(ui->tabMain);
}

void MainWindow::on_leSearch_textChanged(const QString &arg1)
{
    on_pbSearch_clicked();
}

void MainWindow::on_lePassword_returnPressed()
{
    on_pbLogin_clicked();
}
