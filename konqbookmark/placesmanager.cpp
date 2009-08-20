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
#include "itemmodels/konqbookmarkmodel.h"
#include "itemmodels/kcompletionmodel.h"

#include <konq_historyentry.h>
#include <kurlcompletion.h>
#include <kcompletion.h>
#include <kglobal.h>

#include <akonadi/monitor.h>
#include <akonadi/session.h>

#include <QByteArray>

using namespace Konqueror;
using namespace Akonadi;

PlacesManager* PlacesManager::s_self = 0;

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

    void slotBookmarksInserted(const QModelIndex& parent, int start, int end);
    void slotBookmarksRemoved(const QModelIndex& parent, int start, int end);
    
    void slotUrlsInserted(const QModelIndex& parent, int start, int end);
    void slotUrlsRemoved(const QModelIndex& parent, int start, int end);
    
    void slotHistoryEntryAdded(const KonqHistoryEntry &entry);
    void slotHistoryEntryRemoved(const KonqHistoryEntry &entry);
    bool isFolder(const QModelIndex& index);
    void slotHistoryCleared();
    
    PlacesManager *q;
    
    KonqBookmarkModel *m_bookmarksModel;
    KCompletionModel *m_urlCompletionModel;
    
    QHash<QUrl, KonqHistoryEntry*> m_historyEntries;
    QHash<QUrl, KonqBookmark*> m_bookmarks;
    QHash<QUrl, Place*> m_places;
};

