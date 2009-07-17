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

#ifndef KTREEMODELNAVIGATOR_H
#define KTREEMODELNAVIGATOR_H

#include "konqbookmark_export.h"

#include <QWidget>
#include <QObject>
#include <QModelIndex>
#include <QStringList>

class QAbstractTreeModel;
class QMimeData;
class KUrl;

/**
 * @brief Allows to navigate through the paths of tree model with a bread crumb view.
 *
 * This tree model navigator uses breadcrumb view: The URL is represented by a
 * number of buttons, where each button represents an element of the tree model.
 * By clicking on a button the tree view will change its current index and emit
 * currentChanged signal.This mode also supports drag and drop of items.
 * 
 * The typical usage of the KTreeModelNavigator is:
 * - Create an instance of a tree model sibbling of QAbstractTreeModel.
 * - Create an instance of KTreeModelNavigator and set its model as the one
 *   above.
 * - Create an instance of QAbstractItemView which shows the content of the model 
 *   for the index given by the tree model navigator.
 * - Connect to the signal KTreeModelNavigator::currentChanged() and synchronize 
 *   the content of QAbstractItemView with the index given by the tree model
 *   navigator.

 */
class KONQBOOKMARK_EXPORT KTreeModelNavigator : public QWidget
{
    Q_OBJECT
public:
    KTreeModelNavigator(QWidget* parent = 0);
    virtual ~KTreeModelNavigator();
    
    void setModel(QAbstractItemModel *model);
    
    QAbstractItemModel *model();
    
    QModelIndex currentIndex() const;
    
    /**
     * Changes current index based on the mime data from clipboard. Reimplement
     * if you want it to work for your model.
     */
    virtual void setCurrentIndex(const QMimeData* mimeData);

    /**
     * Helper function used to see if there's a common mimetype in @p mimeTypes
     * with the mimetypes provided by the model.
     */
    bool haveCommonMimetypes(const QMimeData* mimeData);
    
public Q_SLOTS:
    void setCurrentIndex(const QModelIndex& index);
    
Q_SIGNALS:
    void currentChanged(const QModelIndex& index);
    
    /**
     * Is emitted if a dropping has been done above the destination
     * \a destination. The receiver must accept the drop event if
     * the dropped data can be handled.
     */
    void mimeDataDropped(const QModelIndex& destination, QDropEvent* event);
    
protected:
    /**
     * Paste the clipboard content, if the middle mouse
     * button has been clicked.
     * @see QWidget::mouseReleaseEvent()
     */
    virtual void mouseReleaseEvent(QMouseEvent* event);
    
    /**
     * Triggered by KTreeModelNavigatorButton when clicked.
     */
    friend class KTreeModelNavigatorButton;
    void currentChangedTriggered(const QModelIndex& index);

private:
    Q_PRIVATE_SLOT(d, void dropMimeData(const QModelIndex& destination, QDropEvent*))
    
private:
    class Private;
    Private* const d;

    Q_DISABLE_COPY(KTreeModelNavigator)
};


#endif // KTREEMODELNAVIGATOR_H
