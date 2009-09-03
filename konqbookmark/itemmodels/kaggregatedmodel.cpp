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
    void rowsAboutToBeInserted(const QModelIndex& sourceParent, int start, int end);
    void rowsAboutToBeRemoved(const QModelIndex& sourceParent, int start, int end);
    void rowsInserted(const QModelIndex& sourceParent, int start, int end);
    void rowsRemoved(const QModelIndex& sourceParent, int start, int end);
    
    int indexRowDiff(const QAbstractItemModel *sourceModel, int sourceRow);
    
    KAggregatedModel* q;
    QList<const QAbstractItemModel*> m_sourceModels;
};

KAggregatedModel::Private::Private(KAggregatedModel* parent)
    : q(parent)
{
    
}

KAggregatedModel::Private::~Private()
{

}


void KAggregatedModel::Private::dataChanged(const QModelIndex& sourceTopLeft,
    const QModelIndex& sourceBottomRight)
{
    QModelIndex topLeft = q->mapFromSource(sourceTopLeft);
    QModelIndex bottomRight = q->mapFromSource(sourceBottomRight);
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

int KAggregatedModel::Private::indexRowDiff(const QAbstractItemModel *sourceModel, int sourceRow)
{
    int prevCount = 0;
    int modelAt = m_sourceModels.indexOf(sourceModel);
    
    if (modelAt == -1) {
        return -1;
    }
    
    foreach (const QAbstractItemModel *model, m_sourceModels) {
        if (model == sourceModel) {
            break;
        }
        prevCount += model->rowCount(QModelIndex());
    }
    
    return sourceRow + prevCount;
}

void KAggregatedModel::Private::rowsAboutToBeInserted(const QModelIndex& sourceParent, int sourceStart, int sourceEnd)
{
    if (sourceParent.isValid()) {
        return;
    }
    
    if (sourceStart < 0 || sourceStart > sourceEnd) {
       return;
    }
    
    QAbstractItemModel* sourceModel = qobject_cast<QAbstractItemModel*>(q->sender());
    
    int diff = indexRowDiff(sourceModel, 0);
    
    int rowCount = q->rowCount(QModelIndex());
    if (diff < 0 || sourceStart + diff > rowCount) {
        return;
    }
    int start = sourceStart + diff;
    int end = sourceEnd + diff;
    
    q->beginInsertRows(QModelIndex(), start, end);
}

void KAggregatedModel::Private::rowsInserted(const QModelIndex& sourceParent, int sourceStart, int sourceEnd)
{
    if (sourceParent.isValid()) {
        return;
    }
    
    if (sourceStart < 0 || sourceStart > sourceEnd) {
       return;
    }
    
    QAbstractItemModel* sourceModel = qobject_cast<QAbstractItemModel*>(q->sender());

    int diff = indexRowDiff(sourceModel, 0);
    int rowCount = q->rowCount(QModelIndex());
    if (diff < 0 || sourceEnd + diff > rowCount) {
        return;
    }
    
    q->endInsertRows();
}

void KAggregatedModel::Private::rowsAboutToBeRemoved(const QModelIndex& sourceParent, int sourceStart, int sourceEnd)
{
    if (sourceParent.isValid()) {
        return;
    }
    
    if (sourceStart < 0 || sourceStart > sourceEnd) {
       return;
    }
    
    QAbstractItemModel* sourceModel = qobject_cast<QAbstractItemModel*>(q->sender());

    int diff = indexRowDiff(sourceModel, 0);
    int rowCount = q->rowCount(QModelIndex());
    if (diff < 0 || sourceStart + diff > rowCount) {
        return;
    }
    
    int start = sourceStart + diff;
    int end = sourceEnd + diff;
    
    q->beginRemoveRows(QModelIndex(), start, end);
}

void KAggregatedModel::Private::rowsRemoved(const QModelIndex& sourceParent, int sourceStart, int sourceEnd)
{
    if (sourceParent.isValid()) {
        return;
    }
    
    if (sourceStart < 0 || sourceStart > sourceEnd) {
       return;
    }
    
    QAbstractItemModel* sourceModel = qobject_cast<QAbstractItemModel*>(q->sender());

    int diff = indexRowDiff(sourceModel, sourceStart);
    int rowCount = q->rowCount(QModelIndex());
    if (diff < 0 || sourceEnd + diff > rowCount) {
        return;
    }
    
    q->endRemoveRows();
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
    
void KAggregatedModel::addSourceModel(QAbstractItemModel *sourceModel)
{
    if (!sourceModel) {
        return;
    }
    
    if (d->m_sourceModels.contains(sourceModel)) {
        return;
    }
    
    int start = rowCount(QModelIndex());
    beginInsertRows(QModelIndex(), start, start + sourceModel->rowCount(QModelIndex()));
    
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

void KAggregatedModel::removeSourceModel(QAbstractItemModel *sourceModel)
{
    if (!d->m_sourceModels.contains(sourceModel)) {
        return;
    }
    
    disconnect(sourceModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
        this, SLOT(dataChanged(const QModelIndex&, const QModelIndex&)));
            
    disconnect(sourceModel, SIGNAL(layoutChanged()),
        this, SLOT(layoutChanged()));
        
    disconnect(sourceModel, SIGNAL(modelReset()),
        this, SLOT(modelReset()));
        
    disconnect(sourceModel, SIGNAL(rowsAboutToBeInserted(const QModelIndex&, int, int)),
        this, SLOT(rowsAboutToBeInserted(const QModelIndex&, int, int)));
        
    disconnect(sourceModel, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)),
        this, SLOT(rowsAboutToBeRemoved(const QModelIndex&, int, int)));
        
    disconnect(sourceModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
        this, SLOT(rowsInserted(const QModelIndex&, int, int)));
        
    disconnect(sourceModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
        this, SLOT(rowsRemoved(const QModelIndex&, int, int)));
    
    int row = d->indexRowDiff(sourceModel, 0);
    int rowCount = sourceModel->rowCount(QModelIndex());
    
    beginRemoveRows(QModelIndex(), row, row + rowCount);
    endRemoveRows();
}

int KAggregatedModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    
    if (!d->m_sourceModels.isEmpty()) {
        const QAbstractItemModel *model = d->m_sourceModels.last();
        
        return d->indexRowDiff(model, model->rowCount(QModelIndex()));
    }
    
    return 0;
}

