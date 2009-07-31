 /*
 * Copyright 2008 Benjamin C. Meyer <ben@meyerhome.net>
 * Copyright (c) 2009 Eduardo Robles Elvira <edulix@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

/****************************************************************************
**
** Copyright (C) 2008-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "modelmenu.h"

#include <QApplication>
#include <QEvent>
#include <QTimer>
#include <QHash>
#include <QMouseEvent>
#include <QSortFilterProxyModel>

#include <kdebug.h>
#include <kdescendantsproxymodel.h>

class ModelMenu::Private
{
public:
    Private(ModelMenu *parent);
    Private(ModelMenu *parent, Private *copy);
    ~Private();

    void slotAboutToShow();
    void actionTriggered(QAction *action);
    QAction *makeAction(const QModelIndex &index);
    
    QAbstractItemModel* currentModel() const;
    QModelIndex currentRootIndex();
    QModelIndex indexToSource(const QModelIndex& index);
    QModelIndex indexToCurrent(const QModelIndex& index);
    bool descendantFromRoot(const QModelIndex &index);
    
    /**
     * Deals with some managements (i.e. updating m_actionForIndex) needed
     * when actions are added;
     */
    void actionAdded(QAction *action);
    
    /**
     * When an action is deleted, we need to know it and remove it from
     * m_actionForIndex.
     */
    void actionDeleted(QObject *actionObj);

    /**
     * Puts all children of parent into menu up to max
     */
    void populateMenu();
    
    /**
     * Inserts an index in the menu. Does the work of dealing with if the index
     * should be a submenu or a simple QAction, etc.
     * @arg index       Index to be inserted
     * @arg before      The index will be inserted before it
     */
    void insertIndex(const QModelIndex &index, QAction *before = 0);
    
    QAction* createSubmenu(const QModelIndex &parent, QAction *before = 0);
    
    /**
     * The following slots are called when the model changes and we need to
     * adopt those changes.
     */
    void dataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight );
    void rowsInserted ( const QModelIndex & parent, int start, int end );
    void rowsAboutTobeRemoved ( const QModelIndex & parent, int start, int end );
    void modelReset (bool force = false);
    
    /// Max number of rows before we go to two columns
    int m_maxRows;
    int m_maxWidth;
    
    /// The model given by the user
    QAbstractItemModel *m_model;
    /// The descendends model for the one given by the user
    KDescendantsProxyModel *m_proxyModel;
    /// The filtered for the model above
    QSortFilterProxyModel *m_searchModel;
    
    QPersistentModelIndex m_root;
    QPoint m_dragStartPos;
    int m_menuRole[MenuRolesSize];
    int m_flags;
    ModelMenu *m_parentMenu;
    /// Dirty is true if there are pending changes (as changes are only applied
    /// dynamically when menu is visible).
    bool m_dirty;
    bool m_searchActive;
    ModelMenu *q;
    
    /// Here we store which action correspond to which index
    QHash<QModelIndex, QAction*> m_actionForIndex;
};

ModelMenu::Private::Private(ModelMenu *parentMenu)
    : m_maxRows(-1),
    m_maxWidth(-1),
    m_model(0),
    m_proxyModel(0),
    m_searchModel(0),
    m_flags(IsRootFlag),
    m_parentMenu(0),
    m_dirty(false),
    m_searchActive(false),
    q(parentMenu)
{
    m_menuRole[StatusBarTextRole] = m_menuRole[SeparatorRole] = 0;
}

ModelMenu::Private::Private(ModelMenu *parentMenu, Private* copy)
    : m_maxRows(-1),
    m_maxWidth(-1),
    m_model(copy->m_model),
    m_proxyModel(copy->m_proxyModel),
    m_searchModel(copy->m_searchModel),
    m_flags(IsRootFlag),
    m_parentMenu(0),
    m_dirty(false),
    m_searchActive(false),
    q(parentMenu)
{
    m_menuRole[StatusBarTextRole] = m_menuRole[SeparatorRole] = 0;
}

ModelMenu::Private::~Private()
{
    
}


QAbstractItemModel* ModelMenu::Private::currentModel() const
{
    if(q->searchActive())
        return m_searchModel;
    else
        return m_model;
}

QModelIndex ModelMenu::Private::currentRootIndex()
{
    if(!q->searchActive())
        return m_root;
    else
        return QModelIndex();
}

