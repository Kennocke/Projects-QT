#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QDebug>
#include <QTextCodec>

int main(int argc, char *argv[])
{

    QTextCodec *utfcodec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(utfcodec);
    QApplication a(argc, argv);
    MainWindow w;
    // Создаём палитру для тёмной темы оформления
        QPalette darkPalette;

         qApp->setStyle(QStyleFactory::create("Fusion"));
        // Настраиваем палитру для цветовых ролей элементов интерфейса
        darkPalette.setColor(QPalette::Button, QColor(225, 242, 245));
        // Устанавливаем данную палитру
        qApp->setPalette(darkPalette);
    w.show();

    return a.exec();
}
