#ifndef CHECKBOXITEMDELEGATE_H
#define CHECKBOXITEMDELEGATE_H

#include <QStyledItemDelegate>

class CheckBoxItemDelegate : public QStyledItemDelegate
{
public:
    CheckBoxItemDelegate();

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

};

#endif // CHECKBOXITEMDELEGATE_H