QModelIndex ModelMenu::Private::indexToSource(const QModelIndex& index)
{
    if(q->searchActive())
        return m_proxyModel->mapToSource(m_searchModel->mapToSource(index));
    else
        return index;
}

QModelIndex ModelMenu::Private::indexToCurrent(const QModelIndex& index)
{
    if(q->searchActive())
        return m_searchModel->mapFromSource(m_proxyModel->mapFromSource(index));
    else
        return index;
}

ModelMenu::ModelMenu(QWidget *parent)
    : KMenu(parent), d(new Private(this))
{
    d->m_flags = IsRootFlag;
    connect(this, SIGNAL(aboutToShow()), this, SLOT(slotAboutToShow()));
    connect(this, SIGNAL(triggered(QAction*)), this, SLOT(actionTriggered(QAction*)));
}

ModelMenu::ModelMenu(ModelMenu *parentMenu)
    : KMenu(parentMenu), d(new Private(this, parentMenu->d))
{
    d->m_parentMenu = parentMenu;
    connect(this, SIGNAL(aboutToShow()), this, SLOT(slotAboutToShow()));
    connect(this, SIGNAL(triggered(QAction*)), this, SLOT(actionTriggered(QAction*)));
}

ModelMenu::~ModelMenu()
{
    delete d;
}

bool ModelMenu::prePopulated()
{
    return false;
}

void ModelMenu::postPopulated()
{
}

void ModelMenu::setModel(QAbstractItemModel *newModel)
{
    if(d->currentModel() == d->m_model && d->m_model)
    {
        disconnect(d->m_model, SIGNAL(dataChanged ( const QModelIndex &, const QModelIndex & )),
            this, SLOT(dataChanged ( const QModelIndex &, const QModelIndex & )));
        disconnect(d->m_model, SIGNAL(rowsInserted ( const QModelIndex & , int , int )),
            this, SLOT(rowsInserted ( const QModelIndex & , int , int )));
        disconnect(d->m_model, SIGNAL(rowsAboutToBeRemoved ( const QModelIndex & , int , int )),
            this, SLOT(rowsAboutTobeRemoved ( const QModelIndex & , int , int )));
        disconnect(d->m_model, SIGNAL(modelReset ()),
            this, SLOT(modelReset ()));
        disconnect(d->m_model, SIGNAL(layoutChanged()),
            this, SLOT(modelReset()));
    }
    
    d->m_model = newModel;
    if(flags() & IsRootFlag)
    {
        delete d->m_proxyModel;
        delete d->m_searchModel;
        
        d->m_proxyModel = new KDescendantsProxyModel(this);
        d->m_proxyModel->setSourceModel(newModel);
        
        d->m_searchModel = new QSortFilterProxyModel(this);
        d->m_searchModel->setSourceModel(d->m_proxyModel);
    }
    
    d->m_dirty = true;
    d->modelReset();
    
    connect(d->currentModel(), SIGNAL(dataChanged ( const QModelIndex &, const QModelIndex & )),
        this, SLOT(dataChanged ( const QModelIndex &, const QModelIndex & )));
    connect(d->currentModel(), SIGNAL(rowsInserted ( const QModelIndex & , int , int )),
        this, SLOT(rowsInserted ( const QModelIndex & , int , int )));
    connect(d->currentModel(), SIGNAL(rowsAboutToBeRemoved ( const QModelIndex & , int , int )),
        this, SLOT(rowsAboutTobeRemoved ( const QModelIndex & , int , int )));
    connect(d->currentModel(), SIGNAL(modelReset ()),
        this, SLOT(modelReset ()));
    connect(d->currentModel(), SIGNAL(layoutChanged()),
        this, SLOT(modelReset()));
}

QAbstractItemModel *ModelMenu::model() const
{
    return d->m_model;
}

void ModelMenu::setRootIndex(const QModelIndex &index)
{
    d->m_root = index;
}

QModelIndex ModelMenu::rootIndex() const
{
    return d->m_root;
}

