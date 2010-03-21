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
#include <KUrlCompletion>
#include <KCompletion>
#include <KGlobal>
#include <KDebug>

#include <Akonadi/ChangeRecorder>
#include <Akonadi/ItemFetchScope>
#include <Akonadi/Session>

#include <QtCore/QByteArray>
#include <QtCore/QModelIndexList>

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
    Private(PlacesManager *parent);
    ~Private();

    void slotBookmarksInserted(const QModelIndex &parent, int start, int end);
    void slotBookmarksRemoved(const QModelIndex &parent, int start, int end);
    void slotBookmarksChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

    void slotUrlsInserted(const QModelIndex &parent, int start, int end);
    void slotUrlsRemoved(const QModelIndex &parent, int start, int end);

    void slotHistoryEntryAdded(const KonqHistoryEntry &entry);
    void slotHistoryEntryRemoved(const KonqHistoryEntry &entry);
    void slotHistoryCleared();

    PlacesManager *q;

    KonqBookmarkModel *m_bookmarksModel;
    KonqHistoryProvider *m_historyProvider;
    QHash<QUrl, Place *> m_places;

    // We keep a hash of history entries and bookmarks even if those are already
    // refered within the Place of each item in m_places just for convenience.
    // If neeeded for simplicity or for reducting memory usage, these two hashes
    // could actually go away
    QHash<QUrl, KonqHistoryEntry *> m_historyEntries;
    QHash<QUrl, KonqBookmark *> m_bookmarks;

    QHash<QUrl, QUrl> m_uniqueBookmarks; // QHash<uniqueUri(), url()>
    QHash<KCompletionModel *, QList<QUrl> *> m_completedUrls;

    // List of the places' URLs. There's an url for each place in the manager,
    // i.e. we keep this list updated. The order of the items in the list
    // determines the order in the PlacesManager model.
    QList<QUrl> m_urls;

    // Useful flag to be able to add a bunch of new places without sending an update to
    // the itemmodel for each of the added/removed element
    bool m_doingAnUpdateFlag;

    // We need a list of usual folders, for example the "Unsorted Bookmarks" folder, useful
    // when adding a bookmark by directly clicking in the bookmark icon of the location bar for
    // example.
    // TODO At the moment, the unsortedBookmarksFolderIndex is the top parent folder.
    Akonadi::Collection m_unsortedBookmarksFolder;
};

PlacesManager::Private::Private(PlacesManager *parent)
    : q(parent)
{
    Session *session = new Session(QByteArray("PlacesManager-") + QByteArray::number(qrand()), q);
    ChangeRecorder *monitor = new ChangeRecorder(q);


    Akonadi::ItemFetchScope scope;
    scope.fetchFullPayload(true);

    monitor->fetchCollection(true);
    monitor->setItemFetchScope(scope);

    // TODO, BUG: If we set only our own mimetype as monitored, then when collections
    // are added or removed we don't get the notice!
    monitor->setAllMonitored();
    monitor->setMimeTypeMonitored(KonqBookmark::mimeType());

    m_bookmarksModel = new KonqBookmarkModel(session, monitor, q);

    connect(m_bookmarksModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
        q, SLOT(slotBookmarksInserted(const QModelIndex &, int, int)));

    connect(m_bookmarksModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
        q, SLOT(slotBookmarksRemoved(const QModelIndex &, int, int)));

    connect(m_bookmarksModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
        q, SLOT(slotBookmarksChanged(const QModelIndex &, const QModelIndex &)));
}

PlacesManager::Private::~Private()
{
}

void PlacesManager::Private::slotBookmarksInserted(const QModelIndex &parent, int start, int end)
{
    for (int i = start; i <= end; i++) {
        QModelIndex index = m_bookmarksModel->index(i, KonqBookmarkModel::UniqueUri, parent);

        // Update m_unsortedBookmarksFolder
        if (!parent.isValid()) {
            Akonadi::Collection collection = qVariantValue<Akonadi::Collection>(
                index.data(EntityTreeModel::CollectionRole));
            if (collection.isValid() && collection.remoteId() == "konqbookmark:/") {
                m_unsortedBookmarksFolder = collection;
            }
        }

        if (index.data().toString().isEmpty()) {
            continue;
        }
        KonqBookmark *konqBookmark = new KonqBookmark(index.data().toString());
        m_uniqueBookmarks[konqBookmark->uniqueUri()] = konqBookmark->url();
        if (konqBookmark->url().toString().isEmpty()) {
            continue;
        }

        // Update/insert place
        Place* place = q->place(konqBookmark);
        place->setItem(qVariantValue<Akonadi::Item>(index.data(EntityTreeModel::ItemRole)));
    }
}

void PlacesManager::Private::slotBookmarksRemoved(const QModelIndex &parent, int start, int end)
{
    for (int i = start; i <= end; i++) {
        QModelIndex index = m_bookmarksModel->index(i, KonqBookmarkModel::Url, parent);

        if (index.data().toString().isEmpty()) {
            continue;
        }

        QUrl url(index.data().toString());
        if (m_bookmarks.contains(url)) {
            // Update/remove place
            Q_ASSERT(m_places[url] != 0);

            // if history entry is null and the url is not set either, just
            // remove this place (otherwise we would end up with a place with
            // both bookmark and history entry unset). Otherwise, we just update the place.
            m_places[url]->setBookmark(0);
            if (!m_places[url]->historyEntry() && m_places[url]->url().isEmpty()) {
                delete m_places[url];
                m_places.remove(url);

                // Remove item from the model
                int pos = m_urls.indexOf(url);
                q->beginRemoveRows(QModelIndex(), pos, pos);
                m_urls.removeAt(pos);
                q->endRemoveRows();
            }

            m_uniqueBookmarks.remove(m_bookmarks[url]->uniqueUri());
            delete m_bookmarks[url];
            m_bookmarks.remove(url);
        }
    }
}

