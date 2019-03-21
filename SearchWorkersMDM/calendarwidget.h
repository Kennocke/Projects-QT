#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

#include <QCalendarWidget>
#include <QtGui>

class CalendarWidget : public QCalendarWidget
{
public:
    CalendarWidget();
    void addMeetDate(const QDate &date, QStringList list);
    void paintCell(QPainter *painter, const QRect &rect, const QDate &date) const;
private:
    QMap<QDate, QStringList> map;
};

#endif // CALENDARWIDGET_H