bool ModelMenu::setSearchActive(bool newSearchActive)
{
        kDebug() << "1 newSearchActive = " << newSearchActive;
    if(flags() & IsRootFlag && searchActive() != newSearchActive)
    {
        kDebug() << "1.1" << (d->currentModel() == d->m_searchModel);
        disconnect(d->currentModel(), SIGNAL(dataChanged ( const QModelIndex &, const QModelIndex & )),
            this, SLOT(dataChanged ( const QModelIndex &, const QModelIndex & )));
        disconnect(d->currentModel(), SIGNAL(rowsInserted ( const QModelIndex & , int , int )),
            this, SLOT(rowsInserted ( const QModelIndex & , int , int )));
        disconnect(d->currentModel(), SIGNAL(rowsAboutToBeRemoved ( const QModelIndex & , int , int )),
            this, SLOT(rowsAboutTobeRemoved ( const QModelIndex & , int , int )));
        disconnect(d->currentModel(), SIGNAL(modelReset ()),
            this, SLOT(modelReset ()));
        disconnect(d->currentModel(), SIGNAL(layoutChanged()),
            this, SLOT(modelReset()));
            
        d->m_searchActive = newSearchActive;
        kDebug() << "1.2"  << (d->currentModel() == d->m_model);
        d->modelReset();
        kDebug() << "1.3";
        
        connect(d->currentModel(), SIGNAL(dataChanged ( const QModelIndex &, const QModelIndex & )),
            this, SLOT(dataChanged ( const QModelIndex &, const QModelIndex & )));
        connect(d->currentModel(), SIGNAL(rowsInserted ( const QModelIndex & , int , int )),
            this, SLOT(rowsInserted ( const QModelIndex & , int , int )));
        connect(d->currentModel(), SIGNAL(rowsAboutToBeRemoved ( const QModelIndex & , int , int )),
            this, SLOT(rowsAboutTobeRemoved ( const QModelIndex & , int , int )));
        connect(d->currentModel(), SIGNAL(modelReset ()),
            this, SLOT(modelReset ()));
        connect(d->currentModel(), SIGNAL(layoutChanged()),
            this, SLOT(modelReset()));
        kDebug() << "1.4";
    }
    
    return d->m_searchActive;
}

bool ModelMenu::searchActive() const
{
    return d->m_searchActive;
}

void ModelMenu::setRole(MenuRole menuRole, int modelRole)
{
    d->m_menuRole[menuRole] = modelRole;
}


QSortFilterProxyModel* ModelMenu::searchModel()
{
    return d->m_searchModel;
}

int ModelMenu::role(MenuRole menuRole) const
{
    return d->m_menuRole[menuRole];
}

void ModelMenu::setFlags(ModelMenu::Flags flags)
{
    d->m_flags = flags;
}

ModelMenu::Flags ModelMenu::flags() const
{
    return ModelMenu::Flags(d->m_flags);
}

Q_DECLARE_METATYPE(QModelIndex)
void ModelMenu::Private::slotAboutToShow()
{
    kDebug();
    if(m_dirty)
        modelReset(true);
}

ModelMenu *ModelMenu::createBaseMenu()
{
    return new ModelMenu(this);
}

QAction* ModelMenu::Private::createSubmenu(const QModelIndex &parent, QAction *before)
{
    QString title = parent.data().toString();
    ModelMenu *modelMenu = q->createBaseMenu();
    // triggered goes all the way up the menu structure
    disconnect(modelMenu, SIGNAL(triggered(QAction*)),
                modelMenu, SLOT(actionTriggered(QAction*)));
    modelMenu->setTitle(title);
    QIcon icon(parent.data(Qt::DecorationRole).value<QPixmap>());
    modelMenu->setIcon(icon);
    q->insertMenu(before, modelMenu);
    actionAdded(modelMenu->menuAction());
    modelMenu->setRootIndex(indexToSource(parent));
    modelMenu->setModel(m_model);
    return modelMenu->menuAction();
}

bool ModelMenu::isFolder(const QModelIndex& index) const
{
    Q_ASSERT(model());
    
    return model()->hasChildren(index);
}

bool ModelMenu::Private::descendantFromRoot(const QModelIndex &index)
{
    QModelIndex ancestor = index.parent();
    for(; ancestor.isValid() || ancestor == m_root; ancestor = ancestor.parent())
        if(ancestor == m_root)
            return true;
        
    return false;
}

