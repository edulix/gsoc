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

    void rowsInserted(const QModelIndex& parent, int start, int end);
    void rowsRemoved(const QModelIndex& parent, int start, int end);
    bool isFolder(const QModelIndex& index);
        
    PlacesManager *q;
    
    KonqBookmarkModel *m_bookmarkModel;
    
    QHash<QUrl, KonqHistoryEntry*> m_historyEntries;
    QHash<QUrl, KonqBookmark*> m_bookmarks;
    QHash<QUrl, Place*> m_places;
};

PlacesManager::Private::Private(PlacesManager* parent)
    : q(parent), m_bookmarkModel(0)
{
    Session* session = new Session(QByteArray( "PlacesManager-" ) + QByteArray::number( qrand() ), q);
    Monitor* monitor = new Monitor( q );
    m_bookmarkModel = new KonqBookmarkModel( session, monitor, q );
    
    connect(m_bookmarkModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
        q, SLOT(rowsInserted(const QModelIndex&, int, int)));
        
    connect(m_bookmarkModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
        q, SLOT(rowsRemoved(const QModelIndex&, int, int)));
}

PlacesManager::Private::~Private()
{
    
}

bool PlacesManager::Private::isFolder(const QModelIndex& index)
{
    if (!index.isValid())
        return false;
    
    Collection collection =  qVariantValue<Collection>(index.data(EntityTreeModel::CollectionRole));
    return collection.isValid();
}

void PlacesManager::Private::rowsInserted(const QModelIndex& parent, int start, int end)
{
    for(int i = start; i <= end; i++) {
        QModelIndex index = parent.child(i, 0);
        if(isFolder(index)) {
            continue;
        }
        
        KonqBookmark *konqBookmark = new KonqBookmark(index.data(KonqBookmarkModel::UniqueUri).toString());
        m_bookmarks[konqBookmark->url()] = konqBookmark;
    }
}

void PlacesManager::Private::rowsRemoved(const QModelIndex& parent, int start, int end)
{
    for(int i = start; i <= end; i++) {
        QModelIndex index = parent.child(i, 0);
        if(isFolder(index)) {
            continue;
        }
        QUrl url(index.data(KonqBookmarkModel::Url).toString());
        if(m_bookmarks.contains(url)) {
            delete m_bookmarks[url];
            m_bookmarks.remove(url);
        }
    }
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
    if(d->m_bookmarks.contains(url))
        return d->m_bookmarks[url];
    
    return 0;
}

KonqBookmark* PlacesManager::bookmark(const KonqHistoryEntry* historyEntry)
{
    if(!historyEntry) {
        return 0;
    }
    
    return bookmark(historyEntry->url);
}

KonqHistoryEntry* PlacesManager::historyEntry(const QUrl& url)
{
    if(d->m_historyEntries.contains(url)) {
        return d->m_historyEntries[url];
    }
    
    return 0;
}

KonqHistoryEntry* PlacesManager::historyEntry(const KonqBookmark* konqBookmark)
{
    if(!konqBookmark) {
        return 0;
    }
    
    return historyEntry(konqBookmark->url());
}

Place* PlacesManager::place(const QUrl& url)
{
    if(d->m_places.contains(url)) {
        return d->m_places[url];
    }
    
    return 0;
}

Place* PlacesManager::place(const KonqBookmark* konqBookmark)
{
    if(!konqBookmark) {
        return 0;
    }
    
    return place(konqBookmark->url());
}

Place* PlacesManager::place(const KonqHistoryEntry* historyEntry)
{
    if(!historyEntry) {
        return 0;
    }
    
    return place(historyEntry->url);
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
