#ifndef NONEDITTABLECOLUMNDELEGATE_H
#define NONEDITTABLECOLUMNDELEGATE_H

#include <QItemDelegate>


class NonEditTableColumnDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    NonEditTableColumnDelegate();
    virtual QWidget * createEditor ( QWidget *, const QStyleOptionViewItem &,
                                     const QModelIndex &) const
    {
        return 0;
    }
};

#endif // NONEDITTABLECOLUMNDELEGATE_H
