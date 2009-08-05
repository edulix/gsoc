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

#include "modelmenu.h"

#include <QApplication>
#include <QEvent>
#include <QTimer>
#include <QStyle>
#include <QDesktopWidget>
#include <QHash>
#include <QList>
#include <QWidgetAction>
#include <QMouseEvent>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

#include <kdebug.h>
#include <kdescendantsproxymodel.h>

/**
 * Private class that helps to provide binary compatibility between releases.
 * @internal
 */
//@cond PRIVATE
class ModelMenu::Private
{
public:
    Private(ModelMenu *parent);
    Private(ModelMenu *parent, Private *copy);
    ~Private();
    
    /**
     * Shared initialization method by both constructors
     */
    void init();

    /**
     * Repopulates if m_dirty is true
     */
    void slotAboutToShow();
    
    /**
     * Search line loses focus
     */
    void slotAboutToHide();
    
    /**
     * Called when an action is activated by the user. It will in turn emit
     * activated if it's index is valid.
     */
    void actionTriggered(QAction *action);
    
    /**
     * Convenience function to create an action out of an index. Extracts the
     * icon, text and tooltipText for the action and then calls to
     * ModelMenu::makeAction().
     * @arg index   Index from currentModel()
     */
    QAction *makeAction(const QModelIndex &index);
    
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
    
    /**
     * Convenience function to map an index from current model (which can either
     * be m_model or m_searchModel) to m_model. 
     */
    QModelIndex indexToSource(const QModelIndex& index);
    
    /**
     * Convenience function which does the reverse as the one above.
     */
    QModelIndex indexToCurrent(const QModelIndex& index);
    
    /**
     * HACK This functions checks if an index from q->model() (i.e. "source
     * model") is descendant from m_root. This is needed when calling to
     * Private::insertIndex() in order to filter index which are not descedants
     * from m_root.
     * 
     * This should be done by setting m_proxyModel->rootIndex() to
     * m_proxyModel->mapFromSource(m_root) but doesn't seem to work.
     */
    bool descendantFromRoot(const QModelIndex &index);
    
    /**
     * Deals with some managements (i.e. updating m_actionForIndex) needed
     * when actions are added.
     */
    void actionAdded(QAction *action);
    
    /**
     * When an action is deleted, we need to know it and remove it from
     * m_actionForIndex.
     */
    void actionDeleted(QObject *actionObj);

    /**
     * Puts all children of parent into menu. Called by modelReset() only.
     */
    void populateMenu();
    
    /**
     * Inserts an index in the menu. Does the work of dealing with if the index
     * should be a submenu or a simple QAction, etc.
     * @arg index           Index from currentModel() to be inserted
     * @arg before          The index will be inserted before it
     * @arg repopulating    Should be true if all actions are bein reinserted
     */
    void insertIndex(const QModelIndex &index, QAction *before = 0);
    
    /**
     * Creates a submenu for the given index and adds it before the "before"
     * action var in our menu. Called by insertIndex().
     * @arg index       Index from currentModel() to be inserted
     * @arg before      The index will be inserted before it
     */
    QAction* createSubmenu(const QModelIndex &parent, QAction *before = 0);
    
    /**
     * The following slots are called when the model changes and we need to
     * adopt those changes.
     */
    void dataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight );
    void rowsInserted ( const QModelIndex & parent, int start, int end );
    void rowsAboutTobeRemoved ( const QModelIndex & parent, int start, int end );
    void modelReset (bool force = false);
    int maxMenuHeight();
    
    /// Max number of rows allowed
    int m_maxRows;
    int m_maxWidth;
    
    /// The model given by the user
    QAbstractItemModel *m_model;
    /// The descendends model for the one given by the user
    KDescendantsProxyModel *m_proxyModel;
    /// The filtered for the model above
    QSortFilterProxyModel *m_searchModel;
    
    /// we'll show all the children of this index in our menu
    QPersistentModelIndex m_root;
    
    QPoint m_dragStartPos;
    int m_menuRole[MenuRolesSize];
    int m_flags;
    
    /// Dirty is true if there are pending changes (as changes are only applied
    /// dynamically when menu is visible).
    bool m_dirty;
    bool m_searchActive;
    
    /// Owner of this private class
    ModelMenu *q;
    
    /// Parent of q
    ModelMenu *m_parentMenu;
    
    bool m_showSearchLine;
    ModelMenuSearchLine *m_searchLine;
    QWidgetAction *m_searchLineAction;
    
    /// Here we store which action correspond to which index
    QHash<QModelIndex, QAction*> m_actionForIndex;
    
    /// Actions shown before the model's actions
    QList<QAction*> m_preActions;
    QAction *m_preSeparator;
    
    /// Actions shown after the model's actions
    QList<QAction*> m_postActions;
    QAction *m_postSeparator;
};

