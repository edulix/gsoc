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

#include "konqbookmarkmodelmenu.h"
#include "itemmodels/konqbookmarkmodel.h"

#include <kbookmarkmenu.h>
#include <kdebug.h>
#include <QWidgetAction>
#include <QPushButton>
#include <QStyle>

class KonqBookmarkModelMenu::Private {
public:
    Private(KonqBookmarkModelMenu *parent);
    
    void setChildAsRoot(const QModelIndex& index);
public:
    KonqBookmarkMenuHelper *m_KonqBookmarkMenuHelper;
    KonqBookmarkModelMenu *m_parent;
};

KonqBookmarkModelMenu::Private::Private(KonqBookmarkModelMenu *parent)
    : m_parent(parent)
{

}

void KonqBookmarkModelMenu::Private::setChildAsRoot(const QModelIndex& index)
{
    if(index.isValid())
    {
        m_parent->setRootIndex(index);
        disconnect(m_parent->model(), SIGNAL(rowsInserted(const QModelIndex&, int, int)),
            m_parent, SLOT(setChildAsRoot(const QModelIndex&)));
    }
}

KonqBookmarkModelMenu::KonqBookmarkModelMenu(QAbstractItemModel* model, KonqBookmarkMenuHelper *KonqBookmarkMenuHelper, QWidget *parent)
    : ModelMenu(parent),  d( new Private(this) )
{
    kDebug();
    setModel(model);
    d->m_KonqBookmarkMenuHelper = KonqBookmarkMenuHelper;
    
    // We want to set "Konqueror Bookmarks" as root
    connect(model, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
        this, SLOT(setChildAsRoot(const QModelIndex&)));
}

KonqBookmarkModelMenu::~KonqBookmarkModelMenu()
{
    delete d;
}

KonqBookmarkModelMenu::KonqBookmarkModelMenu(QAbstractItemModel* model, KonqBookmarkMenuHelper *KonqBookmarkMenuHelper, KonqBookmarkModelMenu *parent)
    : ModelMenu(parent),  d( new Private(this) )
{
    kDebug();
    setModel(model);
    d->m_KonqBookmarkMenuHelper = KonqBookmarkMenuHelper;
}

bool KonqBookmarkModelMenu::prePopulated()
{
    if(flags() & IsRootFlag)
    {
        
        QWidgetAction *widgetAction = new QWidgetAction(this);
        widgetAction->setDefaultWidget(new QPushButton("Funciona!"));
        addAction(widgetAction);
        QTimer::singleShot(1000, widgetAction, SLOT(deleteLater()));
        
        return true;
    }
    
    return false;
}

void KonqBookmarkModelMenu::postPopulated()
{
    
}

ModelMenu *KonqBookmarkModelMenu::createBaseMenu()
{
    return new KonqBookmarkModelMenu(model(), d->m_KonqBookmarkMenuHelper, this);
}

QAction *KonqBookmarkModelMenu::makeAction(const QIcon &icon, const QString &text, QObject *parent)
{
    return ModelMenu::makeAction(icon, text, parent);
}

void KonqBookmarkModelMenu::openBookmark(Qt::MouseButtons mb, Qt::KeyboardModifiers km)
{
    QAction *action = qobject_cast<QAction*>(sender());
    if(!action)
        return;
    
    KUrl url(index(action).data(Akonadi::KonqBookmarkModel::Url).toString());
    d->m_KonqBookmarkMenuHelper->openBookmark(url, mb, km);
}

KMenu *KonqBookmarkModelMenu::contextMenu(QAction * action )
{
    Q_UNUSED(action);
    return 0;
}

#include "konqbookmarkmodelmenu.moc"
