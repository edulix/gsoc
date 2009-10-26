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

#ifndef KONQBOOKMARK_MODEL_H
#define KONQBOOKMARK_MODEL_H

#include "konqbookmark_export.h"
#include <akonadi/entitytreemodel.h>
#include <akonadi/itemmodel.h>
#include <QModelIndex>

class KonqBookmark;

namespace Akonadi
{
    
    class ChangeRecorder;
    class Session;

    /**
     * @brief Main konqueror bookmarks model.
     *
     * This is the main bookmarks model, used for example for the Bookmarks
     * Organizer.
     */
    class KONQBOOKMARK_EXPORT KonqBookmarkModel : public EntityTreeModel
    {
        Q_OBJECT
    public:
        enum Column
        {
            Title,
            Url,
            UniqueUri,
            Tags,
            Description,
            NumVisits,
            Created,
            LastModified,
            LastVisited
        };
        enum { ColumnCount =  9 };

        KonqBookmarkModel(Akonadi::Session *session, Akonadi::ChangeRecorder *monitor, QObject *parent = 0);

        virtual ~KonqBookmarkModel();

        virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
        virtual QVariant entityHeaderData(int section, Qt::Orientation orientation, int role, HeaderGroup headerGroup) const;
        
        // We let the delegate know that we are editable
        Qt::ItemFlags flags(const QModelIndex &index) const;
        
        // With this function we make the model editable
        bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
        
        /**
         * Instead of reimplementing data() function, we reimplement getData() for items in this case,
         * and also for collections in the next function. EntityTreeModel::data() will then call these
         * functions.
         */
        virtual QVariant entityData(const Item &item, int column, int role = Qt::DisplayRole) const;
        virtual QVariant entityData(const Collection &collection, int column, int role = Qt::DisplayRole) const;
    
        bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());
        
        // Drag & drop related functions
        bool dropMimeData(const QMimeData *data, Qt::DropAction action,
            int row, int column, const QModelIndex &parent);
        QMimeData *mimeData(const QModelIndexList &indexes) const;
        QStringList mimeTypes() const;
        QString mimeType() const;
        Qt::DropActions supportedDropActions() const;
    private:
        class Private;
        Private* const d;
    };

}

#endif // KONQ_BOOKMARK_MODEL_H
