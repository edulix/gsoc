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

#ifndef KONQUEROR_PROXY_MODEL_H
#define KONQUEROR_PROXY_MODEL_H

#include "konqbookmark_export.h"
#include <QSortFilterProxyModel>

namespace Konqueror
{
    /** 
     * class created because because of some reason EntityFilterProxyModel doesn't
     * seem to be working right now.
     */
    class KONQBOOKMARK_EXPORT CollectionsProxyModel : public QSortFilterProxyModel
    {        
    public:
        CollectionsProxyModel( QObject *parent = 0 );
    protected:
        
        /**
        * Reimplemented to show only folders
        */
        virtual bool filterAcceptsRow( int sourceRow, const QModelIndex &sourceParent ) const;
    };

}

#endif /* KONQUEROR_PROXY_MODEL_H */
