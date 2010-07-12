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

#include "placesproxymodel.h"
#include "placesmanager.h"
#include "kcompletionmodel.h"
#include "kdescendantsproxymodel_p.h"

#include <kurlcompletion.h>
#include <kdebug.h>

#include <QString>
#include <QVariant>
#include <QtAlgorithms>
#include <QHash>
#include <QMap>
#include <QDateTime>
#include <QPersistentModelIndex>
#include <QtCore/QFile>

using namespace Konqueror;


static const int relevanceMatchBonus  = 10;
static const int completionItemBonus  = 3000;

static const int firstBucketDays      = 4;
static const int secondBucketDays     = 14;
static const int thirdBucketDays      = 31;
static const int fourthBucketDays     = 90;

static const int firstBucketWeight    = 100;
static const int secondBucketWeight   = 70;
static const int thirdBucketWeight    = 50;
static const int fourthBucketWeight   = 30;
static const int defaultBucketWeight  = 10;

/**
 * Private class that helps to provide binary compatibility between releases.
 * @internal
 */
//@cond PRIVATE
class PlacesProxyModel::Private
{
public:
    Private(PlacesProxyModel *parent);
    ~Private();

    int matches(Place *place);
    bool updateRelevance(const QModelIndex &index);
    Place *placeFromIndex(const QModelIndex &index);

    void slotRowsInserted(const QModelIndex &source_parent, int start, int end);
    void slotRowsRemoved(const QModelIndex &source_parent, int start, int end);
    void slotModelReset();

    PlacesProxyModel *q;

    KCompletionModel *m_urlCompletionModel;
    QString m_strQuery;
    QHash<const QPersistentModelIndex, qreal> m_relevance;
    QStringList m_strQueryWords;
};

PlacesProxyModel::Private::Private(PlacesProxyModel *parent)
    : q(parent), m_strQuery(QString())
{

}

PlacesProxyModel::Private::~Private()
{

}

Place* PlacesProxyModel::Private::placeFromIndex(const QModelIndex &index)
{
    QUrl placeUrl = index.data(Place::PlaceUrlRole).toString();
    Place *place = PlacesManager::self()->place(placeUrl);

    // Places manager should have all the places!
    Q_ASSERT_X(place != 0, "placemanager", "Places manager should have all the places!");

    return place;
}

int PlacesProxyModel::Private::matches(Place *place)
{
    int matches = 0;
    int prevMatches = 0;
    bool allHaveMatches = true;
    Q_FOREACH (QString strQuery, m_strQueryWords) {
        prevMatches = matches;
        matches += place->title().count(strQuery, Qt::CaseInsensitive);
        matches += place->url().toString().count(strQuery, Qt::CaseInsensitive);
        matches += place->tags().join(",").count(strQuery, Qt::CaseInsensitive);
        matches += place->description().count(strQuery, Qt::CaseInsensitive);
        if (prevMatches == matches) {
            allHaveMatches = false;
        }
    }
    if (allHaveMatches) {
        return matches;
    } else {
        return 0;
    }
}

bool PlacesProxyModel::Private::updateRelevance(const QModelIndex &index)
{
    Place *place = placeFromIndex(index);
    QDateTime currentTime = QDateTime::currentDateTime();

    qreal relevance = 0;
    int matches = this->matches(place);
    int weight = 0;

    int days = place->lastVisited().daysTo(currentTime);

    if (days < firstBucketDays) {
        weight = firstBucketWeight;
    } else if (days < secondBucketDays) {
        weight = secondBucketWeight;
    } else if (days < thirdBucketDays) {
        weight = thirdBucketWeight;
    } else if (days < fourthBucketDays) {
        weight = fourthBucketWeight;
    } else {
        weight = defaultBucketWeight;
    }

    relevance += matches * weight + place->numVisits() * weight;

    if (m_relevance.contains(index) && m_relevance[index] == relevance) {
        return false;
    }

    if (place->isCompletedItem()) {
        relevance += completionItemBonus;
    }

    m_relevance[index] = relevance;
    return true;
}

