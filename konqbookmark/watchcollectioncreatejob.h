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

#ifndef WATCHCOLLECTIONCREATEJOB_H
#define WATCHCOLLECTIONCREATEJOB_H

#include "konqbookmark_export.h"
#include "modelwatcher.h"

#include <QObject>
#include <QAbstractItemModel>
#include <akonadi/item.h>
#include <akonadi/collection.h>
#include <akonadi/job.h>
#include <akonadi/collectioncreatejob.h>

namespace Akonadi {

/**
 * @short Job that creates a new collection in the Akonadi storage and returns its index in a given model.
 *
 * This job creates a new item with all the set properties in the
 * given target collection, watches for it to be added in a given model and
 * returns the index for that item in the model.
 *
 * Example:
 *
 * @code
 *
 * Akonadi::Collection collection;
 * collection.setParent( parent );
 * collection.setName( folderName );
 * collection.setContentMimeTypes( QStringList( KonqBookmark::mimeType() ) );
 *
 * Akonadi::WatchCollectionCreateJob *job = new Akonadi::WatchCollectionCreateJob( collection, contactModel );
 *
 * if ( job->exec() )
 *   qDebug() << "Collection created and added to the model";
 * else
 *   qDebug() << "Error occurred";
 *
 * @endcode
 *
 * @author Eduardo Robles Elvira <edulix@gmail.com>
 * @author Volker Krause <vkrause@kde.org>
 */
class KONQBOOKMARK_EXPORT WatchCollectionCreateJob : public Job
{
    Q_OBJECT

public:
    /**
     * Creates a new collection create job.
     *
     * @param collection The collection to create.
     * @param parent The parent object.
     */
    WatchCollectionCreateJob( Akonadi::Collection &collection, QAbstractItemModel *model, QObject *parent = 0 );

    /**
     * Destroys the item create job.
     */
    ~WatchCollectionCreateJob();

    /**
     * Returns the created collection with the new unique id, or an invalid
     * collection if the job failed  or was not created yet.
     */
    Akonadi::Collection collection() const;
    
    /**
     * Returns the index of the created collection with the new unique id, or
     * an invalid collection if the job failed or was not created yet.
     */
    QModelIndex collectionIndex() const;

protected:
    virtual void doStart();

protected Q_SLOTS:
    /**
     * Called by m_collectionCreateJob when it finishes.
     */
    void slotCollectionCreated();
    
    /**
     * Called by m_modelWatcher when it finishes, slotCollectionCreated() must 
     * have been called before that happens.
     */
    void slotItemAdded();

private:
    Akonadi::CollectionCreateJob *m_collectionCreateJob;
    ModelWatcher *m_modelWatcher;
    Akonadi::Collection &m_collection;
    QAbstractItemModel *m_model;
};

}

#endif // WATCHCOLLECTIONCREATEJOB_H
