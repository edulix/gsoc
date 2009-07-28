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

#ifndef WATCHITEMCREATEJOB_H
#define WATCHITEMCREATEJOB_H

#include "konqbookmark_export.h"
#include "modelwatcher.h"

#include <QObject>
#include <QAbstractItemModel>
#include <akonadi/item.h>
#include <akonadi/collection.h>
#include <akonadi/job.h>
#include <akonadi/itemcreatejob.h>

namespace Akonadi {

/**
 * @short Job that creates a new item in the Akonadi storage and returns its index in a given model.
 *
 * This job creates a new item with all the set properties in the
 * given target collection, watches for it to be added in a given model and
 * returns the index for that item in the model.
 *
 * Example:
 *
 * @code
 *
 * // Create a contact item in the root collection and select it in the contact view
 *
 * KABC::Addressee addr;
 * addr.setNameFromString( "Joe Jr. Miller" );
 *
 * Akonadi::Item item;
 * item.setMimeType( "text/directory" );
 * item.setPayload<KABC::Addressee>( addr );
 *
 * Akonadi::Collection collection = Akonadi::Collection::root();
 *
 * Akonadi::WatchItemCreateJob *job = new Akonadi::WatchItemCreateJob( item, collection, contactModel );
 *
 * if ( job->exec() )
 * {
 *   qDebug() << "Contact item created and added to the model";
 *   contactView->setSelectedIndex(job->itemIndex());
 * } else
 *   qDebug() << "Error occurred";
 *
 * @endcode
 *
 * @author Eduardo Robles Elvira <edulix@gmail.com>
 * @author Volker Krause <vkrause@kde.org>
 */
class KONQBOOKMARK_EXPORT WatchItemCreateJob : public Job
{
    Q_OBJECT

public:
    /**
     * Creates a new item create job.
     *
     * @param item The item to create.
     *             @note It must have a mime type set.
     * @param collection The parent collection where the new item shall be located in.
     * @param parent The parent object.
     */
    WatchItemCreateJob( Akonadi::Item &item, const Akonadi::Collection &collection, QAbstractItemModel *model, QObject *parent = 0 );

    /**
     * Destroys the item create job.
     */
    ~WatchItemCreateJob();

    /**
     * Returns the created item with the new unique id, or an invalid item if the job failed.
     */
    Akonadi::Item item() const;
    
    /**
     * Returns the created item with the new unique id, or an invalid item if the job failed.
     */
    QModelIndex itemIndex() const;

protected:
    virtual void doStart();

protected Q_SLOTS:
    /**
     * Called by m_itemCreateJob when it finishes.
     */
    void slotItemCreated();
    
    /**
     * Called by m_modelWatcher when it finishes, slotItemCreated() must have been
     * called before that happens.
     */
    void slotItemAdded();

private:
    Akonadi::ItemCreateJob *m_itemCreateJob;
    ModelWatcher *m_modelWatcher;
    Akonadi::Item &m_item;
    const Akonadi::Collection &m_collection;
    QAbstractItemModel *m_model;
};

}

#endif // WATCHITEMCREATEJOB_H
