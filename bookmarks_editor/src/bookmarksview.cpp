/*
    This file is part of the KDE project
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

#include "bookmarksview.h"
#include "settings.h"

#include <QLabel>
#include <QHeaderView>
#include <QString>
#include <QAction>
 
#include <klocale.h>
#include <kmenu.h>
#include <akonadi/collectionfilterproxymodel.h>
#include <akonadi/collectionmodel.h>
#include <konqbookmark/konqbookmark.h>
#include <konqbookmark/konqbookmarkmodel.h>
#include <konqbookmark/konqbookmarkdelegate.h>

class BookmarksView::Private : public QSharedData
{
public:
    Private(BookmarksView */*parent*/) {}
    
    Akonadi::KonqBookmarkModel *mItemModel;
};

BookmarksView::BookmarksView(QWidget *)
    : d( new Private ( this ) )
{
    ui_bookmarksview_base.setupUi(this);
    setAutoFillBackground(true);
    
    QHeaderView* headerView = ui_bookmarksview_base.bookmarksView->header();
    headerView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(headerView, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(configureSettings(const QPoint&)));
}

BookmarksView::~BookmarksView()
{
    Settings::self()->writeConfig();
}

void BookmarksView::createModels()
    {
    Akonadi::CollectionModel *collectionModel = new Akonadi::CollectionModel( this );
 
    Akonadi::CollectionFilterProxyModel *filterModel = new Akonadi::CollectionFilterProxyModel( this );
    filterModel->setSourceModel( collectionModel );
    filterModel->addMimeTypeFilter( QLatin1String( "application/x-vnd.kde.konqbookmark" ) );
 
    d->mItemModel = new Akonadi::KonqBookmarkModel( this );
    Akonadi::KonqBookmarkDelegate *itemDelegate = new Akonadi::KonqBookmarkDelegate( this );
 
    ui_bookmarksview_base.collectionsView->setModel( filterModel );
    ui_bookmarksview_base.bookmarksView->setModel( d->mItemModel );
    ui_bookmarksview_base.bookmarksView->setItemDelegate( itemDelegate );
 
    connect( ui_bookmarksview_base.collectionsView, SIGNAL( currentChanged( Akonadi::Collection ) ),
        d->mItemModel, SLOT( setCollection( Akonadi::Collection ) ) );
     connect(ui_bookmarksview_base.locationComboBox,SIGNAL(returnPressed(const QString&)),
        this, SLOT(addBookmark(const QString&)));

    QStringList shownColumns = Settings::shownColumns();
        
    for (int i = Akonadi::KonqBookmarkModel::Url;
        i <= Akonadi::KonqBookmarkModel::LastVisited; ++i)
    {
        const QString columnName = ui_bookmarksview_base.bookmarksView->model()->headerData(i, Qt::Horizontal).toString();
        bool show = shownColumns.contains(columnName);
        ui_bookmarksview_base.bookmarksView->setColumnHidden(i, !show);
    }
}


void BookmarksView::configureSettings(const QPoint& pos)
{
    KMenu popup(this);
    popup.addTitle(i18nc("@title:menu", "Columns"));

    // add checkbox items for each column
    QHeaderView* headerView = ui_bookmarksview_base.bookmarksView->header();
    
    for (int i = Akonadi::KonqBookmarkModel::Url;
        i <= Akonadi::KonqBookmarkModel::LastVisited; ++i)
    {
        const QString text = ui_bookmarksview_base.bookmarksView->model()->headerData(i, Qt::Horizontal).toString();
        QAction* action = popup.addAction(text);
        action->setCheckable(true);
        action->setChecked(!headerView->isSectionHidden(i));
        action->setData(i);
    }
    popup.addSeparator();

    QAction* activatedAction = popup.exec(ui_bookmarksview_base.bookmarksView->header()->mapToGlobal(pos));
    if (activatedAction != 0) {
        const bool show = activatedAction->isChecked();
        const int columnIndex = activatedAction->data().toInt();

        QStringList shownColumns = Settings::shownColumns();
        QString colName = activatedAction->text().remove(QChar('&'));
        if(show)
            shownColumns.append(colName);
        else
            shownColumns.removeOne(colName);
        
        kDebug() << "Settings::setShownColumns " << shownColumns;
        Settings::setShownColumns(shownColumns);
        
        ui_bookmarksview_base.bookmarksView->setColumnHidden(columnIndex, !show);
    }
}

void BookmarksView::addBookmark(const QString& bookmarkUrl)
{
    KonqBookmark bookmark;
    bookmark.setUrl(bookmarkUrl);
    d->mItemModel->addBookmark(bookmark);
}


#include "bookmarksview.moc"
