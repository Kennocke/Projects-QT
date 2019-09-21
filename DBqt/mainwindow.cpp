#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "comboboxitemdelegate.h"

#include "QtSql/QSqlDatabase"
#include "QSqlQuery"
#include <QDir>
#include <QTextDocument>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include "nonedittablecolumndelegate.h"
#include "dialogchange.h"

//#include "QStandardItemModel"
//#include "QStandardItem"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //создание базы данных типа SQLLITE
    db = QSqlDatabase::addDatabase("QSQLITE");
    //указываем путь к базе данных
    db.setDatabaseName(QDir::toNativeSeparators(QApplication::applicationDirPath()) + "/database.db3");
    //проверка на подклюение к базе данных
    if (db.open())
        qDebug() << "Database opened!";
    model = new QSqlTableModel(this, db);
    model->setTable("People");
    model->select();
    //установка в таблицу модели
    ComboBoxItemDelegate* cbid = new ComboBoxItemDelegate();
    ui->tableView->setModel(model);

    ui->tableView->setItemDelegateForColumn(2, cbid);

    //запрет на редактирования столбца ID. Для этого нужно переопределить делегат.
    //ui->tableView->setItemDelegateForColumn(0, new NonEditTableColumnDelegate());

    //установка выделения целой строки
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    //задаём ширину столбцов равную ширине таблице
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //задаём высоту ячеек равную высоте контента
    ui->tableView->resizeRowsToContents();

    //устанавливаем маску для ввода даты
    ui->leDate->setInputMask("00/00/0000");
    qDebug() << ui->leDate->inputMask();
    //регулярное выражение для ввода данных
    QRegularExpression rx("^(0[1-9]|[12][0-9]|3[01])[- /.](0[1-9]|1[012])[- /.](19|20)[0-9][0-9]$");
    QValidator *validator = new QRegularExpressionValidator(rx, this);

    QLineEdit *edit = new QLineEdit(this);
    //edit->setInputMask("00/00/00");
    edit->setPlaceholderText("00.00.0000");
    edit->setValidator(validator);

    ui->formLayout->addWidget(edit);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pbConnect_clicked()
{
    //model = new QStandardItemModel;
    //QStandardItem *item;

//    //осуществляем запрос
//    QSqlQuery query;
//    query.exec("SELECT _id, name, age FROM People");

//    //заголовки столбцов
//    QStringList horizontalHeader;
//    horizontalHeader.append("ID");
//    horizontalHeader.append("Имя");
//    horizontalHeader.append("Возраст");

//    model->setHorizontalHeaderLabels(horizontalHeader);

//    //счётчик столбцов
//    int count = 0;

//    //Выводим значение из запроса
//    while (query.next())
//    {
//          //получяем ячейку из бд
//          QString _id = query.value(0).toString();
//          //создаём объект Item для ViewTable
//          item = new QStandardItem(_id);
//          //устанавляиваем выравнивание текста по центру
//          item->setTextAlignment(Qt::AlignCenter);
//          //добавляем ячейку в модель
//          model->setItem(count, 0, item);

//          QString name = query.value(1).toString();
//          item = new QStandardItem(name);
//          item->setTextAlignment(Qt::AlignCenter);
//          model->setItem(count, 1, item);

//          QString age = query.value(2).toString();
//          item = new QStandardItem(age);
//          item->setTextAlignment(Qt::AlignCenter);
//          model->setItem(count, 2, item);

//          count++;
//    }
//    //устанавливаем нашу модель в ViewTable
//    ui->tableView->setModel(model);



   // model->setData(index.column(0), )
    //ui->tableView->resizeColumnsToContents();

//    //обнуляем счётчик
//    count = 0;
}


void MainWindow::on_pbRemove_clicked()
{
    if(ui->tableView->currentIndex().isValid())
    {
        //получаем выделенную строку и столбец
        int row = ui->tableView->currentIndex().row();
        int column = ui->tableView->currentIndex().column();

        qDebug() << model->record(row).value(column) << "X";
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


void MainWindow::on_pbAdd_clicked()
{
    QStringList mask = ui->leDate->text().split(".");
    qDebug() << mask[0].toInt();

    if (mask[0].toInt()) {
        //запрос на добавление в БД
        QSqlQuery query;
        bool flag = query.exec("INSERT INTO People (name, age) values('" +
                   ui->leName->text() + "','" +
                   ui->leAge->text() + "')");
        qDebug() << flag;
        //обновляем таблицу
        model->select();
        //очищаем наши поля
        ui->leName->clear();
        ui->leAge->clear();
    }

}

void MainWindow::on_pbChangeRecord_clicked()
{
    //создание нового окна
    form = new DialogChange();
    //отображение нового окна
    form->show();
    //вывод полей выделенной записи в форму для редактирования
    form->setDB(model, ui->tableView->currentIndex().row());
}

void MainWindow::on_pushButton_clicked()
{
    QString strStream;
    QTextStream out(&strStream);

    const int rowCount = ui->tableView->model()->rowCount();
    const int columnCount = ui->tableView->model()->columnCount();
    QString strTitle = "Тестовая таблица";

    out <<  "<html>\n"
        "<head>\n"
        "<meta Content=\"Text/html; charset=Windows-1251\">\n"
        <<  QString("<title>%1</title>\n").arg(strTitle)
        <<  "</head>\n"
        "<body bgcolor=#ffffff link=#5000A0>\n"
        "<table border=1 cellspacing=0 cellpadding=2 width='400'>\n";

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
                QString data = ui->tableView->model()->data(ui->tableView->model()->index(row, column)).toString().simplified();
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
