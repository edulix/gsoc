/*
    This file is part of the KDE project
    Copyright (C) 2003 Alexander Kellett <lypanov@kde.org>
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

#ifndef ACTIONSIMPL_H
#define ACTIONSIMPL_H

#include <QObject>
#include <QSharedDataPointer>

#include "bookmarksview.h"

class BookmarksView;
class ActionsImpl : public QObject
{
   Q_OBJECT

public Q_SLOTS:
    void slotLoad();
    void slotSaveAs();
    void slotCut();
    void slotCopy();
    void slotPaste();
    void slotRename();
    void slotChangeURL();
    void slotChangeComment();
    void slotChangeIcon();
    void slotDelete();
    void slotNewFolder();
    void slotNewBookmark();
    void slotInsertSeparator();
    void slotSort();
    void slotSetAsToolbar();
    void slotOpenInKonqueror();
    void slotRecursiveSort();
    void slotExpandAll();
    void slotCollapseAll();
    void slotImport();
    void slotExportOpera();
    void slotExportHTML();
    void slotExportIE();
    void slotExportNS();
    void slotExportMoz();
    void setBookmarksView(BookmarksView* view);

    static ActionsImpl* self() { if (!s_self) { s_self = new ActionsImpl(); }; return s_self; }
public:
    ~ActionsImpl();
    static ActionsImpl* s_self;

private:
    ActionsImpl();
    BookmarksView *mBookmarksView;
   
};

#endif // ACTIONSIMPL_H
