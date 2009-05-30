/*
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

#ifndef BOOKMARKS_H
#define BOOKMARKS_H


#include <kxmlguiwindow.h>

#include "ui_prefs_base.h"

class BookmarksView;
class QPrinter;
class KToggleAction;
class KUrl;

/**
 * This class serves as the main window for Bookmarks.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author Eduardo Robles Elvira <edulix@gmail.com>
 * @version 0.1
 */
class Bookmarks : public KXmlGuiWindow
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    Bookmarks();

    /**
     * Default Destructor
     */
    virtual ~Bookmarks();

private slots:
    void fileNew();
    void delayedInit();

private:
    void setupActions();

private:
    BookmarksView *m_view;

    KToggleAction *m_toolbarAction;
    KToggleAction *m_statusbarAction;
};

#endif // _BOOKMARKS_H_
