/*
    This file is part of the KDE project
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

#ifndef KONQUEROR_PLACESMANAGER_H
#define KONQUEROR_PLACESMANAGER_H

#include "konqbookmark_export.h"

#include <QObject>
#include <QHash>
#include <QUrl>
#include <QString>
#include <QIcon>
#include <QStringList>
#include <QDateTime>
#include <QAbstractItemModel>

class KonqHistoryEntry;
class KonqBookmark;
class KCompletionModel;

namespace Akonadi {
    class KonqBookmarkModel;
}

namespace Konqueror
{
    class Place;
    
    /**
     * @brief Class that provides access to places related information.
     * 
     * This is a singleton class, so to access to it you should call to
     * @p self(); the PlacesManager will be created the first time you call it:
     * 
     * Inherit from this class if you want the history entries to be filled or
     * if you want to provide an icon for items (hould just reimplement @p icon(QUrl).
     * 
     * @code
     * PlacesManager::self();
     * @endcode
     */
    class KONQBOOKMARK_EXPORT PlacesManager : public QObject
    {
        Q_OBJECT
    public:
        friend class Private;
        class Private;
        
        static PlacesManager* self();
        
        virtual Akonadi::KonqBookmarkModel* bookmarkModel();
        virtual KCompletionModel* urlCompletionModel();
        virtual QAbstractItemModel* historyEntriesModel() { return 0; }
        
        KonqBookmark* bookmark(const QUrl& url);
        KonqBookmark* bookmark(const KonqHistoryEntry* historyEntry);
        
        KonqHistoryEntry* historyEntry(const QUrl& url);
        KonqHistoryEntry* historyEntry(const KonqBookmark* konqBookmark);
        
        Place* place(const QUrl& url);
        Place* place(const KonqBookmark* konqBookmark);
        Place* place(const KonqHistoryEntry* historyEntry);
        
        virtual QIcon* icon(const QUrl& url);
        virtual QIcon* icon(const KonqBookmark* konqBookmark);
        virtual QIcon* icon(const KonqHistoryEntry* historyEntry);
        virtual QIcon* icon(const Place* place);
        
    protected:
        PlacesManager();
        virtual ~PlacesManager();
        
        /**
         * If you create a custom PlacesManager and want self() to refer to it,
         * call to setSelf(this) in the constructor of you custom PlacesManager.
         */
        static void setSelf(PlacesManager *manager);
        static bool hasInstance();
        
    private:
        static PlacesManager *s_self;
        Private* const d;
        
        Q_PRIVATE_SLOT(d, void slotBookmarksInserted(const QModelIndex&, int, int))
        Q_PRIVATE_SLOT(d, void slotBookmarksRemoved(const QModelIndex&, int, int))
        
        Q_PRIVATE_SLOT(d, void slotHistoryEntryAdded(const KonqHistoryEntry &))
        Q_PRIVATE_SLOT(d, void slotHistoryEntryRemoved(const KonqHistoryEntry &))
        Q_PRIVATE_SLOT(d, void slotHistoryCleared())
        
        Q_PRIVATE_SLOT(d, void slotUrlsInserted(const QModelIndex&, int, int))
        Q_PRIVATE_SLOT(d, void slotUrlsRemoved(const QModelIndex&, int, int))
    };
}
#endif // KONQUEROR_PLACESMANAGER_H
