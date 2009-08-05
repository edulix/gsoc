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
#include "modelmenusearchline.h"
#include "itemmodels/konqbookmarkmodel.h"

#include <akonadi/entitytreemodel.h>
#include <kbookmarkmenu.h>
#include <kdebug.h>
#include <kaction.h>
#include <klineedit.h>
#include <kauthorized.h>
#include <QWidgetAction>
#include <QCoreApplication>
#include <QPushButton>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QStyle>
#include <QAction>
#include <QHash>
/**
 * Private class that helps to provide binary compatibility between releases.
 * @internal
 */
//@cond PRIVATE
class KonqBookmarkModelMenu::Private
{
public:
    Private(KonqBookmarkModelMenu *parent);
    virtual ~Private() {}
    
    void setChildAsRoot(const QModelIndex& index);
    
    void updateOpenInTabs(const int& count);
    void updateEditBookmarks(const int& count);
    void updateNewFolder(const int& count);
    void updateAddBookmark(const int& count);
    
    void slotOpenFolderInTabs();
    void slotNewFolder();
    void slotAddBookmark();
    
public:
    KonqBookmarkMenuHelper *m_helper;
    KonqBookmarkModelMenu *q;
    QHash<QString, QAction *> m_helperActions;
};

KonqBookmarkModelMenu::Private::Private(KonqBookmarkModelMenu *parent)
    : q(parent)
{

}

void KonqBookmarkModelMenu::Private::setChildAsRoot(const QModelIndex& index)
{
    if(index.isValid())
    {
        kDebug() << index.data();
        q->setRootIndex(index);
        kDebug() << q->rootIndex().data();
        disconnect(q->model(), SIGNAL(rowsInserted(const QModelIndex&, int, int)),
            q, SLOT(setChildAsRoot(const QModelIndex&)));
    }
}

void KonqBookmarkModelMenu::Private::updateOpenInTabs(const int& count)
{
    // If not supported then we have nothing to do
    if( !m_helper || !m_helper->enableOption("openintabs") || !KAuthorized::authorizeKAction("bookmarks") )
        return;
    
    // If action was added previously, check if it shoul still be there.
    // Remove action in case no bookmark is present
    if(m_helperActions.contains("openintabs") && count == 0)
    {
       delete m_helperActions["openintabs"]; 
       m_helperActions.remove("openintabs");
       return;
    }
    
    // If action was not added but no bookmark is shown, then action should not
    // be added so we finish here
    if(count == 0)
        return;

    QString title = i18n("Open Folder in Tabs");

    KAction * openFolderInTabs = new KAction(title, q);
    openFolderInTabs->setIcon(KIcon("tab-new"));
    openFolderInTabs->setToolTip(i18n("Open all bookmarks in this folder as a new tab."));
    connect( openFolderInTabs, SIGNAL(triggered(bool)), q, SLOT(slotOpenFolderInTabs()));
    q->addAction(openFolderInTabs, PreModelItems);
    m_helperActions["openintabs"] = openFolderInTabs;
}


void KonqBookmarkModelMenu::Private::updateEditBookmarks(const int& count)
{
    Q_UNUSED(count);
    
    // If not supported then we have nothing to do
    if( !m_helper || !m_helper->enableOption("editbookmarks") || !KAuthorized::authorizeKAction("bookmarks") )
        return;

    QString title = i18n("Edit Bookmarks...");

    KAction * editBookmarks = new KAction(title, q);
    editBookmarks->setIcon(KIcon("edit_bookmarks"));
    editBookmarks->setToolTip(i18n("Edit your bookmark collection in a separate window"));
    connect( editBookmarks, SIGNAL(triggered(bool)), q, SLOT(slotEditBookmarks()));
    q->addAction(editBookmarks, PreModelItems);
    m_helperActions["openintabs"] = editBookmarks;
}

void KonqBookmarkModelMenu::Private::updateNewFolder(const int& /*count*/)
{
    // If not supported then we have nothing to do
    if( !m_helper || !m_helper->enableOption("newfolder") || !KAuthorized::authorizeKAction("bookmarks") )
        return;

    QString title = i18n("New Bookmark Folder...");

    KAction *newBookmarkFolder = new KAction(title, q);
    newBookmarkFolder->setIcon(KIcon("folder-new"));
    newBookmarkFolder->setToolTip(i18n("Create a new bookmark folder in this menu"));
    connect( newBookmarkFolder, SIGNAL(triggered(bool)), q, SLOT(slotNewFolder()));
    q->addAction(newBookmarkFolder, PreModelItems);
    m_helperActions["openintabs"] = newBookmarkFolder;
}

