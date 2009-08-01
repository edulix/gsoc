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

class KonqBookmarkMenuHelper;
class KONQBOOKMARK_EXPORT KonqBookmarkModelMenu : public ModelMenu
{
    Q_OBJECT
public:
    KonqBookmarkModelMenu(QAbstractItemModel* model, KonqBookmarkMenuHelper *konqBookmarkOwner, QWidget *parent = 0);
    virtual ~KonqBookmarkModelMenu();
protected:
    KonqBookmarkModelMenu(KonqBookmarkMenuHelper *konqBookmarkOwner, KonqBookmarkModelMenu *parent = 0);
    // return the QMenu that is used to populate sub menu's
    virtual ModelMenu *createBaseMenu();
    
    virtual QAction *makeAction(const QIcon &icon, const QString &text, QObject *parent);
    virtual KMenu * contextMenu(const QModelIndex& index);
    
protected Q_SLOTS:
    /**
     * Received from QActions when selected.
     */
    virtual void openBookmark(Qt::MouseButtons mb, Qt::KeyboardModifiers km);
    
private:
    class Private;
    Private* const d;
    
    Q_PRIVATE_SLOT(d, void setChildAsRoot(const QModelIndex& index))
};


class KONQBOOKMARK_EXPORT KonqBookmarkMenuHelper
{
public:
    KonqBookmarkMenuHelper() {}
    virtual ~KonqBookmarkMenuHelper() {}
    
    virtual KonqBookmark currentPlace() const = 0;
    virtual bool supportsTabs() const { return false; }
    virtual QList<KonqBookmark> currentPlacesList() const { return QList<KonqBookmark>(); }
    
    virtual void openInNewTabs(const QList<KUrl> &urls) { Q_UNUSED(urls); }
    
    virtual void openBookmark(const KUrl& url, Qt::MouseButtons mb, Qt::KeyboardModifiers km) = 0;
    virtual void openInNewTab(const KUrl &url) = 0;
    virtual void openInNewWindow(const KUrl &url) = 0;
};

#endif // KONQBOOKMARK_MODEL_MENU_H
