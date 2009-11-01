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
#include "bookmarks.h"
#include "bookmarksview.h"
#include "settings.h"
#include "actionsimpl.h"

#include <QMenu>
#include <QtGui/QDropEvent>
#include <QtGui/QPainter>
#include <QtGui/QPrinter>
#include <QtCore/QTimer>
#include <QtCore/QTimer>
#include <QtDBus>

#include <Nepomuk/ResourceManager> 
#include <kconfigdialog.h>
#include <kstatusbar.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kstandardaction.h>
#include <KLocale>
#include <akonadi/control.h>
#include <akonadi/changerecorder.h>
#include <akonadi/session.h>

#include <konqbookmark/placesmanager.h>

Bookmarks::Bookmarks()
    : KXmlGuiWindow(),
      m_view(new BookmarksView(this))
{
    Nepomuk::ResourceManager::instance()->init();
    // accept dnd
    setAcceptDrops(true);

    // tell the KXmlGuiWindow that this is indeed the main widget
    setCentralWidget(m_view);

    // then, setup our actions
    setupActions();

    // add a status bar
    statusBar()->show();

    // a call to KXmlGuiWindow::setupGUI() populates the GUI
    // with actions, using KXMLGUI.
    // It also applies the saved mainwindow settings, if any, and ask the
    // mainwindow to automatically save settings if changed: window size,
    // toolbar position, icon size, etc.
    setupGUI();
    
    // Start Akonadi
    QTimer::singleShot(0, this, SLOT(delayedInit()));
}

Bookmarks::~Bookmarks()
{
}