QVariant KAggregatedModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
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
    if (!index.isValid()) {
        return QVariant();
    }
    
    const QModelIndex sourceIndex = mapToSource(index);
    if (!sourceIndex.isValid()) {
        return QVariant();
    }
    
    switch (role) {
    case Qt::DisplayRole:
        return sourceIndex.data(role);
    default:
        return QVariant();
    }
}
    
QModelIndex KAggregatedModel::index(int row, int column, const QModelIndex& parent) const
{
    if (column != 0 || parent.isValid()) {
        return QModelIndex();
    }
    
    if (row < 0 || row > rowCount(QModelIndex())) {
        return QModelIndex();
    }
    
    return createIndex(row, column);
}
    
QModelIndex KAggregatedModel::parent(const QModelIndex& index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}
    
int KAggregatedModel::columnCount(const QModelIndex& index) const
{
    Q_UNUSED(index);
    return ColumnCount;
}
        
QModelIndex KAggregatedModel::mapFromSource(const QModelIndex& sourceIndex) const
{   
    if (!sourceIndex.isValid() || sourceIndex.parent().isValid()) {
        return QModelIndex();
    }
    
    if (!d->m_sourceModels.contains(sourceIndex.model())) {
        return QModelIndex();
    }
    
    int diff = d->indexRowDiff(sourceIndex.model(), sourceIndex.row());
    
    return index(diff + sourceIndex.row(), 0);
}

QModelIndex KAggregatedModel::mapToSource(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }
    
    int prevRows = 0;
    
    foreach (const QAbstractItemModel *model, d->m_sourceModels) {
        int rowCount = model->rowCount(QModelIndex());
        if (index.row() < prevRows + rowCount) {
            return model->index(index.row() - prevRows, 0);
        }
        prevRows += rowCount;
    }
    
    return QModelIndex();
}

#include "kaggregatedmodel.moc"
