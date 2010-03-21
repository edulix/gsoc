/*
    This file is part of the KDE project
    Copyright (C) 2000 David Faure <faure@kde.org>
    Copyright (C) 2002-2003 Alexander Kellett <lypanov@kde.org>
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

#include "actionsimpl.h"
#include <kinputdialog.h>

ActionsImpl *ActionsImpl::s_self = 0;

ActionsImpl::ActionsImpl()
    : mBookmarksView(0)
{
}

ActionsImpl::~ActionsImpl()
{
}

void ActionsImpl::setBookmarksView(BookmarksView *view)
{
    mBookmarksView = view;
}

void ActionsImpl::slotLoad()
{
    //TODO
}

void ActionsImpl::slotSaveAs()
{
    //TODO
}


void ActionsImpl::slotCut()
{
    //TODO
}


void ActionsImpl::slotCopy()
{
    //TODO
}


void ActionsImpl::slotPaste()
{
    //TODO
}


void ActionsImpl::slotRename()
{
    //TODO
}


void ActionsImpl::slotChangeURL()
{
    //TODO
}


void ActionsImpl::slotChangeComment()
{
    //TODO
}


void ActionsImpl::slotChangeIcon()
{
    //TODO
}


void ActionsImpl::slotDelete()
{
    //TODO
}


void ActionsImpl::slotNewFolder()
{

    bool ok;
    QString folderName;
    while (!ok) {
         folderName = KInputDialog::getText(i18n("Create New Bookmark Folder"),
            i18n("New folder:"), QString(), &ok, mBookmarksView);
        if (!ok) {
            return;
        }

        // Folder name can't be empty
        if (folderName.isEmpty()) {
            ok = false;
        }
    }

    mBookmarksView->slotAddFolder(folderName);

}


void ActionsImpl::slotNewBookmark()
{

    mBookmarksView->addBookmark();
}


void ActionsImpl::slotInsertSeparator()
{
    //TODO
}


void ActionsImpl::slotSort()
{
    //TODO
}


void ActionsImpl::slotSetAsToolbar()
{
    //TODO
}


void ActionsImpl::slotOpenInKonqueror()
{
    //TODO
}


void ActionsImpl::slotRecursiveSort()
{
    //TODO
}


void ActionsImpl::slotExpandAll()
{
    //TODO
}


void ActionsImpl::slotCollapseAll()
{
    //TODO
}


void ActionsImpl::slotImport()
{
    //TODO
}


void ActionsImpl::slotExportOpera()
{
    //TODO
}


void ActionsImpl::slotExportHTML()
{
    //TODO
}


void ActionsImpl::slotExportIE()
{
    //TODO
}


void ActionsImpl::slotExportNS()
{
    //TODO
}


void ActionsImpl::slotExportMoz()
{
    //TODO
}
