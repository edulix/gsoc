/*
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

#include "konqbookmarkmodel.h"
#include "konqbookmark.h"
#include "place.h"

#include <akonadi/collection.h>
#include <akonadi/item.h>
#include <akonadi/itemfetchjob.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemdeletejob.h>
#include <akonadi/itemmodifyjob.h>
#include <akonadi/itemmovejob.h>
#include <akonadi/itemcopyjob.h>
#include <akonadi/collectiondeletejob.h>
#include <akonadi/collectionmodifyjob.h>
#include <akonadi/collectioncopyjob.h>
#include <akonadi/transactionsequence.h>
#include <akonadi/monitor.h>
#include <akonadi/session.h>
#include <akonadi/itemfetchscope.h>
#include <Nepomuk/ResourceManager>

#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <KUrl>

#include <QtGui/QPixmap>
#include <QUrl>
#include <QMimeData>
#include <QVariant>

using namespace Akonadi;

/**
 * Private class that helps to provide binary compatibility between releases.
 * @internal
 */
//@cond PRIVATE
class KonqBookmarkModel::Private
{
    public:
};

//@endcond

KonqBookmarkModel::KonqBookmarkModel( Akonadi::Session *session, Akonadi::Monitor *monitor, QObject *parent )
  : EntityTreeModel( session, monitor, parent ),
    d( new Private() )
{
    Akonadi::ItemFetchScope scope;
    scope.fetchFullPayload( true );

    monitor->fetchCollection( true );
    monitor->setItemFetchScope( scope );
    monitor->setMimeTypeMonitored( mimeType() );
    monitor->setMimeTypeMonitored( Akonadi::Collection::mimeType() );
    // FIXME Set all monitored because otherwise the model doesn't get updates 
    // from items/collections being removed.
    monitor->setAllMonitored(true);
}

KonqBookmarkModel::~KonqBookmarkModel()
{
    delete d;
}

int KonqBookmarkModel::columnCount( const QModelIndex& index) const
{
    Q_UNUSED(index);
    
    return 9;
}

QVariant KonqBookmarkModel::getHeaderData( int section, Qt::Orientation orientation, int role, int headerSet ) const
{   
    if ( role == Qt::DisplayRole && orientation == Qt::Horizontal )
    {
        switch( section )
        {
        case Title:
            return i18nc( "@title:column, bookmark title", "Title" );
        case Url:
            return i18nc( "@title:column, bookmark address", "Address" );
        case UniqueUri:
            return i18nc( "@title:column, unique uri", "Uri" );
        case Tags:
            return i18nc( "@title:column, tags", "Tags" );
        case Description:
            return i18nc( "@title:column, description", "Description" );
        case NumVisits:
            return i18nc( "@title:column, visits counter", "Visits Counter" );
        case Created:
            return i18nc( "@title:column, created", "Created" );
        case LastModified:
            return i18nc( "@title:column, last modified", "Last Modified" );
        case LastVisited:
            return i18nc( "@title:column, last visited", "Last Visited" );
        default:
            return QString();
        }
    }
    
    return EntityTreeModel::getHeaderData( section, orientation, role, headerSet );
}


Qt::ItemFlags KonqBookmarkModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsDropEnabled;

    return EntityTreeModel::flags(index) | Qt::ItemIsEditable;
}

QVariant KonqBookmarkModel::getData( const Item &item, int column, int role ) const
{
    if ( item.mimeType() == mimeType() ) {
        if ( !item.hasPayload<KonqBookmark>() ) {

            // Pass modeltest
            if ( role == Qt::DisplayRole )
                return item.remoteId();

            return QVariant();
        }

        const KonqBookmark konqBookmark = item.payload<KonqBookmark>();
//         kDebug() << item.remoteId() << ", " << konqBookmark.uniqueUri();

        // Icon for the model entry
        switch( role )
        {
        case Qt::DecorationRole:
            if ( column == 0 )
            {
                return SmallIcon( QLatin1String( "bookmarks" ) );
            }
            return QVariant();
        case Konqueror::Place::PlaceUrlRole:
            return konqBookmark.url().toString();
        case Qt::DisplayRole:
        case Qt::EditRole:
            switch( column )
            {
            case Title:
                return konqBookmark.title();
            case Url:
                return konqBookmark.url().toString();
            case UniqueUri:
                return konqBookmark.uniqueUri();
            case Tags:
                return konqBookmark.tags().join(",");
            case Description:
                return konqBookmark.description();
            case NumVisits:
                return QString::number(konqBookmark.numVisits());
            case Created:
                return konqBookmark.created();
            case LastModified:
                return konqBookmark.lastModified();
            case LastVisited:
                return konqBookmark.lastVisited();
            default:
                return QString();
                break;
            }
            break;
        }
    }
    return EntityTreeModel::getData( item, column, role );
}

