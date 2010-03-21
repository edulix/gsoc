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

#ifndef KONQUEROR_PLACES_PROXY_MODEL_H
#define KONQUEROR_PLACES_PROXY_MODEL_H

#include "konqbookmark_export.h"
#include "place.h"

#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QAbstractProxyModel>
#include <QtCore/QObject>

namespace Konqueror
{
    class LocationBarCompletionModel;
    // TODO: Rename to Konqueror::LocationBarCompletionModel
    /**
     * This is the completion model used in the location bar. It completes
     * history entries, bookmarks, and URLs. It's done by adding an URL
     * completion model to the PlacesManager, and then using the PlacesManager
     * model as the source model and filtering the relevant places.
     *
     * Those places are also filtered by using the query string, which should
     * be set by the LocationBar when the user writes in it.
     *
     * Depending on the query string each item has a Relevance which can be
     * accessed by using the RelevanceRole. The intention is to be able to
     * sort the items in the completion view in descending order. To do that,
     * LocationBar should use a QSortFilterProxy model and call setSortRole()
     * accordingly.
     */
    class KONQBOOKMARK_EXPORT PlacesProxyModel : public QSortFilterProxyModel
    {
        Q_OBJECT
    public:
        friend class LocationBarCompletionModel;
        PlacesProxyModel(QObject *parent = 0);
        virtual ~PlacesProxyModel();

        /**
         * @returns the query string previously set by calling @p setQuery().
         *
         * @note that if it hasn't been set, returns QString().
         *
         * @see setQuery()
         */
        QString query() const;

        /**
         * Adds support for RelevanceRole. For other roles, proxies the call to
         * the sourceModel.
         */
        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

        /**
         * @warning Do not call this method, because the source model can't change (it
         * always be the PlacesManager model). This method is here only because
         * QSortFilterProxyModel needs it.
         */
        virtual void setSourceModel(QAbstractItemModel *sourceModel);

    public Q_SLOTS:
        /**
         * Updates the query string, and filters again the model by using the
         * new query, updating also the relevance of each index.
         */
        void setQuery(QString query);

    protected:
        /**
         * Will only accept items using two contraints:
         *  - Only accept places from bookmarks, history, or the given url
         *    completion model.
         *  - Only accept items which are relevant to the given query string.
         */
        virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;

    private:
        class Private;
        Private *const d;
        Q_DISABLE_COPY(PlacesProxyModel)

        Q_PRIVATE_SLOT(d, void slotRowsInserted(const QModelIndex &, int, int))
        Q_PRIVATE_SLOT(d, void slotRowsRemoved(const QModelIndex &, int, int))
        Q_PRIVATE_SLOT(d, void slotModelReset())
    };

    /**
     * Model that can be directly used by a LocationBar as its completion model.
     * It's like PlacesProxyModel but sorted by descending relevance.
     * Note that this class is a HACK and should be removed whenever it can be
     * replaced by QSortFilterProxyModel, but at the moment QSortFilterProxyModel
     * doesn't seem to be working.
     */
    class LocationBarCompletionModel : public QAbstractProxyModel
    {
        Q_OBJECT
    public:
        LocationBarCompletionModel(PlacesProxyModel *sourceModel, QObject *parent);

        virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
        virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
        virtual QModelIndex index(int row, int dcolumn, const QModelIndex &parent = QModelIndex()) const;
        virtual QModelIndex parent(const QModelIndex &) const;
        virtual int rowCount(const QModelIndex &) const;
        virtual int columnCount(const QModelIndex &) const;
        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

        // Do not use
        virtual void setSourceModel(QAbstractItemModel */*sourceModel*/) {}
    private:
        class Private;
        Private *const d;

        Q_PRIVATE_SLOT(d, void slotModelReset());
    };
}


#endif // KONQUEROR_PLACES_PROXY_MODEL_H