void KonqBookmarkModelMenu::Private::updateAddBookmark(const int& /*count*/)
{
    // If not supported then we have nothing to do
    if( !m_helper || !m_helper->enableOption("addbookmark") || !KAuthorized::authorizeKAction("bookmarks") )
        return;

    QString title = i18n("Add Bookmark...");

    KAction *addBookmark = new KAction(title, q);
    addBookmark->setIcon(KIcon("addbookmark"));
    addBookmark->setToolTip(i18n("Add Bookmark to current bookmark"));
    addBookmark->setShortcut( QKeySequence() );
    connect( addBookmark, SIGNAL(triggered(bool)), q, SLOT(slotAddBookmark()));
    q->addAction(addBookmark, PreModelItems);
    m_helperActions["addbookmark"] = addBookmark;
}

void KonqBookmarkModelMenu::Private::slotOpenFolderInTabs()
{
    Q_ASSERT(m_helper);
    
    QList<KUrl> urls;
    int size = q->currentModel()->rowCount(q->currentRootIndex());
    
    for (int i = 0; i < size; ++i) {
        QModelIndex index = q->currentModel()->index(i, 0, q->currentRootIndex());
        if(!q->isFolder(index))
        {
            urls.append(KUrl(index.data(Akonadi::KonqBookmarkModel::Url).toString()));
        }
    }
    
    m_helper->openInNewTabs(urls);
}

void KonqBookmarkModelMenu::Private::slotNewFolder()
{
    if(!m_helper)
        return;
    
    Akonadi::Collection collection = 
        qVariantValue<Akonadi::Collection>(q->currentRootIndex().data(Akonadi::EntityTreeModel::CollectionRole));
    if(!collection.isValid())
        return;
    
    m_helper->createNewFolder(collection);
}

void KonqBookmarkModelMenu::Private::slotAddBookmark()
{
    if(!m_helper)
        return;
    
    Akonadi::Collection collection = 
        qVariantValue<Akonadi::Collection>(q->currentRootIndex().data(Akonadi::EntityTreeModel::CollectionRole));
    if(!collection.isValid())
        return;
    
    m_helper->addBookmark(collection);
}
//@endcond

KonqBookmarkModelMenu::KonqBookmarkModelMenu(QAbstractItemModel* model, KonqBookmarkMenuHelper *KonqBookmarkMenuHelper, QWidget *parent)
    : ModelMenu(parent),  d( new Private(this) )
{
    setModel(model);
    setFlags(flags() | OneColumnFlag);
    setShowSearchLine(true);
    d->m_helper = KonqBookmarkMenuHelper;
    
    // We want to set "Konqueror Bookmarks" as root
    connect(model, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
        this, SLOT(setChildAsRoot(const QModelIndex&)));
        
    init();
}

KonqBookmarkModelMenu::KonqBookmarkModelMenu(KonqBookmarkMenuHelper *konqBookmarkMenuHelper, KonqBookmarkModelMenu *parent)
    : ModelMenu(parent),  d( new Private(this) )
{
    d->m_helper = konqBookmarkMenuHelper;
    connect(this, SIGNAL(activated(const QModelIndex &)), this, SLOT(openBookmark(const QModelIndex &)));
        
    init();
}

void KonqBookmarkModelMenu::init()
{
    int childCount = model()->rowCount(rootIndex());
    updateActions(childCount);
}

KonqBookmarkModelMenu::~KonqBookmarkModelMenu()
{
    delete d;
}

ModelMenu *KonqBookmarkModelMenu::createBaseMenu()
{
    return new KonqBookmarkModelMenu(d->m_helper, this);
}

QAction *KonqBookmarkModelMenu::makeAction(const QIcon &icon, const QString &text, QObject *parent)
{
    return ModelMenu::makeAction(icon, text, parent);
}

void KonqBookmarkModelMenu::openBookmark(const QModelIndex& index)
{
    if(!index.isValid())
        return;
    
    KUrl url(index.data(Akonadi::KonqBookmarkModel::Url).toString());
    d->m_helper->openBookmark(url);
}

KMenu *KonqBookmarkModelMenu::contextMenu(const QModelIndex& index)
{
    return ModelMenu::contextMenu(index);
}

void KonqBookmarkModelMenu::updateActions(const int& count)
{
    if(flags() & IsRootFlag)
    {
//         addAddBookmark();
//         addAddBookmarksList();
        d->updateNewFolder(count);
        d->updateEditBookmarks(count);
    }
    else
    {
        d->updateOpenInTabs(count);
    //         addAddBookmark();
    //         addAddBookmarksList();
        d->updateNewFolder(count);
    }
}

bool KonqBookmarkModelMenu::isFolder(const QModelIndex& index) const
{
    if (!index.isValid())
        return false;
    
    Akonadi::Collection collection = 
        qVariantValue<Akonadi::Collection>(index.data(Akonadi::EntityTreeModel::CollectionRole));
    return collection.isValid();
}




#include "konqbookmarkmodelmenu.moc"
