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

#include "konq_historyentry.h" // Imported from libkonq
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
    void slotBookmarksChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
    
    void slotUrlsInserted(const QModelIndex& parent, int start, int end);
    void slotUrlsRemoved(const QModelIndex& parent, int start, int end);
    
    void slotHistoryEntryAdded(const KonqHistoryEntry &entry);
    void slotHistoryEntryRemoved(const KonqHistoryEntry &entry);
        void slotHistoryCleared();
    
    PlacesManager *q;
    
    KonqBookmarkModel *m_bookmarksModel;
    
    QHash<QUrl, KonqHistoryEntry*> m_historyEntries;
    QHash<QUrl, KonqBookmark*> m_bookmarks;
    QHash<QUrl, QUrl> m_uniqueBookmarks; // QHash<uniqueUri(), url()>
    QHash<QUrl, Place*> m_places;
};

PlacesManager::Private::Private(PlacesManager *parent)
    : q(parent)
{   
    Session* session = new Session(QByteArray("PlacesManager-") + QByteArray::number(qrand()), q);
    Monitor* monitor = new Monitor(q);
    m_bookmarksModel = new KonqBookmarkModel(session, monitor, q);
    
    connect(m_bookmarksModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
        q, SLOT(slotBookmarksInserted(const QModelIndex&, int, int)));
        
    connect(m_bookmarksModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
        q, SLOT(slotBookmarksRemoved(const QModelIndex&, int, int)));
    
    connect(m_bookmarksModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
        q, SLOT(slotBookmarksChanged(const QModelIndex&, const QModelIndex&)));
}

PlacesManager::Private::~Private()
{
}

void PlacesManager::Private::slotBookmarksInserted(const QModelIndex& parent, int start, int end)
{
    for(int i = start; i <= end; i++) {
        QModelIndex index = m_bookmarksModel->index(i, KonqBookmarkModel::UniqueUri, parent);
        if(index.data().toString().isEmpty()) {
            continue;
        }
        KonqBookmark *konqBookmark = new KonqBookmark(index.data().toString());
        m_uniqueBookmarks[konqBookmark->uniqueUri()] = konqBookmark->url();
        if(konqBookmark->url().toString().isEmpty() || m_bookmarks.contains(konqBookmark->url())) {
            continue;
        }
        
        m_bookmarks[konqBookmark->url()] = konqBookmark;
        kDebug() << konqBookmark->url() << konqBookmark->uniqueUri();
        
        // Update/insert place
        q->place(konqBookmark->url())->setBookmark(konqBookmark);
    }
}

void PlacesManager::Private::slotBookmarksRemoved(const QModelIndex& parent, int start, int end)
{
    for(int i = start; i <= end; i++) {
        QModelIndex index = m_bookmarksModel->index(i, KonqBookmarkModel::Url, parent);
        
        if(index.data().toString().isEmpty()) {
            continue;
        }
        
        QUrl url(index.data().toString());
        if(m_bookmarks.contains(url)) {
            // Update/remove place
            Q_ASSERT(m_places[url] != 0);
            
            // if history entry is null and the url is not set either, just 
            // remove this place (otherwise we would end up with a place with 
            // both bookmark and history entry unset). Otherwise, we just update the place.
            m_places[url]->setBookmark(0);
            if(!m_places[url]->historyEntry() && m_places[url]->url().isEmpty()) {
                delete m_places[url];
                m_places.remove(url);
            }
            
            m_uniqueBookmarks.remove(m_bookmarks[url]->uniqueUri());
            delete m_bookmarks[url];
            m_bookmarks.remove(url);
        }
    }
}

