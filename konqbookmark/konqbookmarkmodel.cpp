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
    return 2;
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
