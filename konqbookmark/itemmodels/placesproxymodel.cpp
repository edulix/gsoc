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

#include <kdescendantsproxymodel.h>
#include <kdebug.h>

#include <QString>
#include <QHash>
#include <QDateTime>
#include <QPersistentModelIndex>

using namespace Konqueror;


static const int relevance_match_bonus  = 10;

static const int first_bucket_days      = 4;
static const int second_bucket_days     = 14;
static const int third_bucket_days      = 31;
static const int fourth_bucket_days     = 90;

static const int first_bucket_weight    = 100;
static const int second_bucket_weight   = 70;
static const int third_bucket_weight    = 50;
static const int fourth_bucket_weight   = 30;
static const int default_bucket_weight  = 10;

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
    
    int matches(Place* place);
    bool updateRelevance(const QModelIndex& index);
    Place* placeFromIndex(const QModelIndex& index);
    
    void slotRowsInserted(const QModelIndex& parent, int start, int end);
    void slotRowsRemoved(const QModelIndex& parent, int start, int end);
    void slotModelAboutToBeReset();
    
    PlacesProxyModel* q;
    
    QString m_strQuery;
    QAbstractItemModel *m_sourceModel;
    KDescendantsProxyModel *m_descendantsModel;
    QHash<const QPersistentModelIndex, qreal> m_relevance;
};

PlacesProxyModel::Private::Private(PlacesProxyModel* parent)
    : q(parent), m_sourceModel(0), m_descendantsModel(0)
{

}

PlacesProxyModel::Private::~Private()
{

}

Place* PlacesProxyModel::Private::placeFromIndex(const QModelIndex& index)
{
    QUrl placeUrl = q->mapToSource(index).data(Place::PlaceUrlRole).toString();
    Place* place = PlacesManager::self()->place(placeUrl);
    
    // Places manager should have all the places!
    Q_ASSERT_X(place != 0, "placemanager", "Places manager should have all the places!");
    
    return place;
}

int PlacesProxyModel::Private::matches(Place* place)
{
    int matches = 0;
    matches += place->title().count(m_strQuery, Qt::CaseInsensitive);
    matches += place->url().toString().count(m_strQuery, Qt::CaseInsensitive);
    matches += place->tags().join(",").count(m_strQuery, Qt::CaseInsensitive);
    matches += place->description().count(m_strQuery, Qt::CaseInsensitive);
    return matches;
}

bool PlacesProxyModel::Private::updateRelevance(const QModelIndex& index)
{   
    Place* place = placeFromIndex(index);
    QDateTime currentTime = QDateTime::currentDateTime();
    
    qreal relevance = 0;
    int matches = this->matches(place);
    int weight = 0;
    
    int days = place->lastVisited().daysTo(currentTime);
    
    if(days < first_bucket_days) {
        weight = first_bucket_weight;
    } else if(days < second_bucket_days) {
        weight = second_bucket_weight;
    } else if(days < third_bucket_days) {
        weight = third_bucket_weight;
    } else if(days < fourth_bucket_days) {
        weight = fourth_bucket_weight;
    } else {
        weight = default_bucket_weight;
    }
    
    relevance += matches * weight + place->numVisits() * weight;
    
    if(m_relevance[index] == relevance) {
        return false;
    }
    
    m_relevance[index] = relevance;
    return true;
}

void PlacesProxyModel::Private::slotRowsInserted(const QModelIndex& parent, int start, int end)
{
    Q_UNUSED(parent);
    for(int i = start; i <= end; i++) {
        QModelIndex index = q->index(i, 0);
        updateRelevance(index);
    }
}

void PlacesProxyModel::Private::slotRowsRemoved(const QModelIndex& parent, int start, int end)
{
    Q_UNUSED(parent);
    for(int i = start; i <= end; i++) {
        QModelIndex index = q->index(i, 0);
        m_relevance.remove(index);
    }
}

void PlacesProxyModel::Private::slotModelAboutToBeReset()
{
    m_relevance.clear();
    QModelIndexList list = q->persistentIndexList();
    foreach(const QModelIndex& index, list) {
        updateRelevance(index);
    }
}
//@endcond

PlacesProxyModel::PlacesProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent), d(new Private(this))
{
    setSortRole(Place::PlaceRelevanceRole);
    setSortCaseSensitivity(Qt::CaseInsensitive);
    
    connect(this, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
        this, SLOT(slotRowsInserted(const QModelIndex&, int, int)));
        
    connect(this, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
        this, SLOT(slotRowsRemoved(const QModelIndex&, int, int)));
        
    connect(this, SIGNAL(modelAboutToBeReset()),
        this, SLOT(slotModelAboutToBeReset()));
        
    connect(this, SIGNAL(layoutAboutToBeChanged()),
        this, SLOT(slotModelAboutToBeReset()));
}

PlacesProxyModel::~PlacesProxyModel()
{
    delete d;
}

void PlacesProxyModel::setQuery(QString query)
{
    kDebug() << "query";
    d->m_strQuery = query;
    // HACK: triggers a call to d->filter_changed();
    setFilterKeyColumn(0);
}

void PlacesProxyModel::setQuery(QVariant query)
{
    setQuery(query);
}

QVariant PlacesProxyModel::query() const
{
    return d->m_strQuery;
}

void PlacesProxyModel::setSourceModel(QAbstractItemModel* sourceModel)
{
    // TODO we don't allow yet to change from a model to another
    Q_ASSERT(d->m_sourceModel == 0);
    
    d->m_sourceModel = sourceModel;
    d->m_descendantsModel = new KDescendantsProxyModel(this);
    d->m_descendantsModel->setSourceModel(d->m_sourceModel);
    
    QSortFilterProxyModel::setSourceModel(d->m_descendantsModel);
}

QVariant PlacesProxyModel::data(const QModelIndex& index, int role) const
{    
    switch( role )
    {
    case Place::PlaceRelevanceRole:
        if(!d->m_relevance.contains(index)) {
            d->updateRelevance(index);
        }
        return d->m_relevance[index];
    default:
        return QSortFilterProxyModel::data(index, role);
    }
}

bool PlacesProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    QModelIndex sourceIndex = sourceModel()->index(source_row, 0, source_parent);
    
    if(!sourceIndex.isValid() || sourceModel()->hasChildren(sourceIndex)) {
        return false;
    }
    
    QVariant variant = sourceIndex.data(Place::PlaceUrlRole);
    if(variant == QVariant()) {
        return false;
    }
    
    QUrl url = variant.toString();
    Place* place = PlacesManager::self()->place(url);
        
    // Places manager should have all the places!
    Q_ASSERT_X(place != 0, "placemanager", "Places manager should have all the places!");    
    
    return d->matches(place) > 0;
}

#include "placesproxymodel.moc"
