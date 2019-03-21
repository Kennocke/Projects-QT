#include "itemdelegate.h"

#include <QLineEdit>
#include <QComboBox>
#include <QDebug>
#include <QTableView>
#include <QDate>

ItemDelegate::ItemDelegate()
{
}

QWidget *ItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QLineEdit *editor = new QLineEdit(parent);
    QTableView *tab = qobject_cast<QTableView *>(parent->parent());
    if ((index.column() == 2)) {
        QRegularExpression rx("^[а-яА-ЯёЁa-zA-Z ]+$");
        QValidator *validator = new QRegularExpressionValidator(rx);
        editor->setValidator(validator);
        if (tab->accessibleName() == "AdminTable") {
            qDebug() << "Сработало!";
            return editor;
        } else {
            qDebug() << "Не сработало!";
            return 0;
        }
    } else if (index.column() == 3) {
        editor->setInputMask("9999-999999");
        if (tab->accessibleName() == "AdminTable") {
            qDebug() << "Сработало!";
            return editor;
        } else {
            qDebug() << "Не сработало!";
            return 0;
        }
    } else if (index.column() == 4) {
        editor->setValidator(new QDoubleValidator(1, 16777215, 2));
        if (tab->accessibleName() == "AdminTable") {
            qDebug() << "Сработало!";
            return editor;
        } else {
            qDebug() << "Не сработало!";
            return 0;
        }
    } else if (index.column() == 5) {
        editor->setValidator(new QDoubleValidator(1, 16777215, 2));
        if (tab->accessibleName() == "AdminTable") {
            qDebug() << "Сработало!";
            return editor;
        } else {
            qDebug() << "Не сработало!";
            return 0;
        }
    } else if (index.column() == 6) {
        QRegularExpression rxDate("^(0[1-9]|[12][0-9]|3[01])[- /.](0[1-9]|1[012])[- /.](19|20)[0-9][0-9]$");
        QValidator *validator = new QRegularExpressionValidator(rxDate);
        editor->setValidator(validator);
        if (tab->accessibleName() == "AdminTable") {
            qDebug() << "Сработало!";
            return editor;
        } else {
            qDebug() << "Не сработало!";
            return 0;
        }
    } else if (index.column() == 7) {
        QRegularExpression rxDate("^(0[1-9]|[12][0-9]|3[01])[- /.](0[1-9]|1[012])[- /.](19|20)[0-9][0-9]$");
        QValidator *validator = new QRegularExpressionValidator(rxDate);
        editor->setValidator(validator);
        if (tab->accessibleName() == "AdminTable") {
            qDebug() << "Сработало!";
            return editor;
        } else {
            qDebug() << "Не сработало!";
            return 0;
        }
    } else if (index.column() == 8) {
        if (index.data(Qt::EditRole).toString() == "Закрыт") {
            return 0;
        } else {
            QComboBox *cb = new QComboBox(parent);
            cb->addItem(QString("Открыт"));
            cb->addItem(QString("Закрыт"));
            return cb;
        }
    }
}

void ItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (index.column() == 8) {
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

void ItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (index.column() == 8) {
        QComboBox *cb = qobject_cast<QComboBox *>(editor);
        Q_ASSERT(cb);
        model->setData(index, cb->currentText(), Qt::EditRole);
        if (index.data(Qt::EditRole).toString() == "Закрыт") {
            const QModelIndex header = index.sibling(index.row(), index.column()-1);
            model->setData(header, QDate::currentDate().toString(Qt::LocaleDate), Qt::EditRole);
        }
    } else {
        QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
        if (!edit)
            return;
        model->setData(index, edit->text());
    }
}
