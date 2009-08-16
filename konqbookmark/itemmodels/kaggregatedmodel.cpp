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

#include "kaggregatedmodel.h"
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
class KAggregatedModel::Private
{
public:
    Private(KAggregatedModel* parent);
    ~Private();
    
    void dataChanged(const QModelIndex& sourceTopLeft, const QModelIndex& sourceBottomRight);
    void layoutChanged();
    void modelReset();
    void rowsInserted(const QModelIndex& sourceParent, int start, int end);
    void rowsRemoved(const QModelIndex& sourceParent, int start, int end);
    
    KAggregatedModel* q;
    QList<const QAbstractItemModel*> m_sourceModels;
    QHash<const QAbstractItemModel*, QString> m_sourceModelNames;
    QHash<const QAbstractItemModel*, int> m_sourceModelDisplayColumns;
    QHash<const QAbstractItemModel*, QList<QPersistentModelIndex> > m_decendants;
    
    QHash<QString, QPersistentModelIndex> m_sourceIndexes;
    QHash<QPersistentModelIndex, QString*> m_reverseSourceIndexes;
    bool m_removingIndexesFlag;
    QHash<QPersistentModelIndex, QPersistentModelIndex> m_parents;
};

KAggregatedModel::Private::Private(KAggregatedModel* parent)
    : q(parent), m_removingIndexesFlag(false)
{
    
}

KAggregatedModel::Private::~Private()
{

}


void KAggregatedModel::Private::dataChanged(const QModelIndex& sourceTopLeft,
    const QModelIndex& sourceBottomRight)
{
    QAbstractItemModel* sourceModel = qobject_cast<QAbstractItemModel*>(q->sender());
    QModelIndex topLeft = q->mapFromSource(sourceTopLeft, sourceModel);
    QModelIndex bottomRight = q->mapFromSource(sourceBottomRight, sourceModel);
    emit q->dataChanged(topLeft, bottomRight);
}

void KAggregatedModel::Private::layoutChanged()
{   
    emit q->layoutChanged();
}

void KAggregatedModel::Private::modelReset()
{
    layoutChanged();
}

void KAggregatedModel::Private::rowsInserted(const QModelIndex& sourceParent, int start, int end)
{
    
    QAbstractItemModel* sourceModel = qobject_cast<QAbstractItemModel*>(q->sender());
    QModelIndex parent = q->mapFromSource(sourceParent, sourceModel);
    
    q->beginInsertRows(parent, start, end);
    q->endInsertRows();
}


void KAggregatedModel::Private::rowsRemoved(const QModelIndex& sourceParent, int start, int end)
{
    
    QAbstractItemModel* sourceModel = qobject_cast<QAbstractItemModel*>(q->sender());
    QModelIndex parent = q->mapFromSource(sourceParent, sourceModel);
 
    m_removingIndexesFlag = true;
    q->beginRemoveRows(parent, start, end);
    
    
    for(int i = start; i <= end; i++) {
        QModelIndex sourceIndex = sourceModel->index(i, 0, sourceParent);
        m_reverseSourceIndexes.remove(sourceIndex);
        m_parents.remove(sourceIndex);
        
        QString* internalId = m_reverseSourceIndexes[sourceIndex];
        if(internalId) {
            m_sourceIndexes.remove(*internalId);
            delete internalId;
        }
    }
    
    q->endRemoveRows();
    m_removingIndexesFlag = false;
}
//@endcond
    
KAggregatedModel::KAggregatedModel(QObject* parent)
    : QAbstractItemModel(parent), d(new Private(this))
{

}
    
KAggregatedModel::~KAggregatedModel()
{
    delete d;
}
    
void KAggregatedModel::addSourceModel(QAbstractItemModel *sourceModel, QString name, int displayColumn)
{   
    if(d->m_sourceModels.contains(sourceModel)) {
        return;
    }
    
    d->m_sourceModelNames[sourceModel] = name;
    
    if(displayColumn != -1) {
        d->m_sourceModelDisplayColumns[sourceModel] = displayColumn;
    }
    
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
        
    connect(sourceModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
        this, SLOT(rowsInserted(const QModelIndex&, int, int)));
        
    connect(sourceModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
        this, SLOT(rowsRemoved(const QModelIndex&, int, int)));
}