void ModelMenu::Private::insertIndex(const QModelIndex &index, QAction *before)
{
    Q_ASSERT(currentModel());
    
    QAction* lastAddedAction = 0;
    QModelIndex sourceIndex = indexToSource(index);
    
    if(!descendantFromRoot(sourceIndex))
        return;
    
    if(m_actionForIndex.contains(sourceIndex))
    {
        q->insertAction(before, q->actionForIndex(sourceIndex));
        return;
    }
    
    else if (q->isFolder(sourceIndex)) {
        lastAddedAction = createSubmenu(index, before);
    } else {
        if (m_menuRole[SeparatorRole] != 0
            && index.data(m_menuRole[SeparatorRole]).toBool())
            lastAddedAction = q->insertSeparator(before);
        else {
            q->insertAction(before, lastAddedAction = makeAction(index));
        }
        actionAdded(lastAddedAction);
    }
    
    int size = q->actions().size();
    if(q->columnCount() > 1 && m_maxRows > size)
    {
        m_maxRows = size;
    }
}

void ModelMenu::Private::populateMenu()
{
    if (!currentModel())
        return;
    
    int end = currentModel()->rowCount(currentRootIndex());
    kDebug() << end;
    if (m_maxRows != -1)
        end = qMin(m_maxRows, end);
    
    for (int i = 0; i < end; ++i) {
        QModelIndex index = currentModel()->index(i, 0, currentRootIndex());
        kDebug() << index.data();
        insertIndex(index);
    }
}

void ModelMenu::Private::actionAdded(QAction *action)
{
    m_actionForIndex[q->index(action)] = action;
    connect(action, SIGNAL(destroyed(QObject *)), q, SLOT(actionDeleted(QObject *)));
}


void ModelMenu::Private::actionDeleted(QObject* actionObj)
{
    QAction *action = qobject_cast<QAction*>(actionObj);
    QModelIndex index = q->index(action);
    if(index.isValid() && m_actionForIndex.contains(index))
        m_actionForIndex.remove(index);
}

QAction *ModelMenu::Private::makeAction(const QModelIndex &index)
{
    QIcon icon(index.data(Qt::DecorationRole).value<QPixmap>());
    
    QFontMetrics fm(q->font());
    if (m_maxWidth == -1)
        m_maxWidth = fm.width(QLatin1Char('m')) * 30;
    QString smallText = fm.elidedText(index.data().toString(), Qt::ElideMiddle, m_maxWidth);
    
    QAction *action = q->makeAction(icon, smallText, q);
    
    if(m_menuRole[StatusBarTextRole] != 0)
        action->setStatusTip(index.data(m_menuRole[StatusBarTextRole]).toString());

    QVariant v;
    v.setValue(indexToSource(index));
    action->setData(v);
    return action;
}

QAction *ModelMenu::makeAction(const QIcon &icon, const QString &text, QObject *parent)
{
    return new QAction(icon, text, parent);
}

void ModelMenu::Private::actionTriggered(QAction *action)
{
    QModelIndex idx = q->index(action);
    if (idx.isValid())
        emit q->activated(idx);
}

QModelIndex ModelMenu::index(QAction *action)
{
    if (!action)
        return QModelIndex();
    QVariant variant = action->data();
    if (!variant.canConvert<QModelIndex>())
        return QModelIndex();

    return qvariant_cast<QModelIndex>(variant);
}

QModelIndex ModelMenu::indexAt(const QPoint& pt)
{
    return index(actionAt(pt));
}

QAction* ModelMenu::actionForIndex(const QModelIndex& index)
{
    if(!d->m_actionForIndex.contains(index))
        return 0;
    
    return d->m_actionForIndex.value(index);
}

void ModelMenu::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        d->m_dragStartPos = event->pos();
    QMenu::mousePressEvent(event);
}

void ModelMenu::mouseReleaseEvent(QMouseEvent *event)
{
    QMenu::mouseReleaseEvent(event);
}

void ModelMenu::mouseMoveEvent(QMouseEvent *event)
{
    if(!d->currentModel())
        return;
    
    if ((event->pos() - d->m_dragStartPos).manhattanLength() > QApplication::startDragDistance())
    {
        QAction *action = actionAt(d->m_dragStartPos);
        QModelIndex eventIndex = index(action);
        
        if (event->buttons() == Qt::LeftButton
            && eventIndex.isValid()
            && !isFolder(eventIndex))
        {
            QDrag *drag = new QDrag(this);
            drag->setMimeData(d->currentModel()->mimeData((QModelIndexList() << eventIndex)));
            QRect actionRect = actionGeometry(action);
            drag->setPixmap(QPixmap::grabWidget(this, actionRect));
            drag->exec();
        }
    }
    QMenu::mouseMoveEvent(event);
}

