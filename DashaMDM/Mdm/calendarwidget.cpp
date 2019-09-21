#include "calendarwidget.h"
#include <QPainter>
#include <QDebug>

CalendarWidget::CalendarWidget()
{
}

void CalendarWidget::addMeetDate(const QDate &date, QStringList list)
{
    qDebug() << date.toString();
    map.insert(date, list);
    updateCell(date);
}

void CalendarWidget::paintCell(QPainter *painter, const QRect &rect, const QDate &date) const
{
    QFont font;
    font.setPixelSize(12);
    if((map.contains(date)) && (map[date].value(0).toInt() == 0)){
               painter->save();
               painter->setFont(font);
               painter->setBrush(Qt::green);
               painter->drawRect(rect);
               painter->drawText(rect, Qt::AlignHCenter|Qt::TextSingleLine, QString(map[date].value(1)));
               font.setPixelSize(16);
               painter->setFont(font);
               painter->drawText(rect, Qt::AlignCenter|Qt::TextSingleLine, QString::number(date.day()));
               painter->restore();
               return;
    }else if (date == QDate::currentDate()) {
        painter->save();
        painter->setBrush(Qt::yellow);
        painter->drawRect(rect);
        painter->drawText(rect, Qt::AlignCenter|Qt::TextSingleLine, QString::number(date.day()));
        painter->restore();
        return;
    } else
        QCalendarWidget::paintCell(painter, rect, date);
}