PlacesManager::Private::Private(PlacesManager *parent)
    : q(parent)
{   
    Session* session = new Session(QByteArray( "PlacesManager-" ) + QByteArray::number( qrand() ), q);
    Monitor* monitor = new Monitor( q );
    m_bookmarksModel = new KonqBookmarkModel( session, monitor, q );
    
    connect(m_bookmarksModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
        q, SLOT(slotBookmarksInserted(const QModelIndex&, int, int)));
        
    connect(m_bookmarksModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
        q, SLOT(slotBookmarksRemoved(const QModelIndex&, int, int)));
        
    // TODO modelReset
    m_urlCompletionModel = new KCompletionModel(q);
        
    connect(m_urlCompletionModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
        q, SLOT(slotUrlsInserted(const QModelIndex&, int, int)));
        
    connect(m_urlCompletionModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
        q, SLOT(slotUrlsRemoved(const QModelIndex&, int, int)));
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

void PlacesManager::Private::slotBookmarksInserted(const QModelIndex& parent, int start, int end)
{
    for(int i = start; i <= end; i++) {
        QModelIndex index = m_bookmarksModel->index(i, KonqBookmarkModel::UniqueUri, parent);
        if(isFolder(index)) {
            continue;
        }
        
        KonqBookmark *konqBookmark = new KonqBookmark(index.data().toString());
        m_bookmarks[konqBookmark->url()] = konqBookmark;
        
        kDebug() << konqBookmark->url() << konqBookmark->title();
        
        // Update/insert place
        Place *place = q->place(konqBookmark->url());
        if(place) {
            place->setBookmark(konqBookmark);
        }
    }
}

void PlacesManager::Private::slotBookmarksRemoved(const QModelIndex& parent, int start, int end)
{
    for(int i = start; i <= end; i++) {
        QModelIndex index = m_bookmarksModel->index(i, KonqBookmarkModel::Url, parent);
        if(isFolder(index)) {
            continue;
        }
        QUrl url(index.data().toString());
        if(m_bookmarks.contains(url)) {
            
            // Update/remove place
            Place *place = q->place(url);
            
            Q_ASSERT(place != 0);
            
            // if history entry is null and the url is not set either, just 
            // remove this place (otherwise we would end up with a place with 
            // both bookmark and history entry unset). Otherwise, we just update the place.
            m_places[url]->setBookmark(0);
            if(!place->historyEntry() && place->url().isEmpty()) {
                delete m_places[url];
                m_places.remove(url);
            }
            
            delete m_bookmarks[url];
            m_bookmarks.remove(url);
        }
    }
}

void PlacesManager::Private::slotUrlsInserted(const QModelIndex& parent, int start, int end)
{
    for(int i = start; i <= end; i++) {
        QModelIndex index = parent.child(i, 0);
        if(isFolder(index)) {
            continue;
        }
        
        QUrl url = index.data().toString();
        
        // Update/insert place
        Place *place = q->place(url);
        if(!place) {
            m_places[url] = new Place(url, q);
        }
    }
}

void PlacesManager::Private::slotUrlsRemoved(const QModelIndex& parent, int start, int end)
{
    for(int i = start; i <= end; i++) {
        QModelIndex index = parent.child(i, 0);
        if(isFolder(index)) {
            continue;
        }
        
        QUrl url = index.data().toString();
        
        // Remove
        Place *place = q->place(url);
        if(place && !place->bookmark() && !place->historyEntry()) {
            delete m_places[url];
            m_places.remove(url);
        }
    }
}


void PlacesManager::Private::slotHistoryEntryAdded(const KonqHistoryEntry &entry)
{
    KonqHistoryEntry *historyEntry = new KonqHistoryEntry(entry);
    m_historyEntries[entry.url] = historyEntry;
    
    if(m_places.contains(entry.url)) {
        m_places[entry.url]->setHistoryEntry(historyEntry);
    }
}

void PlacesManager::Private::slotHistoryEntryRemoved(const KonqHistoryEntry &entry)
{
    if(!m_historyEntries.contains(entry.url)) {
        return;
    }
    
    // If the history entry is registered, the related place should be there too
    Q_ASSERT(m_places.contains(entry.url));
    
    delete m_historyEntries[entry.url];
    m_historyEntries.remove(entry.url);
    
    if(m_places.contains(entry.url) &&  !m_bookmarks[entry.url]
        && m_places[entry.url]->url() == QUrl())
    {
        delete m_places[entry.url];
        m_places.remove(entry.url);
    } else {
        m_places[entry.url]->setHistoryEntry(0);
    }
}

void PlacesManager::Private::slotHistoryCleared()
{
    foreach(KonqHistoryEntry* entry, m_historyEntries) {
        slotHistoryEntryRemoved(*entry);
    }
}
//@endcond

PlacesManager::PlacesManager()
    : QObject(0), d(new Private(this))
{
    
}

PlacesManager::~PlacesManager()
{
    delete d;
}

PlacesManager* PlacesManager::self()
{
    if(s_self) {
        return s_self;
    }
    
    s_self = new PlacesManager();
    
    return s_self;
}

KonqBookmarkModel* PlacesManager::bookmarkModel()
{
    return d->m_bookmarksModel;
}

KCompletionModel* PlacesManager::urlCompletionModel()
{
    return d->m_urlCompletionModel;
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
 
    QHash<QUrl, KonqHistoryEntry *> myhash;
    myhash[QUrl()] = 0;
    
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
    
    // Create an "orphan" place (with no history entry o konqBookmark attached)
    Place *place = new Place(url);
    d->m_places[url] = place;
    
    return place;
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
    Q_UNUSED(url);
    
    return 0;
}

QIcon* PlacesManager::icon(const KonqBookmark* konqBookmark)
{
    if(!konqBookmark) {
        return 0;
    }
    
    return icon(konqBookmark->url());
}

QIcon* PlacesManager::icon(const KonqHistoryEntry* historyEntry)
{
    if(!historyEntry) {
        return 0;
    }
    
    return icon(historyEntry->url);
}

QIcon* PlacesManager::icon(const Place* place)
{
    if(!place) {
        return 0;
    }
    
    return icon(place->url());
}


void PlacesManager::setSelf(PlacesManager *manager)
{
    s_self = manager;
}

bool PlacesManager::hasInstance()
{
    return s_self != 0;
}

#include "placesmanager.moc"
