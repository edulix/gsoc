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
#include "placesmanager_p.h"
#include "place.h"
#include "konqbookmark.h"
#include "itemmodels/konqbookmarkmodel.h"
#include "itemmodels/kcompletionmodel.h"

#include <konq_historyentry.h> // Imported from libkonq
#include <kurlcompletion.h>
#include <kcompletion.h>
#include <kglobal.h>

#include <akonadi/monitor.h>
#include <akonadi/session.h>

#include <QByteArray>
#include <QModelIndexList>

PlacesManager* PlacesManager::s_self = 0;

PlacesManager::PlacesManager()
    : QAbstractListModel(0), d(new Private(this))
{
    
}

PlacesManager::~PlacesManager()
{
    delete d;
}

PlacesManager *PlacesManager::self()
{
    if (s_self) {
        return s_self;
    }
    
    s_self = new PlacesManager();
    
    return s_self;
}

KonqBookmarkModel *PlacesManager::bookmarkModel()
{
    return d->m_bookmarksModel;
}


KonqHistoryProvider *PlacesManager::historyProvider()
{
    return d->m_historyProvider;
}


bool PlacesManager::filterAcceptUrl(const QUrl &url, KCompletionModel *completionModel) const
{
    if (d->m_bookmarks.contains(url) || d->m_historyEntries.contains(url)) {
        return true;
    }

    return d->m_completedUrls.contains(completionModel) &&
        d->m_completedUrls[completionModel]->contains(url);
}

void PlacesManager::registerUrlCompletionModel(KCompletionModel *urlCompletionModel)
{
    d->m_completedUrls[urlCompletionModel] = new QList<QUrl>();
    
    connect(urlCompletionModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
        this, SLOT(slotUrlsInserted(const QModelIndex &, int, int)));
        
    connect(urlCompletionModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
        this, SLOT(slotUrlsRemoved(const QModelIndex &, int, int)));
}

void PlacesManager::registerHistoryProvider(KonqHistoryProvider *historyProvider)
{
    Q_ASSERT(historyProvider != 0);
    d->m_historyProvider = historyProvider;
    
    KonqHistoryList entries(historyProvider->entries());

    KonqHistoryList::const_iterator it = entries.constBegin();
    const KonqHistoryList::const_iterator end = entries.constEnd();
    for ( ; it != end ; ++it) {
        kDebug() << (*it).url;
        d->slotHistoryEntryAdded(*it);
    }
    
    connect(historyProvider, SIGNAL(entryAdded(const KonqHistoryEntry &)),
        this, SLOT(slotHistoryEntryAdded(const KonqHistoryEntry &)));
        
    connect(historyProvider, SIGNAL(entryRemoved(const KonqHistoryEntry &)),
        this, SLOT(slotHistoryEntryRemoved(const KonqHistoryEntry &)));
        
    connect(historyProvider, SIGNAL(cleared()),
        this, SLOT(slotHistoryCleared()));
}

KonqBookmark *PlacesManager::bookmark(const QUrl &url)
{
    if (d->m_bookmarks.contains(url)) {
        return d->m_bookmarks[url];
    }
    
    return 0;
}

KonqBookmark *PlacesManager::bookmark(const KonqHistoryEntry *historyEntry)
{
    if (!historyEntry) {
        return 0;
    }
    
    return bookmark(historyEntry->url);
}

KonqHistoryEntry *PlacesManager::historyEntry(const QUrl &url)
{
    if (d->m_historyEntries.contains(url)) {
        return d->m_historyEntries[url];
    }
 
    QHash<QUrl, KonqHistoryEntry *> myhash;
    myhash[QUrl()] = 0;
    
    return 0;
}

KonqHistoryEntry *PlacesManager::historyEntry(const KonqBookmark *konqBookmark)
{
    if (!konqBookmark) {
        return 0;
    }
    
    return historyEntry(konqBookmark->url());
}

Place *PlacesManager::place(const QUrl &url)
{
    if (d->m_places.contains(url)) {
        return d->m_places[url];
    }
    
    // Create an "orphan" place (with no history entry o konqBookmark attached)
    Place *place = new Place(url);
    d->m_places[url] = place;
    
    beginInsertRows(QModelIndex(), d->m_urls.count(), d->m_urls.count());
    d->m_urls.append(url);
    endInsertRows();
    
    return place;
}

Place *PlacesManager::place(KonqBookmark *konqBookmark)
{
    if (!konqBookmark) {
        return 0;
    }
    
    QUrl url = konqBookmark->url();
    Place *place = 0;
    if (d->m_places.contains(url)) {
        place = d->m_places[url];
        place->setBookmark(konqBookmark);
        d->m_bookmarks[url] = konqBookmark;
        return place;
    }
    
    // Create the place and set its konqBookmark
    place = new Place(url);
    place->setBookmark(konqBookmark);
    d->m_places[url] = place;
    d->m_bookmarks[url] = konqBookmark;
    
    beginInsertRows(QModelIndex(), d->m_urls.count(), d->m_urls.count());
    d->m_urls.append(url);
    endInsertRows();
    return place;
}

Place *PlacesManager::place(KonqHistoryEntry *historyEntry)
{
    if (!historyEntry) {
        return 0;
    }

    QUrl url = historyEntry->url;
    Place *place = 0;
    if (d->m_places.contains(url)) {
        place = d->m_places[url];
        place->setHistoryEntry(historyEntry);
        d->m_historyEntries[url] = historyEntry;
        return place;
    }
    
    // Create the place and set its historyEntry
    place = new Place(url);
    place->setHistoryEntry(historyEntry);
    d->m_places[url] = place;
    d->m_historyEntries[url] = historyEntry;
    
    beginInsertRows(QModelIndex(), d->m_urls.count(), d->m_urls.count());
    d->m_urls.append(url);
    endInsertRows();
    return place;
}

QIcon PlacesManager::icon(const QUrl &url)
{
    Q_UNUSED(url);
    
    return QIcon();
}

QIcon PlacesManager::icon(const KonqBookmark *konqBookmark)
{
    if (!konqBookmark) {
        return QIcon();
    }
    
    return icon(konqBookmark->url());
}

QIcon PlacesManager::icon(const KonqHistoryEntry *historyEntry)
{
    if (!historyEntry) {
        return QIcon();
    }
    
    return icon(historyEntry->url);
}

QIcon PlacesManager::icon(const Place *place)
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


int PlacesManager::rowCount(const QModelIndex &index) const
{
    if (index.isValid()) {
        return 0;
    }

    return d->m_urls.count();
}

QVariant PlacesManager::data(const QModelIndex &index, int role) const
{
    if (index.parent().isValid() || index.row() < 0 || index.row() > d->m_urls.count()) {
        return QVariant();
    }

    switch (role) {
    case Qt::DisplayRole:
    case Konqueror::Place::PlaceUrlRole:
        return d->m_urls.at(index.row());
    default:
        return QVariant();
    }
}

#include "placesmanager.moc"