void KAggregatedModel::removeSourceModel(QAbstractItemModel *sourceModel)
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
    
    foreach(QPersistentModelIndex sourceIndex, d->m_decendants[sourceModel]) {
        d->m_parents.remove(sourceIndex);
        
        QString* internalId = d->m_reverseSourceIndexes[sourceIndex];
        d->m_sourceIndexes.remove(*internalId);
        d->m_parents.remove(sourceIndex);
        d->m_reverseSourceIndexes.remove(sourceIndex);
        delete internalId;
    }
    
    d->m_decendants.remove(sourceModel);
    d->m_sourceModelNames.remove(sourceModel);
    d->m_sourceModelDisplayColumns.remove(sourceModel);
    
    endRemoveRows();
}

int KAggregatedModel::rowCount(const QModelIndex &parent) const
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

QVariant KAggregatedModel::headerData(int section, Qt::Orientation orientation, int role) const
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

QVariant KAggregatedModel::data(const QModelIndex &index, int role) const
{
    
    if(!index.isValid()) {
        return QVariant();
    }
    
    const QAbstractItemModel *sourceModel = sourceOf(index);
    
    if(!sourceModel) {
        return QVariant();
    }
    
    int col  = 0;
    switch(role) {
    case Qt::DisplayRole:
        if(!index.parent().isValid()) {
            return d->m_sourceModelNames[sourceModel];
        }
        
        if(!d->m_sourceModelDisplayColumns.contains(sourceModel)) {
            return mapToSource(index).data(role);
        }
        
        col = d->m_sourceModelDisplayColumns[sourceModel];
        
        return mapToSource(index).child(0, col).data();
    default:
        return mapToSource(index).data(role);
    }
}
    
QModelIndex KAggregatedModel::index(int row, int column, const QModelIndex& parent) const
{      
    if(column != 0) {
        return QModelIndex();
    }
    
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
    
    QModelIndex sourceParent = mapToSource(parent);
    QPersistentModelIndex sourceIndex = sourceModel->index(row, column, sourceParent);
    QString *internalId;  //TODO: free the QString mallocs
    if(d->m_reverseSourceIndexes.contains(sourceIndex)) {
        internalId = d->m_reverseSourceIndexes[sourceIndex];
    } else {
        if(d->m_removingIndexesFlag) {
            return QModelIndex();
        }
        internalId = new QString(KRandom::randomString( 10 ));
    }
    
    QModelIndex ret = createIndex(row, column, reinterpret_cast<void*>(internalId));
    
    if(!d->m_reverseSourceIndexes.contains(sourceIndex) && !d->m_removingIndexesFlag) {
        d->m_reverseSourceIndexes[sourceIndex] = internalId;
        d->m_sourceIndexes[*internalId] = sourceIndex;
        d->m_parents[sourceIndex] = parent;
        d->m_decendants[sourceModel].append(ret);
    }
    return ret;
}
    
QModelIndex KAggregatedModel::parent(const QModelIndex& index) const
{
    if(!index.isValid()) {
        return QModelIndex();
    }
    
    QModelIndex sourceIndex = mapToSource(index);
    
    if(!sourceIndex.isValid() || !d->m_parents.contains(sourceIndex)) {
        return QModelIndex();
    }
 
    return d->m_parents[sourceIndex];
}
    
int KAggregatedModel::columnCount(const QModelIndex& index) const
{
    Q_UNUSED(index);
    return ColumnCount;
}
        
QModelIndex KAggregatedModel::mapFromSource(const QModelIndex& sourceIndex,
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

QModelIndex KAggregatedModel::mapToSource(const QModelIndex& index) const
{   
    if(!index.isValid()) {
        return QModelIndex();
    }

    QString* internalId = reinterpret_cast<QString*>(index.internalPointer()); 

    if(!internalId) {
        return QModelIndex();
    }
    
    if(!d->m_sourceIndexes.contains(*internalId)) {
        return QModelIndex();
    }
    return d->m_sourceIndexes[*internalId];
}

const QAbstractItemModel* KAggregatedModel::sourceOf(const QModelIndex& index) const
{
    if(!index.isValid()) {
        return 0;
    }
    
    if(!index.parent().isValid()) {
        if(index.row() < 0 || index.row() >= d->m_sourceModels.count()) {
            return 0;
        }
        
        return d->m_sourceModels.at(index.row());
    }
    
    QModelIndex sourceIndex =  mapToSource(index);
    
    if(!sourceIndex.isValid()) {
        return 0;
    }
    
    return sourceIndex.model();
}

#include "kaggregatedmodel.moc"
