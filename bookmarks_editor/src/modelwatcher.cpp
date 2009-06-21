/*
    Copyright (c) 2009 Stephen Kelly <steveire@gmail.com>
    Copyright (c) 2009 Eduardo Robles Elvira <edulix@gmail.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "modelwatcher.h"
#include <akonadi_next/entitytreemodel.h>

using namespace Akonadi;

ModelWatcher::ModelWatcher(Akonadi::Item::Id itemId, Akonadi::Collection::Id colId,
    QAbstractItemModel *model, QObject *parent)
    
    : QObject(parent), m_itemId(itemId), m_colId(colId), m_model(model),
        m_watch(false), m_itemIndex(QModelIndex())
{
    setWatch(true);
}

ModelWatcher::~ModelWatcher()
{
}

void ModelWatcher::setWatch(bool watch)
{
    if(m_watch == watch)
        return;
    
    Q_ASSERT(m_model);
    if (watch)
        connect(m_model, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
            this, SLOT(rowsInserted(const QModelIndex &, int, int)));
    else
        disconnect(m_model, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
            this, SLOT(rowsInserted(const QModelIndex &, int, int)));
            
    m_watch = watch;
}

bool ModelWatcher::watch() const
{
    return m_watch;
}

void ModelWatcher::setWatchedItem(Item::Id itemId)
{
    m_itemId = itemId;
    m_itemIndex = QModelIndex();
}

Item::Id ModelWatcher::watchedItem() const
{
    return m_itemId;
}

void ModelWatcher::setWatchedCollection( Collection::Id colId )
{
    m_colId = colId;
    m_itemIndex = QModelIndex();
}

Collection::Id ModelWatcher::watchedCollection() const
{
    return m_colId;
}

void ModelWatcher::setWatchedModel(QAbstractItemModel *model)
{
    setWatch(false);
    m_model = model;
    m_itemIndex = QModelIndex();
    setWatch(true);
}


QAbstractItemModel *ModelWatcher::watchedModel() const
{
    return m_model;
}

QModelIndex ModelWatcher::itemIndex() const
{
    return m_itemIndex;
}
    
void ModelWatcher::rowsInserted(const QModelIndex &parent, int start, int end)
{
    kDebug();
//     if (parent.data(EntityTreeModel::CollectionIdRole).toLongLong() != m_colId)
//         return;

    const int column = 0;
    for (int row = start; row <= end; ++row)
    {
        QModelIndex idx = m_model->index(row, column, parent);
        kDebug() << "row = " << row << ", m_itemId = " << m_itemId << ", idx.data(EntityTreeModel::ItemIdRole).toLongLong() = " << idx.data(EntityTreeModel::ItemIdRole).toLongLong();
        if (idx.data(EntityTreeModel::ItemIdRole).toLongLong() != m_itemId)
            continue;

        m_itemIndex = idx;

        // stop watching the model.
        setWatch(false);
        
        emit newItem(idx);
    }
}