void PlacesProxyModel::Private::slotRowsInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    for (int i = start; i <= end; i++) {
        QModelIndex index = q->index(i, 0);
        updateRelevance(index);
    }
}

void PlacesProxyModel::Private::slotRowsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    for (int i = start; i <= end; i++) {
        QModelIndex index = q->index(i, 0);
        m_relevance.remove(index);
    }
}

void PlacesProxyModel::Private::slotModelReset()
{
    m_relevance.clear();

    QModelIndexList persistentIndexList = q->persistentIndexList();
    foreach(const QModelIndex &index, persistentIndexList) {
        updateRelevance(index);
    }
}
//@endcond

PlacesProxyModel::PlacesProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent), d(new Private(this))
{
    // Create the url completion model and set the source model
    d->m_urlCompletionModel = new KCompletionModel(this);
    d->m_urlCompletionModel->setCompletion(new KUrlCompletion());
    PlacesManager::self()->registerUrlCompletionModel(d->m_urlCompletionModel);
    QSortFilterProxyModel::setSourceModel(PlacesManager::self());
    connect(this, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
        this, SLOT(slotRowsInserted(const QModelIndex &, int, int)));
    connect(this, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
        this, SLOT(slotRowsRemoved(const QModelIndex &, int, int)));
    connect(this, SIGNAL(modelReset()),
        this, SLOT(slotModelReset()));
    connect(this, SIGNAL(layoutChanged()),
        this, SLOT(slotModelReset()));
}

PlacesProxyModel::~PlacesProxyModel()
{
    delete d;
}

void PlacesProxyModel::setQuery(QString query)
{
    query = query.trimmed();

    if (query.isEmpty()) {
        return;
    }

    d->m_strQuery = query;
    d->m_strQueryWords = query.split(" ", QString::SkipEmptyParts);

    // Now set the correct query for the url completion model. If query is
    // "/home/edulix/downloads/ bz2" and there's no file called " bz2" in downloads dir,
    // then the url completion model query will be "/home/edulix/downloads/", and the
    // proxy model will take the job of filtering items that contains bz2. However if
    // file " bz2" exists, then the url completion model query will be as the normal query:
    // "/home/edulix/downloads/ bz2".
    //
    // We do this by checking space by space, starting from the end, if the file exists. So
    // we check first if "/home/edulix/downloads/ bz2" exists, then "/home/edulix/downloads/",
    // and so on.
    int i = -1;
    QString urlCompletionQuery = query;
    while((i = query.lastIndexOf(' ')) != -1) {
        urlCompletionQuery.truncate(i);
        if (QFile::exists(urlCompletionQuery)) {
            d->m_urlCompletionModel->completion()->slotMakeCompletion(urlCompletionQuery);
            break;
        }
    }
    // If file not found, use the whole query for url completion model query
    if (i == -1) {
        d->m_urlCompletionModel->completion()->slotMakeCompletion(query);
    }

    d->m_relevance.clear();
    invalidate();
}

QString PlacesProxyModel::query() const
{
    return d->m_strQuery;
}

QVariant PlacesProxyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
    case Place::PlaceRelevanceRole:
        if(!d->m_relevance.contains(index)) {
            d->updateRelevance(index);
        }
        return d->m_relevance[index];
    default:
        return QSortFilterProxyModel::data(index, role);
    }
}

void PlacesProxyModel::setSourceModel(QAbstractItemModel */*sourceModel*/)
{
    Q_ASSERT_X(false, "source model", "Shouldn't be called, this proxy model has PlacesManager model as the fixed source model");
}

