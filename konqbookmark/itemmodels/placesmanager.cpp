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

#include "placesmanager.h"
#include "place.h"
#include "konqbookmark.h"
#include "konqbookmarkmodel.h"

#include <konq_historyentry.h>

#include <akonadi/monitor.h>
#include <akonadi/session.h>

#include <QHash>
#include <QUrl>
#include <QByteArray>

using namespace Konqueror;
using namespace Akonadi;

/**
 * Private class that helps to provide binary compatibility between releases.
 * @internal
 */
//@cond PRIVATE
class PlacesManager::Private
{
public:
    Private(PlacesManager* parent);
    ~Private();
    
    PlacesManager *q;
    
    KonqBookmarkModel *m_bookmarkModel;
    
    QHash<QUrl, KonqHistoryEntry> m_historyEntries;
    QHash<QUrl, KonqBookmark> m_bookmarks;
    QHash<QUrl, Place> m_places;
};

PlacesManager::Private::Private(PlacesManager* parent)
    : q(parent), m_bookmarkModel(0)
{
    Akonadi::Session* session = new Akonadi::Session(QByteArray( "PlacesManager-" ) + QByteArray::number( qrand() ), q);
    Akonadi::Monitor* monitor = new Akonadi::Monitor( q );
    m_bookmarkModel = new KonqBookmarkModel( session, monitor, q );
}

PlacesManager::Private::~Private()
{

}
//@endcond

PlacesManager::PlacesManager(QObject* parent)
    : QObject(parent), d(new Private(this))
{
    
}

PlacesManager::~PlacesManager()
{
    delete d;
}

KonqBookmarkModel* PlacesManager::bookmarkModel()
{
    return d->m_bookmarkModel;
}

KonqBookmark* PlacesManager::bookmark(const QUrl& url)
{
    return 0;
}

KonqBookmark* PlacesManager::bookmark(const KonqHistoryEntry* historyEntry)
{
    return 0;
}

KonqBookmark* PlacesManager::bookmark(const Place* place)
{
    return 0;
}

KonqHistoryEntry* PlacesManager::historyEntry(const QUrl& url)
{
    return 0;
}

KonqHistoryEntry* PlacesManager::historyEntry(const KonqBookmark* konqBookmark)
{
    return 0;
}

KonqHistoryEntry* PlacesManager::historyEntry(const Place* place)
{
    return 0;
}

Place* PlacesManager::place(const QUrl& url)
{
    return 0;
}

Place* PlacesManager::place(const KonqBookmark* konqBookmark)
{
    return 0;
}

Place* PlacesManager::place(const KonqHistoryEntry* historyEntry)
{
    return 0;
}

QIcon* PlacesManager::icon(const QUrl& url)
{
    return 0;
}

QIcon* PlacesManager::icon(const KonqBookmark* konqBookmark)
{
    return 0;
}

QIcon* PlacesManager::icon(const KonqHistoryEntry* historyEntry)
{
    return 0;
}

QIcon* PlacesManager::icon(const Place* place)
{
    return 0;
}

#include "placesmanager.moc"
