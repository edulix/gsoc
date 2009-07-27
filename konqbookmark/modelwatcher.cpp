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
#include <akonadi/entitytreemodel.h>

using namespace Akonadi;

ModelWatcher::ModelWatcher(Akonadi::Entity::Id entityId,
    QAbstractItemModel *model, QObject *parent)
    
    : QObject(parent), m_entityId(entityId), m_model(model), m_watch(false), 
    m_entityIndex(QModelIndex())
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

void ModelWatcher::setWatchedEntity(Entity::Id entityId)
{
    m_entityId = entityId;
    m_entityIndex = QModelIndex();
}

Entity::Id ModelWatcher::watchedEntity() const
{
    return m_entityId;
}

void ModelWatcher::setWatchedModel(QAbstractItemModel *model)
{
    setWatch(false);
    m_model = model;
    m_entityId = 0;
    m_entityIndex = QModelIndex();
    setWatch(true);
}


QAbstractItemModel *ModelWatcher::watchedModel() const
{
    return m_model;
}

QModelIndex ModelWatcher::entityIndex() const
{
    return m_entityIndex;
}
    
void ModelWatcher::rowsInserted(const QModelIndex &parent, int start, int end)
{
    const int column = 0;
    for (int row = start; row <= end; ++row)
    {
        QModelIndex idx = m_model->index(row, column, parent);
        if ((idx.data(EntityTreeModel::ItemIdRole).toLongLong() != m_entityId)
            && (idx.data(EntityTreeModel::CollectionIdRole).toLongLong() != m_entityId))
            continue;

        m_entityIndex = idx;

        // stop watching the model.
        setWatch(false);
        
        emit newEntity(idx);
    }
}