void PlacesManager::Private::slotBookmarksChanged(const QModelIndex &topLeft, const QModelIndex
    &bottomRight)
{
    kDebug() << topLeft << bottomRight;
    int start = topLeft.row();
    int end = bottomRight.row();
    if (start > end || start < 0 || topLeft.parent() != bottomRight.parent()) {
        return;
    }
    QModelIndex parent = topLeft.parent();

    for (int i = start; i <= end; i++) {
        QModelIndex index = m_bookmarksModel->index(i, KonqBookmarkModel::UniqueUri, parent);

        kDebug() << index << index.data() << index.data().toString();
        if(index.data().toString().isEmpty() ||
            !m_uniqueBookmarks.contains(index.data().toString())) {
            continue;
        }
        KonqBookmark *konqBookmark = new KonqBookmark(index.data().toString());
        QUrl oldUrl = m_uniqueBookmarks[index.data().toString()];
        if (!oldUrl.isEmpty() && oldUrl == konqBookmark->url()) {
            delete konqBookmark;
            continue;
        }
        // 1. Remove old bookmark
        if (!oldUrl.isEmpty()) {
            Q_ASSERT(m_places[oldUrl] != 0);
            // if history entry is null and the url is not set either, just
            // remove this place (otherwise we would end up with a place with
            // both bookmark and history entry unset). Otherwise, we just update the place.
            m_places[oldUrl]->setBookmark(0);
            if (!m_places[oldUrl]->historyEntry() && m_places[oldUrl]->url().isEmpty()) {
                delete m_places[oldUrl];
                m_places.remove(oldUrl);
            }

            m_uniqueBookmarks[konqBookmark->uniqueUri()] = konqBookmark->url();
            m_bookmarks.remove(oldUrl);
        }

        // 2. Add new bookmark if needed
        if (konqBookmark->url().toString().isEmpty() ||
            m_bookmarks.contains(konqBookmark->url())) {
            delete konqBookmark;
            continue;
        }
        m_bookmarks[konqBookmark->url()] = konqBookmark;
        kDebug() << konqBookmark->url() << konqBookmark->title();

        // Update/insert place
        Place *place = q->place(konqBookmark);
        place->setItem(qVariantValue<Akonadi::Item>(index.data(EntityTreeModel::ItemRole)));

    }
}

void PlacesManager::Private::slotUrlsInserted(const QModelIndex& parent, int start, int end)
{
    KCompletionModel *urlCompletionModel = qobject_cast<KCompletionModel*>(q->sender());
    for (int i = start; i <= end; i++) {
        QModelIndex index = urlCompletionModel->index(i, 0, parent);

        QUrl url = index.data().toString();

        if (!url.isValid()) {
            continue;
        }

        if (!m_completedUrls[urlCompletionModel]->contains(url)) {
            m_completedUrls[urlCompletionModel]->append(url);
        }
        // Update/insert place
        q->place(url);
    }
}

void PlacesManager::Private::slotUrlsRemoved(const QModelIndex& parent, int start, int end)
{
    KCompletionModel *urlCompletionModel = qobject_cast<KCompletionModel*>(q->sender());
    for (int i = start; i <= end; i++) {
        QModelIndex index = urlCompletionModel->index(i, 0, parent);
        QUrl url = index.data().toString();

        if (!url.isValid()) {
            continue;
        }

        // Remove
        if (m_places.contains(url) && !m_places[url]->bookmark() && !m_places[url]->historyEntry()) {
            delete m_places[url];
            m_places.remove(url);
            m_completedUrls[urlCompletionModel]->removeOne(url);

            // Remove item from the model
            int pos = m_urls.indexOf(url);
            q->beginRemoveRows(QModelIndex(), pos, pos);
            m_urls.removeAt(pos);
            q->endRemoveRows();
        }
    }
}

void PlacesManager::Private::slotHistoryEntryAdded(const KonqHistoryEntry &entry)
{
    KonqHistoryEntry *historyEntry = new KonqHistoryEntry(entry);
    m_historyEntries[entry.url] = historyEntry;

    // Update/insert place
    q->place(historyEntry);
}

void PlacesManager::Private::slotHistoryEntryRemoved(const KonqHistoryEntry &entry)
{
    // If the history entry is registered, the related place should be there too
    if (!m_places.contains(entry.url)) {
        return;
    }

    delete m_historyEntries[entry.url];
    m_historyEntries.remove(entry.url);

    if (m_places.contains(entry.url) &&  !m_bookmarks[entry.url]
        && m_places[entry.url]->url() == QUrl()) {
        delete m_places[entry.url];
        m_places.remove(entry.url);

        // Remove item from the model
        int pos = m_urls.indexOf(entry.url);
        q->beginRemoveRows(QModelIndex(), pos, pos);
        m_urls.removeAt(pos);
        q->endRemoveRows();
    } else {
        m_places[entry.url]->setHistoryEntry(0);
    }
}

void PlacesManager::Private::slotHistoryCleared()
{
    foreach (KonqHistoryEntry* entry, m_historyEntries) {
        slotHistoryEntryRemoved(*entry);
    }
}

//@endcond