void Bookmarks::setupActions()
{
    ActionsImpl *actn = ActionsImpl::self();
    
    (void) KStandardAction::open(
        actn, SLOT( slotLoad() ), actionCollection());
    (void) KStandardAction::saveAs(
        actn, SLOT( slotSaveAs() ), actionCollection());

    (void) KStandardAction::cut(actn, SLOT( slotCut() ), actionCollection());
    (void) KStandardAction::copy(actn, SLOT( slotCopy() ), actionCollection());
    (void) KStandardAction::paste(actn, SLOT( slotPaste() ), actionCollection());
    
    // create Konqueror Bookmarks Resource if needed
    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusInterface *interface = new QDBusInterface("org.freedesktop.Akonadi",
        "/ResourceManager", "org.freedesktop.Akonadi.ResourceManager",  bus,
        this);
    QString resourceName("akonadi_konquerorbookmarks_resource");  
    interface->call("addResourceInstance", resourceName);
    
    // actions
    KAction* actnDelete = actionCollection()->addAction("delete");
    actnDelete->setIcon(KIcon("edit-delete"));
    actnDelete->setText(i18n("&Delete"));
    actnDelete->setShortcut(Qt::Key_Delete);
    connect(actnDelete, SIGNAL( triggered() ), m_view, SLOT( slotDelete() ));

    KAction* actnRename = actionCollection()->addAction("rename");
    actnRename->setIcon(KIcon("edit-rename"));
    actnRename->setText(i18n("Rename"));
    actnRename->setShortcut(Qt::Key_F2);
    connect(actnRename, SIGNAL( triggered() ), actn, SLOT( slotRename() ));

    KAction* actnChangeURL = actionCollection()->addAction("changeurl");
    actnChangeURL->setIcon(KIcon("edit-rename"));
    actnChangeURL->setText(i18n("C&hange URL"));
    actnChangeURL->setShortcut(Qt::Key_F3);
    connect(actnChangeURL, SIGNAL( triggered() ), actn, SLOT( slotChangeURL() ));

    KAction* actnChangeComment = actionCollection()->addAction("changecomment");
    actnChangeComment->setIcon(KIcon("edit-rename"));
    actnChangeComment->setText(i18n("C&hange Comment"));
    actnChangeComment->setShortcut(Qt::Key_F4);
    connect(actnChangeComment, SIGNAL( triggered() ), actn, SLOT( slotChangeComment() ));

    KAction* actnChangeIcon = actionCollection()->addAction("changeicon");
    actnChangeIcon->setIcon(KIcon("preferences-desktop-icons"));
    actnChangeIcon->setText(i18n("Chan&ge Icon..."));
    connect(actnChangeIcon, SIGNAL( triggered() ), actn, SLOT( slotChangeIcon() ));

    KAction* actnRecursiveSort = actionCollection()->addAction("recursivesort");
    actnRecursiveSort->setText(i18n("Recursive Sort"));
    connect(actnRecursiveSort, SIGNAL( triggered() ), actn, SLOT( slotRecursiveSort() ));

    KAction* actnNewFolder = actionCollection()->addAction("newfolder");
    actnNewFolder->setIcon(KIcon("folder-new"));
    actnNewFolder->setText(i18n("&New Folder..."));
    actnNewFolder->setShortcut(Qt::CTRL+Qt::Key_N);
    connect(actnNewFolder, SIGNAL( triggered() ), actn, SLOT( slotNewFolder() ));

    KAction* actnNewBookmark = actionCollection()->addAction("newbookmark");
    actnNewBookmark->setIcon(KIcon("bookmark-new"));
    actnNewBookmark->setText(i18n("&New Bookmark"));
    connect(actnNewBookmark, SIGNAL( triggered() ), actn, SLOT( slotNewBookmark() ));

    KAction* actnInsertSeparator = actionCollection()->addAction("insertseparator");
    actnInsertSeparator->setText(i18n("&Insert Separator"));
    actnInsertSeparator->setShortcut(Qt::CTRL+Qt::Key_I);
    connect(actnInsertSeparator, SIGNAL( triggered() ), actn, SLOT( slotInsertSeparator() ));

    KAction* actnSort = actionCollection()->addAction("sort");
    actnSort->setText(i18n("&Sort Alphabetically"));
    connect(actnSort, SIGNAL( triggered() ), actn, SLOT( slotSort() ));

    KAction* actnSetAsToolbar = actionCollection()->addAction("setastoolbar");
    actnSetAsToolbar->setIcon(KIcon("bookmark-toolbar"));
    actnSetAsToolbar->setText(i18n("Set as T&oolbar Folder"));
    connect(actnSetAsToolbar, SIGNAL( triggered() ), actn, SLOT( slotSetAsToolbar() ));

    KAction* actnExpandAll = actionCollection()->addAction("expandall");
    actnExpandAll->setText(i18n("&Expand All Folders"));
    connect(actnExpandAll, SIGNAL( triggered() ), actn, SLOT( slotExpandAll() ));

    KAction* actnCollapseAll = actionCollection()->addAction("collapseall");
    actnCollapseAll->setText(i18n("Collapse &All Folders"));
    connect(actnCollapseAll, SIGNAL( triggered() ), actn, SLOT( slotCollapseAll() ));

    KAction* actnOpenLink = actionCollection()->addAction("openinkonqueror");
    actnOpenLink->setIcon(KIcon("document-open"));
    actnOpenLink->setText(i18n("&Open in Konqueror"));
    connect(actnOpenLink, SIGNAL( triggered() ), actn, SLOT( slotOpenInKonqueror() ));

    KAction* actnImportNS = actionCollection()->addAction("importNS");
    actnImportNS->setObjectName("NS");
    actnImportNS->setIcon(KIcon("netscape"));
    actnImportNS->setText(i18n("Import &Netscape Bookmarks..."));
    connect(actnImportNS, SIGNAL( triggered() ), actn, SLOT( slotImport() ));

    KAction* actnImportOpera = actionCollection()->addAction("importOpera");
    actnImportOpera->setObjectName("Opera");
    actnImportOpera->setIcon(KIcon("opera"));
    actnImportOpera->setText(i18n("Import &Opera Bookmarks..."));
    connect(actnImportOpera, SIGNAL( triggered() ), actn, SLOT( slotImport() ));

    KAction* actnImportCrashes = actionCollection()->addAction("importCrashes");
    actnImportCrashes->setObjectName("Crashes");
    actnImportCrashes->setText(i18n("Import All &Crash Sessions as Bookmarks..."));
    connect(actnImportCrashes, SIGNAL( triggered() ), actn, SLOT( slotImport() ));

    KAction* actnImportGaleon = actionCollection()->addAction("importGaleon");
    actnImportGaleon->setObjectName("Galeon");
    actnImportGaleon->setText(i18n("Import &Galeon Bookmarks..."));
    connect(actnImportGaleon, SIGNAL( triggered() ), actn, SLOT( slotImport() ));

    KAction* actnImportKDE2 = actionCollection()->addAction("importKDE2");
    actnImportKDE2->setObjectName("KDE2");
    actnImportKDE2->setIcon(KIcon("kde"));
    actnImportKDE2->setText(i18n("Import &KDE 2 or KDE 3 Bookmarks..."));

    connect(actnImportKDE2, SIGNAL( triggered() ), actn, SLOT( slotImport() ));

    KAction* actnImportIE = actionCollection()->addAction("importIE");
    actnImportIE->setObjectName("IE");
    actnImportIE->setText(i18n("Import &Internet Explorer Bookmarks..."));
    connect(actnImportIE, SIGNAL( triggered() ), actn, SLOT( slotImport() ));

    KAction* actnImportMoz = actionCollection()->addAction("importMoz");
    actnImportMoz->setObjectName("Moz");
    actnImportMoz->setIcon(KIcon("mozilla"));
    actnImportMoz->setText(i18n("Import &Mozilla Bookmarks..."));
    connect(actnImportMoz, SIGNAL( triggered() ), actn, SLOT( slotImport() ));

    KAction* actnExportNS = actionCollection()->addAction("exportNS");
    actnExportNS->setIcon(KIcon("netscape"));
    actnExportNS->setText(i18n("Export &Netscape Bookmarks"));
    connect(actnExportNS, SIGNAL( triggered() ), actn, SLOT( slotExportNS() ));

    KAction* actnExportOpera = actionCollection()->addAction("exportOpera");
    actnExportOpera->setIcon(KIcon("opera"));
    actnExportOpera->setText(i18n("Export &Opera Bookmarks..."));
    connect(actnExportOpera, SIGNAL( triggered() ), actn, SLOT( slotExportOpera() ));

    KAction* actnExportHTML = actionCollection()->addAction("exportHTML");
    actnExportHTML->setIcon(KIcon("text-html"));
    actnExportHTML->setText(i18n("Export &HTML Bookmarks..."));
    connect(actnExportHTML, SIGNAL( triggered() ), actn, SLOT( slotExportHTML() ));

    KAction* actnExportIE = actionCollection()->addAction("exportIE");
    actnExportIE->setText(i18n("Export &Internet Explorer Bookmarks..."));
    connect(actnExportIE, SIGNAL( triggered() ), actn, SLOT( slotExportIE() ));

    KAction* actnExportMoz = actionCollection()->addAction("exportMoz");
    actnExportMoz->setIcon(KIcon("mozilla"));
    actnExportMoz->setText(i18n("Export &Mozilla Bookmarks..."));
    connect(actnExportMoz, SIGNAL( triggered() ), actn, SLOT( slotExportMoz() ));
    KStandardAction::quit(qApp, SLOT(closeAllWindows()), actionCollection());
}

void Bookmarks::delayedInit()
{
    if ( !Akonadi::Control::start( this ) ) {
        qApp->exit( -1 );
        return;
    }
    
    m_view->createModels();
    ActionsImpl::self()->setBookmarksView(m_view);
}

#include "bookmarks.moc"
