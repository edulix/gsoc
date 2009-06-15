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
#include <QDataWidgetMapper>
     
#include <klocale.h>
#include <kmenu.h>
#include <klineedit.h>
#include <akonadi/collectionfilterproxymodel.h>
#include <akonadi/collectionmodel.h>
#include <akonadi/item.h>
#include <konqbookmark/konqbookmarkmodel.h>
#include <konqbookmark/konqbookmarkdelegate.h>

class BookmarksView::Private : public QSharedData
{
public:
    Private(BookmarksView */*parent*/) {}
    
    Akonadi::KonqBookmarkModel *mItemModel;
    QModelIndex mCurrentIndex;
    QDataWidgetMapper *mMapper;
};

BookmarksView::BookmarksView(QWidget *)
    : d( new Private ( this ) )
{
    ui_bookmarksview_base.setupUi(this);
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
 
    d->mItemModel = new Akonadi::KonqBookmarkModel( this );
    Akonadi::KonqBookmarkDelegate *itemDelegate = new Akonadi::KonqBookmarkDelegate( this );
 
    ui_bookmarksview_base.collectionsView->setModel( filterModel );
    ui_bookmarksview_base.bookmarksView->setModel( d->mItemModel );
    ui_bookmarksview_base.bookmarksView->setItemDelegate( itemDelegate );
    
    connect( ui_bookmarksview_base.collectionsView, SIGNAL( currentChanged( Akonadi::Collection ) ),
        d->mItemModel, SLOT( setCollection( Akonadi::Collection ) ) );
    connect( ui_bookmarksview_base.bookmarksView, SIGNAL( currentChanged( const KonqBookmark&, const QModelIndex & ) ),
        this, SLOT( setCurrentBookmark( const KonqBookmark&, const QModelIndex & ) ) );
    
    d->mMapper = new QDataWidgetMapper(this);
    d->mMapper->setModel(d->mItemModel);
    d->mMapper->addMapping(ui_bookmarksview_base.titleBox, Akonadi::KonqBookmarkModel::Title);
    d->mMapper->addMapping(ui_bookmarksview_base.addressBox, Akonadi::KonqBookmarkModel::Url);
    d->mMapper->addMapping(ui_bookmarksview_base.tagsBox, Akonadi::KonqBookmarkModel::Tags);
    d->mMapper->addMapping(ui_bookmarksview_base.descriptionBox, Akonadi::KonqBookmarkModel::Description);
    d->mMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
}

void BookmarksView::addBookmark()
{
    KonqBookmark bookmark;
    const QModelIndex& currentIndex = d->mItemModel->addBookmark(bookmark);
    setCurrentBookmark(bookmark, currentIndex);
    ui_bookmarksview_base.bookmarksView->setCurrentIndex(currentIndex);
}

void BookmarksView::slotDelete()
{
    d->mItemModel->removeRow(ui_bookmarksview_base.bookmarksView->currentIndex().row());
}

void BookmarksView::setCurrentBookmark( const KonqBookmark& item, const QModelIndex &currentIndex)
{
    d->mCurrentIndex = currentIndex;
    d->mMapper->setCurrentIndex(currentIndex.row());
}

#include "bookmarksview.moc"
