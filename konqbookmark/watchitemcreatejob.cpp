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
#include "watchitemcreatejob.h"

#include <kdebug.h>

using namespace Akonadi;

WatchItemCreateJob::WatchItemCreateJob( Item &item, const Collection &collection, QAbstractItemModel *model, QObject *parent )
    : Job(parent), m_item(item), m_collection(collection), m_model(model)
{
}

WatchItemCreateJob::~WatchItemCreateJob()
{
}

Item WatchItemCreateJob::item() const
{
    if(!m_itemCreateJob)
        return Item();
    
    return m_itemCreateJob->item();
}

QModelIndex WatchItemCreateJob::itemIndex() const
{
    if(!m_modelWatcher)
        return QModelIndex();
    
    return m_modelWatcher->entityIndex();
}

void WatchItemCreateJob::doStart()
{
    kDebug();
    m_itemCreateJob = new ItemCreateJob(m_item, m_collection, this);
    connect(m_itemCreateJob, SIGNAL(finished(KJob *)), this, SLOT(slotItemCreated()));
}

void WatchItemCreateJob::slotItemCreated()
{
    kDebug();
    m_item = m_itemCreateJob->item(); // updates the item id
    m_modelWatcher = new ModelWatcher(m_item.id(), m_model, this);
    connect(m_modelWatcher, SIGNAL(newItem(const QModelIndex &)), this, SLOT(slotItemAdded()));
    if(m_itemCreateJob->error())
    {
        qWarning() << m_itemCreateJob->errorString();
        emitResult();
        return;
    }
}

void WatchItemCreateJob::slotItemAdded()
{
    kDebug();
    emitResult();
}

