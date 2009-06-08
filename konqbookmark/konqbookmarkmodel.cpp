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
#include <akonadi/itemmodifyjob.h>
#include <akonadi/itemfetchscope.h>
#include <Nepomuk/ResourceManager>

#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <KUrl>

#include <QtGui/QPixmap>
#include <QUrl>
#include <QMimeData>

using namespace Akonadi;

class KonqBookmarkModel::Private
{
    public:
};

KonqBookmarkModel::KonqBookmarkModel( QObject *parent )
  : ItemModel( parent ),
    d( new Private() )
{
    fetchScope().fetchFullPayload();
}

KonqBookmarkModel::~KonqBookmarkModel()
{
    delete d;
}

int KonqBookmarkModel::columnCount( const QModelIndex& ) const
{
    return 9;
}

QVariant KonqBookmarkModel::data( const QModelIndex &index,  int role ) const
{
    if ( role == ItemModel::IdRole )
        return ItemModel::data( index, role );

    if ( !index.isValid() || index.row() >= rowCount() )
        return QVariant();

    const Item item = itemForIndex( index );

    if ( !item.hasPayload<KonqBookmark>() )
        return QVariant();

    const KonqBookmark konqBookmark = item.payload<KonqBookmark>();

    // Icon for the model entry
    switch( role )
    {
    case Qt::DecorationRole:
        if ( index.column() == 0 )
        {
            return SmallIcon( QLatin1String( "bookmark" ) );
        }
        break;
        
    case Qt::DisplayRole:
    case Qt::EditRole:
        switch( index.column() )
        {
        case Title:
            return konqBookmark.title();
        case Url:
            return konqBookmark.url().toString();
        case UniqueUri:
            return konqBookmark.uniqueUri();
        case Tags:
            return konqBookmark.tags();
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
        
    default:
        return QVariant();
        break;
    }

    return QVariant();
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
            return i18nc( "@title:column, bookmark url", "Url" );
        case UniqueUri:
            return i18nc( "@title:column, unique uri", "Uri" );
        case Tags:
            return i18nc( "@title:column, tags", "Tags" );
        case Description:
            return i18nc( "@title:column, description", "Description" );
        case NumVisits:
            return i18nc( "@title:column, number of visits", "Number of Visits" );
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

    return ItemModel::headerData( section, orientation, role );
}


Qt::ItemFlags KonqBookmarkModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return ItemModel::flags(index) | Qt::ItemIsEditable;
}
 
bool KonqBookmarkModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole)
    {
        Item item = itemForIndex( index );
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
            konqBookmark.setTags(value.toStringList());
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
 
        ItemModifyJob *modifyJob = new ItemModifyJob( item, this );
        if ( !modifyJob->exec() ) {
            kDebug() << modifyJob->errorString();
            return false;
        }
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

bool KonqBookmarkModel::addBookmark( const KonqBookmark &konqBookmark )
{
    Item item;
    beginInsertRows(QModelIndex(), rowCount()-1, rowCount()-1);
    item.setMimeType( "application/x-vnd.kde.konqbookmark" );
    item.setPayload<KonqBookmark>( konqBookmark );
    ItemCreateJob *job = new ItemCreateJob( item, collection() );
    bool ret = job->exec();
    endInsertRows();
    return ret;
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
 