QVariant KonqBookmarkModel::getData( const Collection &collection, int column, int role ) const
{
    switch( role )
    {
    case Qt::DecorationRole:
        if ( column == 0 )
        {
            return SmallIcon( QLatin1String( "folder" ) );
        }
        return QVariant();
    case Konqueror::Place::PlaceUrlRole:
            return QVariant();
    case Qt::DisplayRole:
    case Qt::EditRole:
        switch( column )
        {
        case Title:
            return collection.name();
        case UniqueUri:
        case Url:
        case Tags:
        case Description:
        case NumVisits:
        case Created:
        case LastModified:
        case LastVisited:
        default:
            return QString();
        }
    // return QVariant for Qt::CheckState otherwise checkboxes are shown for most columns (except name)
    case Qt::CheckStateRole:
        return QVariant();
    }
    return EntityTreeModel::getData( collection, column, role );
}

bool KonqBookmarkModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // For EntityTree::getData(... EntityTreeModel::ItemRole) column's index must be 0,
    // same with EntityTree::setData(... EntityTreeModel::ItemRole), etc
    QModelIndex indexZero = EntityTreeModel::index(index.row(), 0, index.parent());
    
    if (index.isValid() && role == Qt::EditRole)
    {
        
        Item item = qVariantValue<Item>(data(indexZero, EntityTreeModel::ItemRole));
        Collection collection = qVariantValue<Collection>(data(indexZero, EntityTreeModel::CollectionRole));
        if ( item.isValid() )
        {
            if ( !item.hasPayload<KonqBookmark>() )
                return false;

            KonqBookmark konqBookmark = item.payload<KonqBookmark>();
            switch( index.column() )
            {
            case Title:
                // We'll only commit real changes. same for all the other cases
                if(konqBookmark.title() == value.toString())
                    return false;
                konqBookmark.setTitle(value.toString());
                break;
            case Url:
                if(konqBookmark.url() == value.toString())
                    return false;
                konqBookmark.setUrl(QUrl(value.toString()));
                break;
            case UniqueUri:
                if(konqBookmark.uniqueUri() == value.toString())
                    return false;
                break;
            case Tags:
                if(konqBookmark.tags() == value.toString().split(","))
                    return false;
                konqBookmark.setTags(value.toString().split(","));
                break;
            case Description:
                if(konqBookmark.description() == value.toString())
                    return false;
                konqBookmark.setDescription(value.toString());
                break;
            case NumVisits:
                if(konqBookmark.numVisits() == value.toString().toLong())
                    return false;
                konqBookmark.setNumVisits(value.toString().toLong());
                break;
            case Created:
                if(konqBookmark.created() == value.toDateTime())
                    return false;
                konqBookmark.setCreated(value.toDateTime());
                break;
            case LastModified:
                if(konqBookmark.lastModified() == value.toDateTime())
                    return false;
                konqBookmark.setLastModified(value.toDateTime());
                break;
            case LastVisited:
                if(konqBookmark.lastVisited() == value.toDateTime())
                    return false;
                konqBookmark.setLastVisited(value.toDateTime());
                break;
            default:
                break;
            }
            
            emit dataChanged(index, index);
//             EntityTreeModel::setData(indexZero, QVariant::fromValue(item), EntityTreeModel::ItemRole);
            return true;
        } else if ( collection.isValid() )
        {
            kDebug() << "4";
            EntityTreeModel::setData(indexZero, value, Qt::EditRole);
            return true;
        }
    }
    return false;
}

