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

#include <QHash>
#include <QVariant>
#include <QList>
#include <QPersistentModelIndex>

#include <klocale.h>
#include <krandom.h>
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
    QHash<const QAbstractItemModel*, QString> m_sourceModelNames;
    
    QHash<QString, QPersistentModelIndex> m_sourceIndexes;
    QHash<QPersistentModelIndex, QString*> m_reverseSourceIndexes;
    
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
    kDebug() << sourceParent << start << end;
    
    QAbstractItemModel* sourceModel = qobject_cast<QAbstractItemModel*>(q->sender());
    QModelIndex parent = q->mapFromSource(sourceParent, sourceModel);
    kDebug() << sourceModel << parent;
    
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
    kDebug() << sourceParent << start << end;
    
//     Q_UNUSED(sourceParent);
//     Q_UNUSED(start);
//     Q_UNUSED(end);
    q->endInsertRows();
}


void AggregatedPlacesQueryModel::Private::rowsRemoved(const QModelIndex& sourceParent, int start, int end)
{
    // TODO ?
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
    SearchMode mode, QString name)
{
    Q_UNUSED(mode);
    
    kDebug() << sourceModel;
    
    if(d->m_sourceModels.contains(sourceModel)) {
        return;
    }
    
    d->m_sourceModelNames[sourceModel] = name;
    
    int row = d->m_sourceModels.count();
    beginInsertRows(QModelIndex(), row, row);
    
    d->m_sourceModels.append(sourceModel);
    
    endInsertRows();
    
    connect(sourceModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
        this, SLOT(dataChanged(const QModelIndex&, const QModelIndex&)));
            
    connect(sourceModel, SIGNAL(layoutChanged()),
        this, SLOT(layoutChanged()));
        
    connect(sourceModel, SIGNAL(modelReset()),
        this, SLOT(modelReset()));
        
    connect(sourceModel, SIGNAL(rowsAboutToBeInserted(const QModelIndex&, int, int)),
        this, SLOT(rowsAboutToBeInserted(const QModelIndex&, int, int)));
        
    connect(sourceModel, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)),
        this, SLOT(rowsAboutToBeRemoved(const QModelIndex&, int, int)));
        
    connect(sourceModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
        this, SLOT(rowsInserted(const QModelIndex&, int, int)));
        
    connect(sourceModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
        this, SLOT(rowsRemoved(const QModelIndex&, int, int)));
}

void AggregatedPlacesQueryModel::removeSourceModel(QAbstractItemModel *sourceModel)
{
    if(!d->m_sourceModels.contains(sourceModel)) {
        return;
    }
    
    disconnect(sourceModel, SIGNAL(void dataChanged(const QModelIndex&, const QModelIndex&)),
        this, SLOT(void dataChanged(const QModelIndex&, const QModelIndex&)));
            
    disconnect(sourceModel, SIGNAL(void layoutChanged()),
        this, SLOT(void layoutChanged()));
        
    disconnect(sourceModel, SIGNAL(void modelReset()),
        this, SLOT(void modelReset()));
        
    disconnect(sourceModel, SIGNAL(void rowsAboutToBeInserted(const QModelIndex&, int, int)),
        this, SLOT(void rowsAboutToBeInserted(const QModelIndex&, int, int)));
        
    disconnect(sourceModel, SIGNAL(void rowsAboutToBeRemoved(const QModelIndex&, int, int)),
        this, SLOT(void rowsAboutToBeRemoved(const QModelIndex&, int, int)));
        
    disconnect(sourceModel, SIGNAL(void rowsInserted(const QModelIndex&, int, int)),
        this, SLOT(void rowsInserted(const QModelIndex&, int, int)));
        
    disconnect(sourceModel, SIGNAL(void rowsRemoved(const QModelIndex&, int, int)),
        this, SLOT(void rowsRemoved(const QModelIndex&, int, int)));
    
    int row = d->m_sourceModels.indexOf(sourceModel);
    
    beginRemoveRows(QModelIndex(), row, row);
    
    d->m_sourceModels.removeOne(sourceModel);
    
    foreach(QPersistentModelIndex sourceIndex, d->m_decendents[sourceModel]) {
        d->m_parents.remove(sourceIndex);
        
        QString* internalId = d->m_reverseSourceIndexes[sourceIndex];
        d->m_sourceIndexes.remove(*internalId);
        d->m_reverseSourceIndexes.remove(sourceIndex);
        delete internalId;
    }
    
    d->m_decendents.remove(sourceModel);
    d->m_sourceModelNames.remove(sourceModel);
    
    endRemoveRows();
}

int AggregatedPlacesQueryModel::rowCount(const QModelIndex &parent) const
{   
    // If it's invalid we want to count the top level indexes i.e. the models
    if(!parent.isValid()) {
        kDebug() << parent << "d->m_sourceModels.count():" << d->m_sourceModels.count();
        return d->m_sourceModels.count();
    }
    
    const QAbstractItemModel *sourceModel = sourceOf(parent);
    
    if(!sourceModel) {
        kDebug() << parent << "zero:" << 0;
        return 0;
    }
    
    kDebug() << parent << "sourceModel->rowCount(mapToSource(parent)): " << sourceModel->rowCount(mapToSource(parent));
    return sourceModel->rowCount(mapToSource(parent));
}

