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

#ifndef KONQBOOKMARKDELEGATE_H
#define KONQBOOKMARKDELEGATE_H

#include "konqbookmark_export.h"
#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>

namespace Akonadi
{
    /**
     * Delegate used in the bookmarks editor for editing the bookmarks.
     */
    class KONQBOOKMARK_EXPORT KonqBookmarkDelegate : public QItemDelegate
    {
    public:
        KonqBookmarkDelegate(QObject *parent = 0);

        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                            const QModelIndex &index) const;

        void setEditorData(QWidget *editor, const QModelIndex &index) const;
        void setModelData(QWidget *editor, QAbstractItemModel *model,
                        const QModelIndex &index) const;

        void updateEditorGeometry(QWidget *editor,
            const QStyleOptionViewItem &option, const QModelIndex &index) const;
    };
}

#endif // KONQBOOKMARKDELEGATE_H
