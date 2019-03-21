#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QtSql/QSqlDatabase>
#include <QFileDialog>
#include <QTime>
#include <QSqlTableModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pbStart_clicked();

    void on_pbCommon_clicked();

    void on_pbAnswer_clicked();

    void on_pbAddTest_clicked();

    void on_pbAddPath_clicked();

    void on_pbAddQuestion_clicked();

private:
    Ui::MainWindow *ui;

    //Список вопросов
    QList<QPixmap> listQuestion;

    //Список ответов
    QMap<int, QStringList> mapAnswer;

    //переменная для бд
    QSqlDatabase db;

    //колличество вопросов
    int sizeTest = 0;

    //текущий вопрос
    int currentAnswer = 0;

    //номер верного ответа на данном этапе
    int trueAnswer = 0;

    //счётчик верных ответов
    int countTrueAnswer = 0;

    //функция создания теста
    void createTest(QString index);

    //функция переключения вопроса
    void nextQuestion();

    //функция заполнения выпадающих списков
    void insertCB();

    //для окна выбора пути
    QFileDialog dialog;

    //переменная - таймер
    QTimer *timer;
    QTime time;

    //модель для результатов
    QSqlTableModel *model;

private slots:
    void slotTimerAlarm();
    void on_pushButton_clicked();
    void on_pbExit_clicked();
    void on_pbBackTheory_clicked();
    void on_pbExtraExit_clicked();
    void on_pbTheory_clicked();
    void on_pbCancel_clicked();
    void on_pbAddTheory_clicked();
    void on_pbLogin_clicked();
    void on_pbExitAdmin_clicked();
    void on_pbDeleteTheme_clicked();
    void on_pbResult_clicked();
    void on_pbBackMenu_clicked();
};

#endif // MAINWINDOW_H