QVariant AggregatedPlacesQueryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ( role == Qt::DisplayRole && orientation == Qt::Horizontal )
    {
        switch(section) {
        case 0:
            return i18nc( "@title:column, display role", "Display Role" );
        default:
            return QString();
        }
    }
    
    return QVariant();
}

QVariant AggregatedPlacesQueryModel::data(const QModelIndex &index, int role) const
{
    
    if(!index.isValid()) {
        kDebug() << "!index.isValid() : QVariant()" << index;
        return QVariant();
    }
    
    const QAbstractItemModel *sourceModel = sourceOf(index);
    
    if(!sourceModel) {
        kDebug() << "!sourceModel : QVariant()" << index;
        return QVariant();
    }
    
    switch(role) {
    case Place::PlaceRelevanceRole:
        return QVariant(); // TODO
    case Place::PlaceUrlRole:
        return mapToSource(index).data(role);
    case Qt::DisplayRole:
    case Qt::EditRole:
        // top level, show name
        if(!index.parent().isValid()) {
            return d->m_sourceModelNames[sourceModel];
        }
        
        kDebug() << index << role;
        kDebug() << "mapToSource(index).data()" << mapToSource(index) << mapToSource(index).data();
        return mapToSource(index).data();
    default:
        return QVariant();
    }
}
    
QModelIndex AggregatedPlacesQueryModel::index(int row, int column, const QModelIndex& parent) const
{   
    kDebug() << row << column << parent;
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
    QPersistentModelIndex sourceIndex = sourceModel->index(row, column, sourceParent);
    QString *internalId;  //TODO: free the QString mallocs
    if(d->m_reverseSourceIndexes.contains(sourceIndex)) {
        kDebug()  << "notnew" << d->m_reverseSourceIndexes[sourceIndex];
        internalId = d->m_reverseSourceIndexes[sourceIndex];
    } else {
        internalId = new QString(KRandom::randomString( 10 ));
        kDebug() << "new" << internalId << *internalId;
    }
    
    QModelIndex ret = createIndex(row, column, reinterpret_cast<void*>(internalId));
    kDebug() << ret.internalPointer();
    internalId = reinterpret_cast<QString*>(ret.internalPointer());
    kDebug() << internalId << *internalId << ret;
    kDebug() << "sourceIndex =" << sourceIndex;
    
    if(!d->m_reverseSourceIndexes.contains(sourceIndex)) {
        kDebug() << "filling";
        d->m_reverseSourceIndexes[sourceIndex] = internalId;
        Q_ASSERT(d->m_reverseSourceIndexes.contains(sourceIndex));
        d->m_sourceIndexes[*internalId] = sourceIndex;
        d->m_parents[ret] = parent;
        d->m_decendents[sourceModel].append(ret);
    }
    return ret;
}
    
QModelIndex AggregatedPlacesQueryModel::parent(const QModelIndex& index) const
{   
    if(!index.isValid() || !d->m_parents.contains(index)) {
        kDebug() << "!index.isValid() || !d->m_parents.contains(index)" << index << d->m_parents.contains(index);
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
    kDebug() << sourceIndex << sourceModel;
    
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
    
    kDebug() << "sModel: " << sModel;
    
    Q_ASSERT(sourceModel != 0 && sModel == sourceModel);
    
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

    QString* internalId = reinterpret_cast<QString*>(index.internalPointer());
    
    kDebug() << index << internalId;
    

    if(!internalId) {
        return QModelIndex();
    }
    
    kDebug() << *internalId;
    
    if(!d->m_sourceIndexes.contains(*internalId)) {
        kDebug() << "not found";
        return QModelIndex();
    }
    
    return d->m_sourceIndexes[*internalId];
}

const QAbstractItemModel* AggregatedPlacesQueryModel::sourceOf(const QModelIndex& index) const
{
    kDebug() << index;
    if(!index.isValid()) {
        kDebug() << "!index.isValid()";
        return 0;
    }
    
    if(!index.parent().isValid()) {
        if(index.row() < 0 || index.row() >= d->m_sourceModels.count()) {
            kDebug() << "index.row() = " << index.row();
            return 0;
        }
        kDebug() << "d->m_sourceModels.at(index.row())" << d->m_sourceModels.at(index.row()) << index.row();
        
        return d->m_sourceModels.at(index.row());
    }
    
    QModelIndex sourceIndex =  mapToSource(index);
    
    kDebug() << "sourceIndex = " << sourceIndex;
    
    if(!sourceIndex.isValid()) {
        return 0;
    }
    
    return sourceIndex.model();
}

#include "aggregatedplacesquerymodel.moc"