bool PlacesProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent);

    Q_ASSERT(sourceModel());
    // 1. Obtain the place URL for the given row
    QModelIndex sourceIndex = sourceModel()->index(source_row, 0, QModelIndex());

    if (!sourceIndex.isValid()) {
        return false;
    }

    QVariant variant = sourceIndex.data(Place::PlaceUrlRole);
    if (variant == QVariant()) {
        return false;
    }

    QUrl url = variant.toString();
    if(url.isEmpty()) {
        return false;
    }

    // 2. Only accept URLs from history, bookmarks, or our url completion model
    if(!PlacesManager::self()->filterAcceptUrl(url, d->m_urlCompletionModel)) {
        return false;
    }

    Place* place = PlacesManager::self()->place(url);
    // Places manager should have all the places!
    Q_ASSERT_X(place != 0, "placemanager", "Places manager should have all the places!");

    // 3. Only accept places relevant to our query
    bool accept = d->matches(place) > 0;
    return accept;
}


////////////////////////

class LocationBarCompletionModel::Private
{
public:
    Private(LocationBarCompletionModel *parent);
    ~Private();

    void slotModelReset();

    LocationBarCompletionModel *q;
    QHash<int, int> m_mapFromSource;
    QHash<int, int> m_mapToSource;
};

LocationBarCompletionModel::Private::Private(LocationBarCompletionModel *parent)
    : q(parent)
{

}

LocationBarCompletionModel::Private::~Private()
{

}

class ComparePlaces
{
public:
    bool operator()(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const
    {
        int leftRelevance = sourceLeft.data(Place::PlaceRelevanceRole).value<qreal>();
        int rightRelevance = sourceRight.data(Place::PlaceRelevanceRole).value<qreal>();
        int ret = leftRelevance > rightRelevance;
        if (leftRelevance == rightRelevance) {
            return sourceLeft.data().toString() > sourceRight.data().toString();
        }
        return ret;
    }
};

void LocationBarCompletionModel::Private::slotModelReset()
{
    emit q->layoutAboutToBeChanged();
    QModelIndexList values;
    m_mapFromSource.clear();
    m_mapToSource.clear();
    for (int i = 0; i < q->sourceModel()->rowCount(); i++) {
        QModelIndex sourceIndex = q->sourceModel()->index(i, 0);
        values.append(sourceIndex);
    }
    qSort(values.begin(), values.end(), ComparePlaces());
    int i = 0;
    Q_FOREACH (const QModelIndex sourceIndex, values) {
        m_mapToSource[i] = sourceIndex.row();
        m_mapFromSource[sourceIndex.row()] = i;
        i++;
    }
    emit q->layoutChanged();
}

LocationBarCompletionModel::LocationBarCompletionModel(PlacesProxyModel *sourceModel, QObject
    *parent)
    : QAbstractProxyModel(parent), d(new Private(this))
{
    QAbstractProxyModel::setSourceModel(sourceModel);

    d->slotModelReset();

    connect(sourceModel, SIGNAL(modelReset()),
        this, SLOT(slotModelReset()));
    connect(sourceModel, SIGNAL(layoutChanged()),
        this, SLOT(slotModelReset()));
}

QModelIndex LocationBarCompletionModel::mapToSource(const QModelIndex &proxyIndex) const
{
    return sourceModel()->index(d->m_mapToSource[proxyIndex.row()], 0);
}

QModelIndex LocationBarCompletionModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    return index(d->m_mapFromSource[sourceIndex.row()], 0);
}

QModelIndex LocationBarCompletionModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column != 0 || parent.isValid()) {
        return QModelIndex();
    }

    if (row < 0 || row > rowCount(QModelIndex())) {
        return QModelIndex();
    }

    return createIndex(row, column);
}

QModelIndex LocationBarCompletionModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

int LocationBarCompletionModel::rowCount(const QModelIndex &index) const
{
    if(index.isValid()) {
        return 0;
    }

    return sourceModel()->rowCount();
}

int LocationBarCompletionModel::columnCount(const QModelIndex &) const
{
    return sourceModel()->columnCount();
}

QVariant LocationBarCompletionModel::data(const QModelIndex &index, int role) const
{
    return sourceModel()->data(mapToSource(index), role);
}

#include "placesproxymodel.moc"
