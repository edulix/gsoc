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

#include <QtGui/QLabel>
 
#include <klocale.h>
#include <akonadi/collectionfilterproxymodel.h>
#include <akonadi/collectionmodel.h>
#include <konqbookmark/konqbookmarkmodel.h>

BookmarksView::BookmarksView(QWidget *)
{
    ui_bookmarksview_base.setupUi(this);
    settingsChanged();
    setAutoFillBackground(true);
}

BookmarksView::~BookmarksView()
{

}

void BookmarksView::createModels()
{
    Akonadi::CollectionModel *collectionModel = new Akonadi::CollectionModel( this );
 
    Akonadi::CollectionFilterProxyModel *filterModel = new Akonadi::CollectionFilterProxyModel( this );
    filterModel->setSourceModel( collectionModel );
    filterModel->addMimeTypeFilter( QLatin1String( "application/x-vnd.kde.konqbookmark" ) );
 
    Akonadi::ItemModel *itemModel = new Akonadi::KonqBookmarkModel( this );
 
    ui_bookmarksview_base.collectionsView->setModel( filterModel );
    ui_bookmarksview_base.bookmarksView->setModel( itemModel );
 
    connect( ui_bookmarksview_base.collectionsView, SIGNAL( currentChanged( Akonadi::Collection ) ),
        itemModel, SLOT( setCollection( Akonadi::Collection ) ) );

}

void BookmarksView::switchColors()
{
    // switch the foreground/background colors of the label
    QColor color = Settings::col_background();
    Settings::setCol_background( Settings::col_foreground() );
    Settings::setCol_foreground( color );

    settingsChanged();
}

void BookmarksView::settingsChanged()
{
    QPalette pal;
    pal.setColor( QPalette::Window, Settings::col_background());
    pal.setColor( QPalette::WindowText, Settings::col_foreground());

    // i18n : internationalization
    emit signalChangeStatusbar( i18n("Settings changed") );
}

#include "bookmarksview.moc"
