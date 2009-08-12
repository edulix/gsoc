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
#include <QUrl>
#include <QString>
#include <QIcon>
#include <QStringList>
#include <QDateTime>

class KonqHistoryEntry;
class KonqBookmark;

namespace Akonadi {
    class KonqBookmarkModel;
}

namespace Konqueror
{
    class Place;
    
    /**
     * @brief Class that provides access to places related information.
     */
    class PlacesManager : public QObject
    {
        Q_OBJECT
        
    public:
        PlacesManager(QObject* parent = 0);
        virtual ~PlacesManager();
        
        virtual Akonadi::KonqBookmarkModel* bookmarkModel();
        
        virtual KonqBookmark* bookmark(const QUrl& url);
        virtual KonqBookmark* bookmark(const KonqHistoryEntry* historyEntry);
        virtual KonqBookmark* bookmark(const Place* place);
        
        virtual KonqHistoryEntry* historyEntry(const QUrl& url);
        virtual KonqHistoryEntry* historyEntry(const KonqBookmark* konqBookmark);
        virtual KonqHistoryEntry* historyEntry(const Place* place);
        
        virtual Place* place(const QUrl& url);
        virtual Place* place(const KonqBookmark* konqBookmark);
        virtual Place* place(const KonqHistoryEntry* historyEntry);
        
        virtual QIcon* icon(const QUrl& url);
        virtual QIcon* icon(const KonqBookmark* konqBookmark);
        virtual QIcon* icon(const KonqHistoryEntry* historyEntry);
        virtual QIcon* icon(const Place* place);
        
    private:
        class Private;
        Private* const d;
    };
}
#endif // KONQUEROR_PLACESMANAGER_H
