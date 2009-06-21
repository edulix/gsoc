/*
    Copyright (c) 2009 Stephen Kelly <steveire@gmail.com>
    Copyright (c) 2009 Eduardo Robles Elvira <edulix@gmail.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef MODELWATCHER_H
#define MODELWATCHER_H

#include <QObject>
#include <QAbstractItemModel>
#include <akonadi/item.h>
#include <akonadi/collection.h>

/**
 * @short Watches an item to be inserted in a model.
 */
class ModelWatcher : public QObject
{
    Q_OBJECT
  
public:
    
    /**
     * The constructor
     */
    ModelWatcher(Akonadi::Item::Id itemId, Akonadi::Collection::Id colId,
        QAbstractItemModel *model, QObject *parent = 0);
    
    ~ModelWatcher();

    void setWatch(bool watch);
    
    bool watch() const;

    void setWatchedItem(Akonadi::Item::Id itemId);
    
    Akonadi::Item::Id watchedItem() const;

    void setWatchedCollection( Akonadi::Collection::Id colId );
    
    Akonadi::Collection::Id watchedCollection() const;

    void setWatchedModel(QAbstractItemModel *model);
    
    QAbstractItemModel *watchedModel() const;
    
    QModelIndex itemIndex() const;

signals:
    void newItem(const QModelIndex &idx);

protected slots:
    void rowsInserted(const QModelIndex &parent, int start, int end);

private:
    Akonadi::Item::Id m_itemId; /// Item being inserted
    Akonadi::Collection::Id m_colId; /// Collection in which the item is being inserted
    QAbstractItemModel *m_model; /// Model to watch for item insertion
    bool m_watch; /// are we currently watching?
    QModelIndex m_itemIndex; /// Index of the inserted item
};

#endif // MODELWATCHER_H
