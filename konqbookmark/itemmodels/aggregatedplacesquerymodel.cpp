/*
   This file is part of the KDE libraries
   Copyright (c) 2009 Eduardo Robles Elvira <edulix@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.   If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "aggregatedplacesquerymodel.h"
#include "place.h"
#include "placequerymatch.h"

#include <QHash>
#include <QVariant>
#include <QList>
#include <QPersistentModelIndex>
#include <kdebug.h>

using namespace Konqueror;

/**
 * Private class that helps to provide binary compatibility between releases.
 * @internal
 */
//@cond PRIVATE
class AggregatedPlacesQueryModel::Private
{
public:
    Private(AggregatedPlacesQueryModel* parent);
    ~Private();
    
    void dataChanged(const QModelIndex& sourceTopLeft, const QModelIndex& sourceBottomRight);
    void layoutChanged();
    void modelReset();
    void rowsAboutToBeInserted(const QModelIndex& sourceParent, int start, int end);
    void rowsAboutToBeRemoved(const QModelIndex& sourceParent, int start, int end);
    void rowsInserted(const QModelIndex& sourceParent, int start, int end);
    void rowsRemoved(const QModelIndex& sourceParent, int start, int end);
    
    AggregatedPlacesQueryModel* q;
    QList<const QAbstractItemModel*> m_sourceModels;
    QHash<QPersistentModelIndex, QPersistentModelIndex> m_parents;
    QHash<const QAbstractItemModel*, QList<QPersistentModelIndex> > m_decendents;
};

AggregatedPlacesQueryModel::Private::Private(AggregatedPlacesQueryModel* parent)
    : q(parent)
{
    
}

AggregatedPlacesQueryModel::Private::~Private()
{

}


void AggregatedPlacesQueryModel::Private::dataChanged(const QModelIndex& sourceTopLeft,
    const QModelIndex& sourceBottomRight)
{
    QAbstractItemModel* sourceModel = qobject_cast<QAbstractItemModel*>(q->sender());
    QModelIndex topLeft = q->mapFromSource(sourceTopLeft, sourceModel);
    QModelIndex bottomRight = q->mapFromSource(sourceBottomRight, sourceModel);
    emit q->dataChanged(topLeft, bottomRight);
}

void AggregatedPlacesQueryModel::Private::layoutChanged()
{
    QAbstractItemModel* sourceModel = qobject_cast<QAbstractItemModel*>(q->sender());
    int row = m_sourceModels.indexOf(sourceModel);
    
    // Instead of emitting a full layoutChanged(), we remove and repopulate only the
    // branch of the tree related to this model
    q->beginRemoveRows(QModelIndex(), row, row);
    q->endRemoveRows();
    q->beginInsertRows(QModelIndex(), row, row);
    q->endInsertRows();
}

void AggregatedPlacesQueryModel::Private::modelReset()
{
    layoutChanged();
}


void AggregatedPlacesQueryModel::Private::rowsAboutToBeInserted(const QModelIndex& sourceParent, int start, int end)
{
    QAbstractItemModel* sourceModel = qobject_cast<QAbstractItemModel*>(q->sender());
    QModelIndex parent = q->mapFromSource(sourceParent, sourceModel);
    
    q->beginInsertRows(parent, start, end);
}


void AggregatedPlacesQueryModel::Private::rowsAboutToBeRemoved(const QModelIndex& sourceParent, int start, int end)
{
    QAbstractItemModel* sourceModel = qobject_cast<QAbstractItemModel*>(q->sender());
    QModelIndex parent = q->mapFromSource(sourceParent, sourceModel);
    
    q->beginRemoveRows(parent, start, end);
}