bool KonqBookmarkModel::removeRows( int row, int count, const QModelIndex & parent)
{
    Akonadi::TransactionSequence *transaction = new TransactionSequence;
    for(int i = row; i < row + count; i++)
    {
        const QModelIndex& entityIndex = index(i, 0, parent);
        if(!entityIndex.isValid())
            continue;
        
        Item item = qVariantValue<Item>(data(entityIndex, EntityTreeModel::ItemRole));
        Collection collection = qVariantValue<Collection>(data(entityIndex, EntityTreeModel::CollectionRole));
        if ( item.isValid() && !item.remoteId().isEmpty()) {
            kDebug() << "removing item remoteId = " << item.remoteId();
            new Akonadi::ItemDeleteJob( item, transaction );
        } else  if ( collection.isValid() ) {
            kDebug() << "removing collection (name, remoteId) = " << collection.name() << collection.remoteId();
            new Akonadi::CollectionDeleteJob( collection, transaction );
        }
    }

    if(!transaction->exec())
    {
        kDebug() << transaction->errorString();
        return false;
    }
    return true;
}

QString KonqBookmarkModel::mimeType() const
{
    return QString("application/x-vnd.kde.konqbookmark");
}

QStringList KonqBookmarkModel::mimeTypes() const
{
    QStringList types;
    types << mimeType();
    return types;
}

QMimeData *KonqBookmarkModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (QModelIndex index, indexes) {
        if (index.isValid()) {
            Item item = qVariantValue<Item>(data(index, EntityTreeModel::ItemRole));
            Collection collection = qVariantValue<Collection>(data(index, EntityTreeModel::CollectionRole));
            if(item.isValid() && item.hasPayload<KonqBookmark>())
            {
                stream << EntityTreeModel::ItemRole << item.id();
            } else if (collection.isValid())
            {
                stream << EntityTreeModel::CollectionRole << collection.id();
            }
        }
    }

    mimeData->setData(mimeType(), encodedData);
    return mimeData;
}

bool KonqBookmarkModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                            int row, int column, const QModelIndex &parent)
{   
    kDebug() << "row, column = " << row << ", " << column;
    
    if (!data->hasFormat(mimeType()))
        return false;

    if (action == Qt::IgnoreAction)
        return true;

    if (column > 0)
        return false;

    int beginRow;

    
     if (row != -1)
         beginRow = row;
     else if (parent.isValid())
         beginRow = parent.row();
     else
         beginRow = rowCount(QModelIndex());

    QByteArray encodedData = data->data(mimeType());
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    Akonadi::TransactionSequence *transaction = new TransactionSequence;
    Entity::Id id;
    int role;
    
    while (!stream.atEnd()) {
        // Get the parent collection of the new item
        const QModelIndex& entityIndex = index(beginRow, 0, parent);
        if(!entityIndex.isValid())
            continue;
        
        Item item = qVariantValue<Item>(this->data(entityIndex, EntityTreeModel::ItemRole));
        Collection parentCollection = qVariantValue<Collection>(this->data(entityIndex, EntityTreeModel::CollectionRole));
        if ( item.isValid() ) {
            parentCollection = qVariantValue<Collection>(this->data(parent, EntityTreeModel::CollectionRole));
        }
        
        
        // Dropped a collection or an item?
        stream >> role >> id;
        if(role == EntityTreeModel::ItemRole)
        {
            Item item(id);
            
            //Move or copy?
            if(action == Qt::MoveAction)
            {
                new Akonadi::ItemMoveJob(item, parentCollection, transaction);
            } else if(action == Qt::CopyAction)
            {
                new Akonadi::ItemCopyJob(item, parentCollection, transaction);
            }
        } else if (role == EntityTreeModel::CollectionRole)
        {
            Collection collection(id);
            
            if(!collection.isValid())
                continue;
            
            //Move or copy?
            if(action == Qt::MoveAction)
            {
                collection.setParent(parentCollection);
                new Akonadi::CollectionModifyJob(collection, transaction);
            } else if(action == Qt::CopyAction)
            {
                new Akonadi::CollectionCopyJob(collection, parentCollection, transaction);
            }
        }
    }
    // As this is an async way to drop data, we always return false. Akonadi will
    // update the model later on 
    return false;
}

Qt::DropActions KonqBookmarkModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}
 