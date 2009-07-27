/*
    Copyright (c) 2006 - 2007 Volker Krause <vkrause@kde.org>
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

#include "modelwatcher.h"
#include "watchcollectioncreatejob.h"

#include <kdebug.h>

using namespace Akonadi;

WatchCollectionCreateJob::WatchCollectionCreateJob( Collection &collection, QAbstractItemModel *model, QObject *parent )
    : Job(parent), m_collection(collection), m_model(model)
{
}

WatchCollectionCreateJob::~WatchCollectionCreateJob()
{
}

Collection WatchCollectionCreateJob::collection() const
{
    if(!m_collectionCreateJob)
        return Collection();
    
    return m_collectionCreateJob->collection();
}

QModelIndex WatchCollectionCreateJob::collectionIndex() const
{
    if(!m_modelWatcher)
        return QModelIndex();
    
    return m_modelWatcher->entityIndex();
}

void WatchCollectionCreateJob::doStart()
{
    kDebug();
    m_collectionCreateJob = new CollectionCreateJob(m_collection, this);
    connect(m_collectionCreateJob, SIGNAL(finished(KJob *)), this, SLOT(slotCollectionCreated()));
}

void WatchCollectionCreateJob::slotCollectionCreated()
{
    kDebug();
    m_collection = m_collectionCreateJob->collection(); // updates the collection id
    m_modelWatcher = new ModelWatcher(m_collection.id(), m_model, this);
    connect(m_modelWatcher, SIGNAL(newEntity(const QModelIndex &)), this, SLOT(slotCollectionAdded()));
    if(m_collectionCreateJob->error())
    {
        qWarning() << m_collectionCreateJob->errorString();
        emitResult();
        return;
    }
}

void WatchCollectionCreateJob::slotItemAdded()
{
    kDebug();
    emitResult();
}

