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

#include <akonadi/collection.h>
#include <akonadi/item.h>
#include <akonadi/itemfetchjob.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemdeletejob.h>
#include <akonadi/itemmodifyjob.h>
#include <akonadi/collectiondeletejob.h>
#include <akonadi/collectionmodifyjob.h>
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

class KonqBookmarkModel::Private
{
    public:
};

KonqBookmarkModel::KonqBookmarkModel( Akonadi::Session *session, Akonadi::Monitor *monitor, QObject *parent )
  : EntityTreeModel( session, monitor, parent ),
    d( new Private() )
{
    Akonadi::ItemFetchScope scope;
    scope.fetchFullPayload( true );

    monitor->fetchCollection( true );
    monitor->setItemFetchScope( scope );
//     monitor->setMimeTypeMonitored( mimeType() );
//     monitor->setMimeTypeMonitored( Akonadi::Collection::mimeType() );
    // FIXME Set all monitored because otherwise the model doesn't get updates 
    // from items/collections being removed.
    monitor->setAllMonitored(true);
}

KonqBookmarkModel::~KonqBookmarkModel()
{
    delete d;
}

int KonqBookmarkModel::columnCount( const QModelIndex& /*index*/) const
{
    return 9;
}

QVariant KonqBookmarkModel::headerData( int section, Qt::Orientation orientation, int role ) const
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

    return EntityTreeModel::headerData( section, orientation, role );
}


Qt::ItemFlags KonqBookmarkModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

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

        // Icon for the model entry
        switch( role )
        {
        case Qt::DecorationRole:
            if ( column == 0 )
            {
                return SmallIcon( QLatin1String( "bookmarks" ) );
            }
            return QVariant();
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
            return QString("");
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
        QVariant var = indexZero.data(EntityTreeModel::ItemRole);
        QVariant var2 = indexZero.data(EntityTreeModel::CollectionRole);
        Item item = var.value<Item>();
        Collection collection = var2.value<Collection>();
        if ( item.isValid() )
        {  
            if(!item.hasPayload<KonqBookmark>())
                return false;
            
            KonqBookmark konqBookmark = item.payload<KonqBookmark>();
            switch( index.column() )
            {
            case Title:
                konqBookmark.setTitle(value.toString());
                break;
            case Url:
                konqBookmark.setUrl(QUrl(value.toString()));
                break;
            case UniqueUri:
                konqBookmark.setUniqueUri(value.toString());
                break;
            case Tags:
                konqBookmark.setTags(value.toString().split(","));
                break;
            case Description:
                konqBookmark.setDescription(value.toString());
                break;
            case NumVisits:
                konqBookmark.setNumVisits(value.toString().toLong());
                break;
            case Created:
                konqBookmark.setCreated(value.toDateTime());
                break;
            case LastModified:
                konqBookmark.setLastModified(value.toDateTime());
                break;
            case LastVisited:
                konqBookmark.setLastVisited(value.toDateTime());
                break;
            default:
                break;
            }
            item.setPayload<KonqBookmark>( konqBookmark );
            
            EntityTreeModel::setData(indexZero, QVariant::fromValue(item), EntityTreeModel::ItemRole);
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
//     beginRemoveRows(parent, row, row+count-1);
    Akonadi::TransactionSequence *transaction = new TransactionSequence;
    for(int i = row; i < row + count; i++)
    {
        const QModelIndex& entityIndex = index(i, 0, parent);
        if(!entityIndex.isValid())
            continue;
        
        QVariant var = entityIndex.data(EntityTreeModel::ItemRole);
        QVariant var2 = entityIndex.data(EntityTreeModel::CollectionRole);
        Item item = var.value<Item>();
        Collection collection = var2.value<Collection>();
        if ( item.isValid() ) {
            kDebug() << "removing item remoteId = " << item.remoteId();
            new Akonadi::ItemDeleteJob( item, transaction );
        } else {
            kDebug() << "removing collection (name, remoteId) = " << collection.name() << collection.remoteId();
            new Akonadi::CollectionDeleteJob( collection, transaction );
        }
    }

    if(!transaction->exec())
        kDebug() << transaction->errorString();
    
//     endRemoveRows();
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
            //TODO
//             QPixmap pixmap = qVariantValue<QPixmap>(data(index, Qt::UserRole));
//             QPoint location = data(index, Qt::UserRole + 1).toPoint();
//             stream << pixmap << location;
        }
    }

    mimeData->setData(mimeType(), encodedData);
    return mimeData;
}

bool KonqBookmarkModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                            int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(row);
    if (!data->hasFormat(mimeType()))
        return false;

    if (action == Qt::IgnoreAction)
        return true;

    if (column > 0)
        return false;

    int endRow;

    if (!parent.isValid()) {
//         TODO
//         if (row < 0)
//             endRow = pixmaps.size();
//         else
//             endRow = qMin(row, pixmaps.size());
    } else
        endRow = parent.row();

    QByteArray encodedData = data->data(mimeType());
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    while (!stream.atEnd()) {
//         QPixmap pixmap;
//         QPoint location;
//         stream >> pixmap >> location;

        beginInsertRows(QModelIndex(), endRow, endRow);
//         pixmaps.insert(endRow, pixmap);
//         locations.insert(endRow, location);
        endInsertRows();

        ++endRow;
    }

    return true;
}

Qt::DropActions KonqBookmarkModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}
 