ModelMenu::Private::Private(ModelMenu *parentMenu)
    : m_maxRows(-1),
    m_maxWidth(-1),
    m_model(0),
    m_proxyModel(0),
    m_searchModel(0),
    m_flags(IsRootFlag),
    m_dirty(false),
    m_searchActive(false),
    q(parentMenu),
    m_parentMenu(0),
    m_showSearchLine(false),
    m_searchLine(0),
    m_searchLineAction(0),
    m_preSeparator(0),
    m_postSeparator(0)
{
    init();
}

ModelMenu::Private::Private(ModelMenu *parentMenu, Private* copy)
    : m_maxRows(-1),
    m_maxWidth(-1),
    m_model(copy->m_model),
    m_proxyModel(copy->m_proxyModel),
    m_searchModel(copy->m_searchModel),
    m_flags(IsRootFlag),
    m_dirty(false),
    m_searchActive(false),
    q(parentMenu),
    m_parentMenu(0),
    m_showSearchLine(false),
    m_searchLine(0),
    m_searchLineAction(0),
    m_preSeparator(0),
    m_postSeparator(0)
{
    init();
}


void ModelMenu::Private::init()
{
    m_menuRole[StatusBarTextRole] = m_menuRole[SeparatorRole] = 0;
    
    connect(q, SIGNAL(aboutToShow()), q, SLOT(slotAboutToShow()));
    connect(q, SIGNAL(aboutToHide()), q, SLOT(slotAboutToHide()));
    connect(q, SIGNAL(triggered(QAction*)), q, SLOT(actionTriggered(QAction*)));
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

Q_DECLARE_METATYPE(QModelIndex)
void ModelMenu::Private::slotAboutToShow()
{
    modelReset(true);
}

void ModelMenu::Private::slotAboutToHide()
{
    if(m_showSearchLine)
    {
        m_searchLine->clearFocus();
        QFocusEvent event(QEvent::FocusOut);
        QCoreApplication::sendEvent(m_searchLine, &event);
    }
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
    
    
    int size = q->actions().size();
    
    QAction* addedAction = 0;
    QAction* tempBefore = (m_postSeparator) ? m_postSeparator : before;
    QModelIndex sourceIndex = indexToSource(index);
    
    if(!descendantFromRoot(sourceIndex))
        return;
    
    if(m_actionForIndex.contains(sourceIndex))
    {
        if(m_maxRows != -1 && size >= m_maxRows && m_searchActive)
            return;
        addedAction = q->actionForIndex(sourceIndex);
        q->insertAction(tempBefore, addedAction);
    }
    else if (q->isFolder(sourceIndex))
    {
        addedAction = createSubmenu(index, before);
    } else
    {
        if (m_menuRole[SeparatorRole] != 0
            && index.data(m_menuRole[SeparatorRole]).toBool())
        {
            addedAction = new QAction(q);
            addedAction->setSeparator(true);
        }
        else
        {
            addedAction = makeAction(index);
        }
        actionAdded(addedAction);
        
        if(m_maxRows != -1 && size >= m_maxRows && m_searchActive)
            return;
        q->insertAction(tempBefore, addedAction);
    }
    
    int maxHeight = maxMenuHeight();
    
    if(q->sizeHint().height() > maxHeight)
    {
        if(addedAction)
            q->removeAction(addedAction);
        m_maxRows = size;
    }
}

int ModelMenu::Private::maxMenuHeight()
{
    int max = QApplication::desktop()->availableGeometry((QApplication::desktop()->screenNumber(q))).height();
    int top = q->parentWidget()->geometry().topLeft().y();
    
    return qMax(max / 2, max - top) - 30;
}
void ModelMenu::Private::populateMenu()
{
    kDebug();
    if (!currentModel())
        return;
    
    int end = currentModel()->rowCount(currentRootIndex());
    
    for (int i = 0; i < end; ++i) {
        QModelIndex index = currentModel()->index(i, 0, currentRootIndex());
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

void ModelMenu::Private::actionTriggered(QAction *action)
{
    QModelIndex idx = q->index(action);
    if (idx.isValid())
        emit q->activated(idx);
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
    kDebug();
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
    emit q->rowCountChanged(m_actionForIndex.values().count());
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
            // Remove the action but not delete it. Thus it'll be still there
            // when we need it again
            q->removeAction(action);
        }
    }
    emit q->rowCountChanged(m_actionForIndex.values().count());
}

void ModelMenu::Private::modelReset (bool force)
{
    if(!force && !q->isVisible())
    {
        m_dirty = true;
        return;
    }
    
    int prevCountActions = q->actions().count();
    QListIterator<QAction*> it(m_actionForIndex.values());
    while (it.hasNext())
    {
        QAction* action = it.next();
        if(!action)
            continue;
        
        // Remove the action but not delete it. Thus it'll be still there
        // when we need it again
        q->removeAction(action);
    }

    m_maxRows = -1;
    populateMenu();
    m_dirty = false;
    
    int postCountActions = q->actions().count();
    if(prevCountActions != postCountActions)
        emit q->rowCountChanged(m_actionForIndex.values().count());
}

//@endcond

ModelMenu::ModelMenu(QWidget *parent)
    : KMenu(parent), d(new Private(this))
{
    d->m_flags = IsRootFlag;
}

ModelMenu::ModelMenu(ModelMenu *parentMenu)
    : KMenu(parentMenu), d(new Private(this, parentMenu->d))
{
    d->m_parentMenu = parentMenu;
}

ModelMenu::~ModelMenu()
{
    delete d;
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
    if(d->m_root == index)
        return;
    
    d->m_root = index;
    
    d->modelReset();
    
    emit rootChanged(index);
}

QModelIndex ModelMenu::rootIndex() const
{
    return d->m_root;
}

bool ModelMenu::setShowSearchLine(bool newSearchLine)
{
    if(!(flags() & IsRootFlag))
        return false;
    
    if(d->m_showSearchLine == newSearchLine)
        return d->m_showSearchLine;
    
    d->m_showSearchLine = newSearchLine;
    
    if(newSearchLine)
    {
        d->m_searchLineAction = new QWidgetAction(this);
        d->m_searchLine = new ModelMenuSearchLine(this);
        d->m_searchLineAction->setDefaultWidget(d->m_searchLine);
        if(d->m_preActions.empty())
        {
            addAction(d->m_searchLineAction, PreModelItems);
        } else
        {
            insertAction(d->m_preActions.first(), d->m_searchLineAction);
            d->m_preActions.prepend(d->m_searchLineAction);
        }
    }
    else
    {
        d->m_searchLineAction->deleteLater();
        d->m_searchLine = 0;
        d->m_searchLineAction = 0;
    }
    
    return d->m_showSearchLine;
}

bool ModelMenu::showSearchLine() const
{
    return d->m_showSearchLine;
}


ModelMenuSearchLine* ModelMenu::searchLine() 
{
    return d->m_searchLine;
}

bool ModelMenu::setSearchActive(bool newSearchActive)
{
    if(flags() & IsRootFlag && searchActive() != newSearchActive)
    {
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

ModelMenu *ModelMenu::createBaseMenu()
{
    return new ModelMenu(this);
}

bool ModelMenu::isFolder(const QModelIndex& index) const
{
    Q_ASSERT(model());
    
    return model()->hasChildren(index);
}

QAction *ModelMenu::makeAction(const QIcon &icon, const QString &text, QObject *parent)
{
    return new QAction(icon, text, parent);
}

QAbstractItemModel* ModelMenu::currentModel() const
{
    return d->currentModel();
}

QModelIndex ModelMenu::currentRootIndex()
{
    return d->currentRootIndex();
}

void ModelMenu::addAction(QAction *action, MenuItemLocation location)
{
    if(location == PreModelItems)
    {
        d->m_preActions.append(action);
        
        if(d->m_preActions.empty())
        {
            d->m_preSeparator = new QAction(this);
            d->m_preSeparator->setSeparator(true);
            insertAction(d->m_postSeparator, d->m_preSeparator);
            d->modelReset();
        }
        insertAction(d->m_preSeparator, action);
        
    } else if (location == PostModelItems)
    {
        d->m_postActions.append(action);
        
        if(d->m_postActions.empty())
        {
            d->m_postSeparator = new QAction(this);
            d->m_postSeparator->setSeparator(true);
            QWidget::addAction(d->m_postSeparator);
        }
        QWidget::addAction(action);
    }
}

void ModelMenu::removeAction(QAction* action, MenuItemLocation location)
{
    if(location == PreModelItems)
    {
        removeAction(action);
        d->m_preActions.removeOne(action);
        if(d->m_preActions.empty())
        {
            delete d->m_preSeparator;
            d->m_preSeparator = 0;
        }
    } else if (location == PostModelItems)
    {
        removeAction(action);
        d->m_postActions.removeOne(action);
        if(d->m_postActions.empty())
        {
            delete d->m_postSeparator;
            d->m_postSeparator = 0;
        }
    }
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

#include "modelmenu.moc"
