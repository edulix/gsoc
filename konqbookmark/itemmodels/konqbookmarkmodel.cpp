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
#include <akonadi/changerecorder.h>
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

KonqBookmarkModel::KonqBookmarkModel( Akonadi::Session *session, Akonadi::ChangeRecorder *monitor, QObject *parent )
  : EntityTreeModel( session, monitor, parent ),
    d( new Private() )
{
}

KonqBookmarkModel::~KonqBookmarkModel()
{
    delete d;
}

int KonqBookmarkModel::columnCount(const QModelIndex& index) const
{
    Q_UNUSED(index);
    
    return 9;
}

QVariant KonqBookmarkModel::entityHeaderData(int section, Qt::Orientation orientation, int role, HeaderGroup headerGroup ) const
{   
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch(section) {
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
    
    return EntityTreeModel::entityHeaderData( section, orientation, role, headerGroup );
}


Qt::ItemFlags KonqBookmarkModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsDropEnabled;

    return EntityTreeModel::flags(index) | Qt::ItemIsEditable;
}

QVariant KonqBookmarkModel::entityData( const Item &item, int column, int role ) const
{
    if ( item.mimeType() == KonqBookmark::mimeType() ) {
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
    return EntityTreeModel::entityData( item, column, role );
}

QVariant KonqBookmarkModel::entityData( const Collection &collection, int column, int role ) const
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
    return EntityTreeModel::entityData( collection, column, role );
}

bool KonqBookmarkModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // For EntityTree::entityData(... EntityTreeModel::ItemRole) column's index must be 0,
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
    kDebug();
    Akonadi::TransactionSequence *transaction = new TransactionSequence;
    for (int i = row; i < row + count; i++) {
        const QModelIndex& entityIndex = index(i, 0, parent);
        kDebug() << entityIndex << i << parent;
        if (!entityIndex.isValid()) {
            continue;
        }
        
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

    if(!transaction->exec()) {
        kDebug() << transaction->errorString();
        return false;
    }
    return true;
}
 