void AggregatedPlacesQueryModel::Private::rowsInserted(const QModelIndex& sourceParent, int start, int end)
{
    Q_UNUSED(sourceParent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    q->endInsertRows();
}


void AggregatedPlacesQueryModel::Private::rowsRemoved(const QModelIndex& sourceParent, int start, int end)
{
    Q_UNUSED(sourceParent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    q->endRemoveRows();
}
//@endcond
    
AggregatedPlacesQueryModel::AggregatedPlacesQueryModel(QObject* parent)
    : QAbstractItemModel(parent), d(new Private(this))
{

}
    
AggregatedPlacesQueryModel::~AggregatedPlacesQueryModel()
{
    delete d;
}
    
void AggregatedPlacesQueryModel::addSourceModel(QAbstractItemModel *sourceModel,
    SearchMode mode, int placeUrlRole)
{
    Q_UNUSED(mode);
    Q_UNUSED(placeUrlRole);
    
    if(d->m_sourceModels.contains(sourceModel)) {
        return;
    }
    
    int row = d->m_sourceModels.count();
    
    beginInsertRows(QModelIndex(), row, row);
    
    d->m_sourceModels.append(sourceModel);
    
    endInsertRows();
    
    connect(sourceModel, SIGNAL(void dataChanged(const QModelIndex& sourceTopLeft, const QModelIndex& sourceBottomRight)),
        this, SLOT(void dataChanged(const QModelIndex& sourceTopLeft, const QModelIndex& sourceBottomRight)));
            
    connect(sourceModel, SIGNAL(void layoutAboutToBeChanged()),
        this, SLOT(void layoutAboutToBeChanged()));
            
    connect(sourceModel, SIGNAL(void layoutChanged()),
        this, SLOT(void layoutChanged()));
            
    connect(sourceModel, SIGNAL(void modelAboutToBeReset()),
        this, SLOT(void modelAboutToBeReset()));
        
    connect(sourceModel, SIGNAL(void modelReset()),
        this, SLOT(void modelReset()));
        
    connect(sourceModel, SIGNAL(void rowsAboutToBeInserted(const QModelIndex& sourceParent, int start, int end)),
        this, SLOT(void rowsAboutToBeInserted(const QModelIndex& sourceParent, int start, int end)));
        
    connect(sourceModel, SIGNAL(void rowsAboutToBeRemoved(const QModelIndex& sourceParent, int start, int end)),
        this, SLOT(void rowsAboutToBeRemoved(const QModelIndex& sourceParent, int start, int end)));
        
    connect(sourceModel, SIGNAL(void rowsInserted(const QModelIndex& sourceParent, int start, int end)),
        this, SLOT(void rowsInserted(const QModelIndex& sourceParent, int start, int end)));
        
    connect(sourceModel, SIGNAL(void rowsRemoved(const QModelIndex& sourceParent, int start, int end)),
        this, SLOT(void rowsRemoved(const QModelIndex& sourceParent, int start, int end)));
}

void AggregatedPlacesQueryModel::removeSourceModel(QAbstractItemModel *sourceModel)
{
    if(!d->m_sourceModels.contains(sourceModel)) {
        return;
    }
    
    disconnect(sourceModel, SIGNAL(void dataChanged(const QModelIndex& sourceTopLeft, const QModelIndex& sourceBottomRight)),
        this, SLOT(void dataChanged(const QModelIndex& sourceTopLeft, const QModelIndex& sourceBottomRight)));
            
    disconnect(sourceModel, SIGNAL(void layoutAboutToBeChanged()),
        this, SLOT(void layoutAboutToBeChanged()));
            
    disconnect(sourceModel, SIGNAL(void layoutChanged()),
        this, SLOT(void layoutChanged()));
            
    disconnect(sourceModel, SIGNAL(void modelAboutToBeReset()),
        this, SLOT(void modelAboutToBeReset()));
        
    disconnect(sourceModel, SIGNAL(void modelReset()),
        this, SLOT(void modelReset()));
        
    disconnect(sourceModel, SIGNAL(void rowsAboutToBeInserted(const QModelIndex& sourceParent, int start, int end)),
        this, SLOT(void rowsAboutToBeInserted(const QModelIndex& sourceParent, int start, int end)));
        
    disconnect(sourceModel, SIGNAL(void rowsAboutToBeRemoved(const QModelIndex& sourceParent, int start, int end)),
        this, SLOT(void rowsAboutToBeRemoved(const QModelIndex& sourceParent, int start, int end)));
        
    disconnect(sourceModel, SIGNAL(void rowsInserted(const QModelIndex& sourceParent, int start, int end)),
        this, SLOT(void rowsInserted(const QModelIndex& sourceParent, int start, int end)));
        
    disconnect(sourceModel, SIGNAL(void rowsRemoved(const QModelIndex& sourceParent, int start, int end)),
        this, SLOT(void rowsRemoved(const QModelIndex& sourceParent, int start, int end)));
    
    int row = d->m_sourceModels.indexOf(sourceModel);
    
    beginRemoveRows(QModelIndex(), row, row);
    
    d->m_sourceModels.removeOne(sourceModel);
    
    foreach(QPersistentModelIndex index, d->m_decendents[sourceModel]) {
        d->m_parents.remove(index);
    }
    
    d->m_decendents.remove(sourceModel);
    
    endRemoveRows();
}

int AggregatedPlacesQueryModel::rowCount(const QModelIndex &parent) const
{
    // If it's invalid we want to count the top level indexes i.e. the models
    if(!parent.isValid()) {
        return d->m_sourceModels.count();
    }
    
    const QAbstractItemModel *sourceModel = sourceOf(parent);
    
    if(!sourceModel) {
        return 0;
    }
    
    return sourceModel->rowCount(mapToSource(parent));
}

QVariant AggregatedPlacesQueryModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }
    
    const QAbstractItemModel *sourceModel = sourceOf(index);
    
    if(!sourceModel) {
        return QVariant();
    }
    
    switch(role) {
    case PlaceQueryMatchRole:
        return QVariant(); // TODO
    default:
        return mapToSource(index).data();
    }
}
    
