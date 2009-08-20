/*
    This file is part of the KDE project
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

#include "collectionsproxymodel.h"
#include <akonadi/entitytreemodel.h>

using namespace Konqueror;
using namespace Akonadi;

CollectionsProxyModel::CollectionsProxyModel( QObject *parent )
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    setSortCaseSensitivity(Qt::CaseInsensitive);
}

static int indexTypeHelper(const QModelIndex & index)
{
    QString mimeType = index.data(EntityTreeModel::MimeTypeRole).toString();
    if(mimeType == Collection::mimeType()) {
        return EntityTreeModel::CollectionRole;
    } else {
        return EntityTreeModel::ItemRole;
    }
}

bool CollectionsProxyModel::filterAcceptsRow( int sourceRow, const QModelIndex &sourceParent ) const
{
    QModelIndex sourceIndex = sourceModel()->index(sourceRow, 0, sourceParent);    
    return indexTypeHelper(sourceIndex) == EntityTreeModel::CollectionRole;
}
