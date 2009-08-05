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

#ifndef MODELMENU_H
#define MODELMENU_H

#include "konqbookmark_export.h"
#include "modelmenusearchline.h"

#include <QMenu>
#include <QFlags>
#include <QModelIndex>

#include <kmenu.h>

class QSortFilterProxyModel;
class ModelMenuSearchLine;
class QAbstractItemModel;

/**
 * @short A QMenu that is dynamically populated and updated from a QAbstractItemModel
 *
 * This class is similar to an ItemView (uses a QAbstractItemModel as
 * a data source) but inheriting instead from QMenu. Each item in the menu
 * represents an index in the model.
 * 
 * It supports searching with a search line. Search can be activated with the
 * property showSearchLine.
 *
 * @author Eduardo Robles Elvira <edulix@gmail.com>
 */
class KONQBOOKMARK_EXPORT ModelMenu : public KMenu
{
    Q_OBJECT
    Q_PROPERTY(bool showSearchLine READ showSearchLine WRITE setShowSearchLine)
public:
    enum Flag
    {
        NoOptionsFlag = 0x0,
        OneColumnFlag = 0x1, // TODO
        IsRootFlag = 0x2,
        CustomFlag = 0xF
    };
    Q_DECLARE_FLAGS(Flags, Flag)
    
    enum MenuRole
    {
        StatusBarTextRole = 0x1,
        SeparatorRole = 0x2
    };
    enum { MenuRolesSize = 2 };
    
    enum MenuItemLocation
    {
        PreModelItems = 0x1,
        PostModelItems = 0x2
    };
    
    ModelMenu(QWidget *parent = 0);
    ModelMenu(ModelMenu *parentMenu = 0);
    virtual ~ModelMenu();

    void setModel(QAbstractItemModel *model);
    QAbstractItemModel *model() const;

    void setRootIndex(const QModelIndex &index);
    QModelIndex rootIndex() const;

    void setRole(MenuRole menuRole, int modelRole);
    int role(MenuRole menuRole) const;

    QModelIndex index(QAction *action);
    QModelIndex indexAt(const QPoint& pt);
    QAction *actionForIndex(const QModelIndex& index);
    
    virtual KMenu *contextMenu(const QModelIndex& index) { Q_UNUSED(index); return 0; }

    void setFlags(Flags flags);
    Flags flags() const;
    
    QSortFilterProxyModel* searchModel();
        
    bool setShowSearchLine(bool showSearchLine);
    
    bool showSearchLine() const;
    
    ModelMenuSearchLine *searchLine();
    
    void removeAction(QAction* action)
    {
        QMenu::removeAction(action);
    }
    
protected:
    virtual QAction *makeAction(const QIcon &icon, const QString &text, QObject *parent);
    
    /**
     * With this helper function inherited classes can add actions that will
     * appear in the menu either before or after the real model menu actions.
     * This is function is normally called in the constructor.
     *
     * @note It will trigger a modelReset() internal call (all currentnly added
     * model actions will be removed and einserted correctly) if it's the first
     * preAction added, so it's best if you call to this function before the
     * menu is shown.
     */
    void addAction(QAction *action, MenuItemLocation location);
    
    
    /**
     * Call this if you want to remove any action added with @ref addAction(action, location).
     * It will trigger take care of removing any separator if needed.
     */
    void removeAction(QAction* action, MenuItemLocation location);
    
    /**
     * Returns whether an index corresponds to a folder or not. This is needed
     * to now if we should create a QAction or a QMenu when inserting an index.
     * 
     * The default implementation only checks if the index has any children.
     */
    virtual bool isFolder(const QModelIndex& index) const;
    
    /**
     * @return the QMenu that is used to populate sub-menu's
     */
    virtual ModelMenu *createBaseMenu();

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    
    /**
     * Search mode can be activated only if we are root.
     * 
     * @returns true if search mode was activated
     */
    friend class ModelMenuSearchLine;
    bool setSearchActive(bool searchActive);
    bool searchActive() const;
    
    /**
     * Current model changes. When searching is activated, currentModel() returns
     * m_searchModel, otherwise it returns m_model.
     */
    QAbstractItemModel* currentModel() const;
    
    /**
     * When search is activated currentModel() is m_search which contains all
     * the indexes in a plain list without any hierarchy other than all of them
     * have QModelIndex() as parent so that's what currentRootIndex() returns if
     * search is active. OTherwise returns m_root.
     */
    QModelIndex currentRootIndex();

    
Q_SIGNALS:
    /**
     * Emitted when one of this menu's actions have been activated
     */
    void activated(const QModelIndex &index);
    
    /**
     * Emitted when we change from one root index to another.
     * Note: This is NOT emitted when the data in the model changes but the
     * QModelIndex remains being the same.
     */
    void rootChanged(const QModelIndex &rootIndex);
    
    /**
     * Emitted when the number of model items in the menu changes. Useful if
     * in your inheritted class you want to activate/deactivate some actions
     * depending of the number of rows; for example, an "Open in
     * Tabs" action does not make sense if the root item for tihs menu has no
     * child.
     */
    void rowCountChanged(const int& count);

private:
    class Private;
    Private* const d;
    
    Q_PRIVATE_SLOT(d, void slotAboutToShow())
    Q_PRIVATE_SLOT(d, void slotAboutToHide())
    Q_PRIVATE_SLOT(d, void actionTriggered(QAction *action))
    Q_PRIVATE_SLOT(d, void actionDeleted(QObject* actionObj))
    Q_PRIVATE_SLOT(d, void dataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight ))
    Q_PRIVATE_SLOT(d, void rowsInserted ( const QModelIndex & parent, int start, int end ))
    Q_PRIVATE_SLOT(d, void rowsAboutTobeRemoved ( const QModelIndex & parent, int start, int end ))
    Q_PRIVATE_SLOT(d, void modelReset (bool aboutTobeShown = false))
};

#endif // MODELMENU_H