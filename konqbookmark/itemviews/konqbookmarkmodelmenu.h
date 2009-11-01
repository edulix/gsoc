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

#ifndef KONQBOOKMARK_MODEL_MENU_H
#define KONQBOOKMARK_MODEL_MENU_H

#include "konqbookmark_export.h"

#include "konqbookmark.h"
#include "modelmenu.h"

#include <QAbstractItemModel>
#include <QList>
#include <KUrl>

#include <kactioncollection.h>
#include <konqbookmarkmenu.h>
#include <akonadi/collection.h>

class KonqBookmarkMenuHelper;
class KActionCollection;
class KONQBOOKMARK_EXPORT KonqBookmarkModelMenu : public ModelMenu
{
    Q_OBJECT
public:
    KonqBookmarkModelMenu(QAbstractItemModel* model, KonqBookmarkMenuHelper *konqBookmarkOwner, KActionCollection *actionCollection, QWidget *parent = 0);
    
    virtual ~KonqBookmarkModelMenu();

protected:
    KonqBookmarkModelMenu(KonqBookmarkMenuHelper *konqBookmarkOwner, KActionCollection *actionCollection, KonqBookmarkModelMenu *parent = 0);
    // return the QMenu that is used to populate sub menu's
    virtual ModelMenu *createBaseMenu();
    
    virtual QAction *makeAction(const QIcon &icon, const QString &text, QObject *parent);
    
    virtual KMenu * contextMenu(const QModelIndex& index);
    
    virtual bool isFolder(const QModelIndex& index) const;
    
protected Q_SLOTS:
    /**
     * Received from QActions when activated.
     */
    void openBookmark(const QModelIndex& index);
    
    /**
     * @short Set ups necessary helper actions in the menu.
     *
     * Called when the number of items changes in the model and also by the
     * constructor.
     */ 
    void updateActions(const int& count);
    
private:
    /**
     * Shared initialization functions by the constructors.
     */
    void init();
    
    class Private;
    Private* const d;
    
    Q_PRIVATE_SLOT(d, void setChildAsRoot(const QModelIndex& index))
    Q_PRIVATE_SLOT(d, void slotOpenFolderInTabs())
    Q_PRIVATE_SLOT(d, void slotAddBookmark())
    Q_PRIVATE_SLOT(d, void slotNewFolder())
    Q_PRIVATE_SLOT(d, void slotEditBookmarks())
};

class KONQBOOKMARK_EXPORT KonqBookmarkMenuHelper
{
public:
    KonqBookmarkMenuHelper() {}
    virtual ~KonqBookmarkMenuHelper() {}
    
    virtual KonqBookmark currentPlace() const = 0;
    virtual bool supportsTabs() const { return false; }
    virtual QList<KonqBookmark> currentPlacesList() const { return QList<KonqBookmark>(); }
    virtual bool enableOption(const QString &option) const = 0;
    
    virtual void openInNewTabs(const QList<KUrl> &urls) { Q_UNUSED(urls); }
    
    virtual void openBookmark(const KUrl& url) = 0;
    virtual void openInNewTab(const KUrl &url) = 0;
    virtual void openInNewWindow(const KUrl &url) = 0;
    
    virtual void createNewFolder(Akonadi::Collection parent) = 0;
    virtual void addBookmark(Akonadi::Collection parent) = 0;
    virtual void editBookmarks() = 0;
};


#endif // KONQBOOKMARK_MODEL_MENU_H