QModelIndex AggregatedPlacesQueryModel::index(int row, int column, const QModelIndex& parent) const
{   
    if(!parent.isValid()) {
        if(row < 0 || row > d->m_sourceModels.count()) {
            return QModelIndex();
        }
        
        return createIndex(row, column);
    }
    const QAbstractItemModel *sourceModel = sourceOf(parent);
    
    if(!sourceModel) {
        return QModelIndex();
    }
    
    if(column < 0 || column > 1) {
        return QModelIndex();
    }
    
    QModelIndex sourceParent = mapToSource(parent);
    QPersistentModelIndex* sourceIndex = new QPersistentModelIndex(sourceParent.child(row, column));
    
    QModelIndex ret = createIndex(row, column, (void *)sourceIndex);
    
    if(!d->m_parents.contains(ret)) {
        d->m_parents[ret] = parent;
        d->m_decendents[sourceModel].append(ret);
    }
    return ret;
}
    
QModelIndex AggregatedPlacesQueryModel::parent(const QModelIndex& index) const
{   
    if(!index.isValid() || !d->m_parents.contains(index)) {
        return QModelIndex();
    }
    
    return d->m_parents[index];
}
    
int AggregatedPlacesQueryModel::columnCount(const QModelIndex& index) const
{
    Q_UNUSED(index);
    return ColumnCount;
}
        
QModelIndex AggregatedPlacesQueryModel::mapFromSource(const QModelIndex& sourceIndex,
    const QAbstractItemModel *sourceModel) const
{
    const QAbstractItemModel* sModel = sourceModel;
    if(!sourceIndex.isValid()) {
        if(!sourceModel) {
            return QModelIndex();
        }
        
        if(!d->m_sourceModels.contains(sourceModel)) {
            return QModelIndex();
        }
        
        return index(d->m_sourceModels.indexOf(sourceModel), 0);
    }
    
    sModel = sourceIndex.model();
    
    Q_ASSERT(sourceModel != 0 && sModel != sourceModel);
    
    if(!d->m_sourceModels.contains(sModel)) {
        return QModelIndex();
    }
    
    QList<int> rows;
    QList<int> cols;
    QModelIndex ancestor = sourceIndex;
    while(ancestor.parent().isValid()) {
        rows.prepend(ancestor.row());
        cols.prepend(ancestor.column());
        ancestor = ancestor.parent();
    }
    
    QModelIndex ret = index(d->m_sourceModels.indexOf(sModel), 0);
    for(int i = 0; i < rows.count(); i++) {
        ret = index(rows.at(i), cols.at(i), ret);
    }
    
    return ret;
}

QModelIndex AggregatedPlacesQueryModel::mapToSource(const QModelIndex& index) const
{   
    if(!index.isValid()) {
        return QModelIndex();
    }
    
    if(!index.parent().isValid()) {
        return QModelIndex();
    }
    
    return *reinterpret_cast<QPersistentModelIndex*>(index.internalPointer());
}

const QAbstractItemModel* AggregatedPlacesQueryModel::sourceOf(const QModelIndex& index) const
{
    if(!index.isValid()) {
        return 0;
    }
    
    QModelIndex ancestor = index;
    while(ancestor.parent().isValid()) {
        ancestor = ancestor.parent();
    }
    
    // ancestor is now a top level item (parent == QModelIndex()). Now we can
    // easily know which is it's source model
    if(ancestor.row() < 0 || ancestor.row() > d->m_sourceModels.count()) {
        return 0;
    }
    
    return d->m_sourceModels.at(ancestor.row());
}

#include "aggregatedplacesquerymodel.moc"
