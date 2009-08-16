/*
    This file is part of the KDE libraries
    Copyright (c) 2009 Eduardo Robles Elvira <edulix@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KAGGREGATED_MODEL_H
#define KAGGREGATED_MODEL_H

#include "konqbookmark_export.h"
#include <QObject>
#include <QModelIndex>
#include <QAbstractItemModel>

/**
 * This class agregates multiple models into one model. It shows only one column
 * for each index in its display role.
 * 
 * To aggregate a new model, call either addModel(model, displayColumn, displayName),
 * see @p addSourceModel() for more details.
 */
class KONQBOOKMARK_EXPORT KAggregatedModel : public QAbstractItemModel
{
    Q_OBJECT
    
public:
    enum {ColumnCount = 1 };
    
    KAggregatedModel(QObject* parent = 0);
    
    virtual ~KAggregatedModel();
    
    void addSourceModel(QAbstractItemModel *model, QString displayName = QString(), int displayColumn = -1);
    
    void removeSourceModel(QAbstractItemModel *model);
    
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    
    virtual QVariant data(const QModelIndex &index, int role) const;
    
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    
    virtual QModelIndex parent(const QModelIndex& index) const;
    
    virtual int columnCount(const QModelIndex& index) const;
    
    virtual QModelIndex mapFromSource(const QModelIndex& sourceIndex, const QAbstractItemModel *sourceModel = 0) const;
    
    virtual QModelIndex mapToSource(const QModelIndex& index) const;
    
    virtual const QAbstractItemModel* sourceOf(const QModelIndex& index) const;

private:
    class Private;
    friend class Private;
    Private* const d;
    
    Q_PRIVATE_SLOT(d, void dataChanged(const QModelIndex&, const QModelIndex&))
    Q_PRIVATE_SLOT(d, void layoutChanged())
    Q_PRIVATE_SLOT(d, void modelReset())
    Q_PRIVATE_SLOT(d, void rowsInserted(const QModelIndex&, int, int))
    Q_PRIVATE_SLOT(d, void rowsRemoved(const QModelIndex&, int, int))
};

#endif // KAGGREGATED_MODEL_H
