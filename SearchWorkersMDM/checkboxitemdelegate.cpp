#include "checkboxitemdelegate.h"

#include <QComboBox>
#include <QLineEdit>
#include <QCompleter>
#include <QRegExpValidator>
#include "mainwindow.h"

CheckBoxItemDelegate::CheckBoxItemDelegate()
{
}

QWidget *CheckBoxItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QLineEdit *editor = new QLineEdit(parent);
    QTableView *tab = qobject_cast<QTableView *>(parent->parent());
    if ((index.column() == 2) || (index.column() == 3)) {
        QRegularExpression rx("^[а-яА-ЯёЁa-zA-Z]+$");
        QValidator *validator = new QRegularExpressionValidator(rx);
        editor->setValidator(validator);
        return 0;
    } else if (index.column() == 4) {
        editor->setValidator(new QIntValidator(1, 16777215));
        return 0;
    } else if (index.column() == 7){
        QRegularExpression rxDate("^(0[1-9]|[12][0-9]|3[01])[- /.](0[1-9]|1[012])[- /.](19|20)[0-9][0-9]$");
        QValidator *validator = new QRegularExpressionValidator(rxDate);
        editor->setValidator(validator);
                return 0;
    } else if (index.column() == 8) {
        QRegularExpression rxDate("^(0[1-9]|[12][0-9]|3[01])[- /.](0[1-9]|1[012])[- /.](19|20)[0-9][0-9]$");
        QValidator *validator = new QRegularExpressionValidator(rxDate);
        editor->setValidator(validator);
                return 0;
    } else if (index.column() == 6) {
        QComboBox *cb = new QComboBox(parent);
        cb->addItem(QString("Муж"));
        cb->addItem(QString("Жен"));
        return 0;
    } else {
        editor->setValidator(new QDoubleValidator(1, 16777215, 2));
        return 0;
    }
}

void CheckBoxItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if ((index.column() == 6)) {
        QComboBox *cb = qobject_cast<QComboBox *>(editor);
        Q_ASSERT(cb);
        const QString currentText = index.data(Qt::EditRole).toString();
        const int cbIndex = cb->findText(currentText);
        if (cbIndex >= 0)
            cb->setCurrentIndex(cbIndex);
    } else {
        QString value = index.model()->data(index).toString(); //получаем что уже есть в модели
        QLineEdit *fileDialog = static_cast<QLineEdit*> (editor);
        fileDialog->setText(value);
    }
}

void CheckBoxItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if ((index.column() == 6)) {
        QComboBox *cb = qobject_cast<QComboBox *>(editor);
        Q_ASSERT(cb);
        model->setData(index, cb->currentText(), Qt::EditRole);
    } else {
        QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
        if (!edit)
            return;
        model->setData(index, edit->text());
    }
}
