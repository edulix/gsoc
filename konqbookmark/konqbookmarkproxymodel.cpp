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

#include "konqbookmarkproxymodel.h"
#include <akonadi_next/entitytreemodel.h>

using namespace Akonadi;

KonqBookmarkProxyModel::KonqBookmarkProxyModel( QObject *parent )
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    setSortCaseSensitivity(Qt::CaseInsensitive);
}

int indexTypeHelper(const QModelIndex & index)
{
    QVariant varCollection = index.data(EntityTreeModel::CollectionRole);
    if(varCollection.value<Akonadi::Collection>().isValid())
        return EntityTreeModel::CollectionRole;
    else
        return EntityTreeModel::ItemRole;
}

bool KonqBookmarkProxyModel::lessThan ( const QModelIndex & left, const QModelIndex & right ) const
{
    bool leftIsCollection = indexTypeHelper(left);
    bool rightIsCollection = indexTypeHelper(right);
    
    if(leftIsCollection && !rightIsCollection)
        return true;
    else if(!leftIsCollection && rightIsCollection)
        return false;
    else
        return QSortFilterProxyModel::lessThan(left, right);
}