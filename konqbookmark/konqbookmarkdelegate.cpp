/*
    Copyright (c) 2009 Eduardo Robles Elvira <edulix@gmail.com>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#include "konqbookmarkdelegate.h"

#include <QtGui>
#include <QString>
#include <klineedit.h> 

using namespace Akonadi;

KonqBookmarkDelegate::KonqBookmarkDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

QWidget *KonqBookmarkDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */,
    const QModelIndex &/* index */) const
{
    KLineEdit *editor = new KLineEdit(parent);

    return editor;
}

void KonqBookmarkDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    QString text = index.model()->data(index, Qt::EditRole).toString();

    KLineEdit *kLineEditEditor = static_cast<KLineEdit*>(editor);
    kLineEditEditor->setText(text);
}

void KonqBookmarkDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const
{
    KLineEdit *kLineEditEditor = static_cast<KLineEdit*>(editor);
    QString text = kLineEditEditor->text();

    model->setData(index, text, Qt::EditRole);
}

void KonqBookmarkDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
