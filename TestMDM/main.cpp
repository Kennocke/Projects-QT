#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    // Создаём палитру для тёмной темы оформления
    // Создаём палитру для тёмной темы оформления
        QPalette darkPalette;

        // Настраиваем палитру для цветовых ролей элементов интерфейса
        darkPalette.setColor(QPalette::Window, QColor(255, 215, 158));
        darkPalette.setColor(QPalette::WindowText, Qt::black);
        darkPalette.setColor(QPalette::Base, Qt::white);
        darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ToolTipBase, Qt::black);
        darkPalette.setColor(QPalette::ToolTipText, Qt::black);
        darkPalette.setColor(QPalette::Text, Qt::black);
        darkPalette.setColor(QPalette::Button, Qt::yellow);
        darkPalette.setColor(QPalette::ButtonText, Qt::black);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);

        // Устанавливаем данную палитру
        qApp->setPalette(darkPalette);

    w.show();

    return a.exec();
}
