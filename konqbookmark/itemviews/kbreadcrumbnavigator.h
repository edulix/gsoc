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

#ifndef KBREADCRUMBNAVIGATOR_H
#define KBREADCRUMBNAVIGATOR_H

#include "konqbookmark_export.h"

#include <QWidget>
#include <QObject>
#include <QModelIndex>
#include <QStringList>
#include <QItemSelectionModel>

class QAbstractTreeModel;
class QMimeData;
class KUrl;

/**
 * @brief Allows to navigate through the paths of an item model with a bread crumb view.
 *
 * In this navigator the URL is represented by a number of buttons, where each
 * button represents an element of the tree model. By clicking on a button the
 * tree view will change its current index and emit currentChanged signal. This
 * mode also supports drag and drop of items.
 * 
 * The typical usage of the breadcrumb navigator is:
 *
 @code
    QAbstractItemModel *model = new MyModel(this);
    
    QTreeView *view = new QTreeView(this);
    view->setModel(model); 
    
    KBreadCrumbView *breadCrumb = new KBreadCrumbView(this);
    breadCrumb->setModel(model);
    breadCrumb->setSelectionModel(view->selectionModel());
 @endcode
 *
 * In the example above, when the user changes current item in the view, that
 * change is automatically reflected in the breadcrumb. Also, if the user
 * changes the current index in the breadcrumb, that's also reflected in the
 * view.
 */
class KONQBOOKMARK_EXPORT KBreadCrumbNavigator : public QWidget
{
    Q_OBJECT
public:
    KBreadCrumbNavigator(QWidget* parent = 0);
    virtual ~KBreadCrumbNavigator();
    
    void setModel(QAbstractItemModel *model);
    
    /**
     * We use current index from the selection model. We will this selection
     * model as ours, so we'll both react on changes refreshing the widget state
     * and change current selection on the selection model if needed.
     */
    void setSelectionModel(QItemSelectionModel *selectionModel);
    QItemSelectionModel *selectionModel();
    
    QAbstractItemModel *model();
    
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
    
    /**
     * Returns current index in relation to the model.
     */
    QModelIndex currentIndex();
    
Q_SIGNALS:
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
     * Triggered by KBreadCrumbNavigatorButton when clicked.
     */
    friend class KBreadCrumbNavigatorButton;
    void currentChangedTriggered(const QModelIndex& index);

private:
    Q_PRIVATE_SLOT(d, void dropMimeData(const QModelIndex& destination, QDropEvent*))
    Q_PRIVATE_SLOT(d, void slotCurrentIndexChanged())
    
private:
    class Private;
    Private* const d;

    Q_DISABLE_COPY(KBreadCrumbNavigator)
};


#endif // KBREADCRUMBNAVIGATOR_H
