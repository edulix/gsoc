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

#ifndef PLACES_PROXY_MODEL_H
#define PLACES_PROXY_MODEL_H

#include "konqbookmark_export.h"
#include "placequerymatch.h"

#include <QSortFilterProxyModel>
#include <QObject>

namespace Konqueror
{
    /**
     * Given a model that supports the Place::PlaceUrlRole this model proxies it
     * converting it in a descendant model.
     * 
     * Each index will have a PlaceQueryMatch which corresponds to the url for
     * that index extracted from the source model with the PlaceUrl role. The
     * relevance of the place will be recalculated when you call to @p setQuery.
     */
    class KONQBOOKMARK_EXPORT PlacesProxyModel : public QSortFilterProxyModel
    {        
        Q_OBJECT
    public:
        enum {
            RelevanceRole = Qt::UserRole + 60
        };
        
        PlacesProxyModel(QObject *parent = 0);
        virtual ~PlacesProxyModel();
        
        virtual void setQuery(QVariant query);
        virtual QVariant query() const;
        
        /** 
         * Reimplemented because we won't use directly the given source model,
         * but we will use a KDescendantsProxyModel instead as a source.
         */
        virtual void setSourceModel(QAbstractItemModel* sourceModel);
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
        
    protected:
        virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;
        
    private:
        class Private;
        Private* const d;
        
        Q_PRIVATE_SLOT(d, void slotRowsInserted(const QModelIndex&, int, int))
        Q_PRIVATE_SLOT(d, void slotRowsRemoved(const QModelIndex&, int, int))
        Q_PRIVATE_SLOT(d, void slotModelAboutToBeReset())
    };
}

#endif // PLACES_PROXY_MODEL_H
