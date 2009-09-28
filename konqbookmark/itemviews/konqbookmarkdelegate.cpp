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
#include <kdebug.h> 
#include "../itemmodels/konqbookmarkmodel.h" 
#include "locationbar.h"

using namespace Akonadi;
using namespace Konqueror;

KonqBookmarkDelegate::KonqBookmarkDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

QWidget *KonqBookmarkDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */,
    const QModelIndex &index) const
{
    if(index.column() == KonqBookmarkModel::Url) {
        // Edit the address with a locationbar
        LocationBar *editor = new LocationBar(parent);
        return editor;
    } else {
        KLineEdit *editor = new KLineEdit(parent);
        // Show some space at the left for the bookmarks' icon
        if (index.column() == KonqBookmarkModel::Title) {
            editor->setContentsMargins(24, 0, 0, 0);
        }
        return editor;
    }
}

void KonqBookmarkDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    QString text = index.model()->data(index, Qt::EditRole).toString();
    
    if(index.column() == KonqBookmarkModel::Url) {
        LocationBar *LocationBarEditor = qobject_cast<LocationBar*>(editor);
        LocationBarEditor->setText(text);
    } else {
        KLineEdit *kLineEditEditor = static_cast<KLineEdit*>(editor);
        kLineEditEditor->setText(text);
    }
}

void KonqBookmarkDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const
{
    QString text;
    if(index.column() == KonqBookmarkModel::Url) {
        LocationBar *LocationBarEditor = qobject_cast<LocationBar*>(editor);
        text = LocationBarEditor->text();
    } else {
        KLineEdit *kLineEditEditor = static_cast<KLineEdit*>(editor);
        text = kLineEditEditor->text();
    }

    model->setData(index, text, Qt::EditRole);
}

void KonqBookmarkDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    QRect geometry(option.rect.x()-2, option.rect.y()-2, option.rect.width()+4, option.rect.height()+4);
    editor->setGeometry(geometry);
}