void ModelMenu::Private::dataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight )
{    
    if(!q->isVisible())
    {
        m_dirty = true;
        return;
    }

    const QModelIndex& top = topLeft.sibling(topLeft.row(), 0);
    
    // Here we assume top.parent() == bottom.parent()
    if(top.parent() != currentRootIndex())
        return;
    
    for(int i = top.row(); i <= bottomRight.row(); i++)
    {
        const QModelIndex& index = top.sibling(i, 0);
        QAction *action = q->actionForIndex(indexToSource(index));
        if(!action)
            continue;
        
        QIcon icon(index.data(Qt::DecorationRole).value<QPixmap>());
        action->setIcon(icon);
        
        QFontMetrics fm(q->font());
        if (m_maxWidth == -1)
            m_maxWidth = fm.width(QLatin1Char('m')) * 30;
        QString smallText = fm.elidedText(index.data().toString(), Qt::ElideMiddle, m_maxWidth);
        action->setText(smallText);
        
        if(m_menuRole[StatusBarTextRole] != 0)
            action->setStatusTip(index.data(m_menuRole[StatusBarTextRole]).toString());
        
    }
    
}

void ModelMenu::Private::rowsInserted ( const QModelIndex & parent, int start, int end )
{   
    if(!q->isVisible())
    {
        m_dirty = true;
        return;
    }
    
    if(parent.parent() == currentRootIndex())
    {
        // We might have to update parent (if it's in our list) to change from
        // being a simple QAction to now get a QMenu
        QAction *action = q->actionForIndex(indexToSource(parent));
        if(action && !action->menu())
        {
            QString title = parent.data().toString();
            ModelMenu *modelMenu = q->createBaseMenu();
            // triggered goes all the way up the menu structure
            disconnect(modelMenu, SIGNAL(triggered(QAction*)),
                        modelMenu, SLOT(actionTriggered(QAction*)));
            modelMenu->setTitle(title);
            QIcon icon(parent.data(Qt::DecorationRole).value<QPixmap>());
            modelMenu->setIcon(icon);
            action->setMenu(modelMenu);
            modelMenu->setRootIndex(indexToSource(parent));
            modelMenu->setModel(m_model);
        }
        return;
    } else if(parent != currentRootIndex())
        return;
    
    // We'll insert the new actions before this action. If no action is found
    // the actions will be appended
    const QModelIndex& index = parent.child(start - 1, 0);
    QAction* before = q->actionForIndex(indexToSource(index));
    for(int i = start; i <= end; i++)
    {
        const QModelIndex& index = parent.child(i, 0);
        insertIndex(index, before);
        
    }
}

void ModelMenu::Private::rowsAboutTobeRemoved ( const QModelIndex & parent, int start, int end )
{
    if(parent != currentRootIndex())
        return;
    
    if(!q->isVisible())
    {
        m_dirty = true;
        return;
    }
    
    for(int i = start; i <= end; i++)
    {
        // Delete the related actions and let actionDeleted() slot deal with it
        const QModelIndex& index = parent.child(i, 0);
        QAction *action = q->actionForIndex(indexToSource(index));
        if(action)
        {
            q->removeAction(action);
            //TODO: crash (now leaks)
//             action->deleteLater();
        }
    }
}

void ModelMenu::Private::modelReset (bool force)
{
    if(!force && !q->isVisible())
    {
        m_dirty = true;
        return;
    }
    
    QListIterator<QAction*> it(m_actionForIndex.values());
    QList<QAction*> actionsToDelete(m_actionForIndex.values());
    while (it.hasNext())
    {
        QAction* action = it.next();
        if(!action)
            continue;
        q->removeAction(action);
            //TODO: crash (now leaks and itemModel)
//         action->deleteLater();
    }

    if (q->prePopulated())
        q->addSeparator();
    populateMenu();
    q->postPopulated();
    
    m_dirty = false;
        
}

#include "modelmenu.moc"
