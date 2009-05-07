/*
 * Bookmarks.h
 *
 * Copyright (C) 2008 Eduardo Robles Elvira <edulix@gmail.com>
 */
#ifndef Bookmarks_H
#define Bookmarks_H


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
    void optionsPreferences();
    void delayedInit();

private:
    void setupActions();

private:
    Ui::prefs_base ui_prefs_base ;
    BookmarksView *m_view;

    QPrinter   *m_printer;
    KToggleAction *m_toolbarAction;
    KToggleAction *m_statusbarAction;
};

#endif // _Bookmarks_H_