void PlacesManager::Private::slotBookmarksChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    kDebug() << topLeft << bottomRight;
    int start = topLeft.row();
    int end = bottomRight.row();
    if(start > end || start < 0 || topLeft.parent() != bottomRight.parent()) {
        return;
    }
    QModelIndex parent = topLeft.parent();
    
    for(int i = start; i <= end; i++) {
        QModelIndex index = m_bookmarksModel->index(i, KonqBookmarkModel::UniqueUri, parent);
        
        kDebug() << index << index.data() << index.data().toString();
        if(index.data().toString().isEmpty() ||
            !m_uniqueBookmarks.contains(index.data().toString())) {
            continue;
        }
        KonqBookmark *konqBookmark = new KonqBookmark(index.data().toString());
        QUrl oldUrl = m_uniqueBookmarks[index.data().toString()];
        if(!oldUrl.isEmpty() && oldUrl == konqBookmark->url()) {
            delete konqBookmark;
            continue;
        }
        // 1. Remove old bookmark
        if(!oldUrl.isEmpty()) {
            Q_ASSERT(m_places[oldUrl] != 0);
            // if history entry is null and the url is not set either, just 
            // remove this place (otherwise we would end up with a place with 
            // both bookmark and history entry unset). Otherwise, we just update the place.
            m_places[oldUrl]->setBookmark(0);
            if(!m_places[oldUrl]->historyEntry() && m_places[oldUrl]->url().isEmpty()) {
                delete m_places[oldUrl];
                m_places.remove(oldUrl);
            }
            
            m_uniqueBookmarks[konqBookmark->uniqueUri()] = konqBookmark->url();
            m_bookmarks.remove(oldUrl);
        }
        
        // 2. Add new bookmark if needed
        if(konqBookmark->url().toString().isEmpty() ||
            m_bookmarks.contains(konqBookmark->url())) {
            delete konqBookmark;
            continue;
        }
        m_bookmarks[konqBookmark->url()] = konqBookmark;
        kDebug() << konqBookmark->url() << konqBookmark->title();
        
        // Update/insert place
        q->place(konqBookmark->url())->setBookmark(konqBookmark);
        
    }
}

void PlacesManager::Private::slotUrlsInserted(const QModelIndex& parent, int start, int end)
{
    KCompletionModel *urlCompletionModel = qobject_cast<KCompletionModel*>(q->sender());
    for(int i = start; i <= end; i++) {
        QModelIndex index = urlCompletionModel->index(i, 0, parent);
        
        QUrl url = index.data().toString();
        
        if(!url.isValid()) {
            continue;
        }
        
        // Update/insert place
        q->place(url);
    }
}

void PlacesManager::Private::slotUrlsRemoved(const QModelIndex& parent, int start, int end)
{
    KCompletionModel *urlCompletionModel = qobject_cast<KCompletionModel*>(q->sender());
    for(int i = start; i <= end; i++) {
        QModelIndex index = urlCompletionModel->index(i, 0, parent);
        QUrl url = index.data().toString();
        
        if(!url.isValid()) {
            continue;
        }
        
        // Remove
        if(m_places.contains(url) && !m_places[url]->bookmark() && !m_places[url]->historyEntry()) {
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

void PlacesManager::registerUrlCompletionModel(KCompletionModel* urlCompletionModel)
{
    connect(urlCompletionModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
        this, SLOT(slotUrlsInserted(const QModelIndex&, int, int)));
        
    connect(urlCompletionModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
        this, SLOT(slotUrlsRemoved(const QModelIndex&, int, int)));
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
    if (!historyEntry) {
        return 0;
    }
    
    return place(historyEntry->url);
}

QIcon PlacesManager::icon(const QUrl& url)
{
    Q_UNUSED(url);
    
    return QIcon();
}

QIcon PlacesManager::icon(const KonqBookmark* konqBookmark)
{
    if (!konqBookmark) {
        return QIcon();
    }
    
    return icon(konqBookmark->url());
}

QIcon PlacesManager::icon(const KonqHistoryEntry* historyEntry)
{
    if (!historyEntry) {
        return QIcon();
    }
    
    return icon(historyEntry->url);
}

QIcon PlacesManager::icon(const Place* place)
{
    if (!place) {
        return QIcon();
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
