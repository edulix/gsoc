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
#include "watchitemcreatejob.h"
#include "settings.h"

#include <konqbookmark/konqbookmark.h>

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
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemfetchscope.h>
#include <akonadi/monitor.h>
#include <akonadi/session.h>
#include <akonadi_next/entitytreemodel.h>
#include <akonadi/item.h>
#include <kjob.h>
#include <konqbookmark/konqbookmarkmodel.h>
#include <konqbookmark/konqbookmarkdelegate.h>

using namespace Akonadi;

class BookmarksView::Private : public QSharedData
{
public:
    Private(BookmarksView */*parent*/) {}
    
    Akonadi::KonqBookmarkModel *mBookmarkModel;
    QDataWidgetMapper *mMapper;
    ModelWatcher *mModelWatcher;
    Akonadi::Monitor *mMonitor;
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
    filterModel->addMimeTypeFilter( KonqBookmark::mimetype() );
 
    Akonadi::Session *session = new Akonadi::Session(QByteArray( "BookmarksView-" ) + QByteArray::number( qrand() ), this);
    
    // Configure what should be shown in the model:
    Akonadi::ItemFetchScope scope;
    scope.fetchFullPayload( true );

    d->mMonitor = new Monitor( this );
    
    d->mBookmarkModel = new Akonadi::KonqBookmarkModel( session, d->mMonitor, this );
    
    Akonadi::KonqBookmarkDelegate *itemDelegate = new Akonadi::KonqBookmarkDelegate( this );
 
    ui_bookmarksview_base.collectionsView->setModel( filterModel );
    ui_bookmarksview_base.bookmarksView->setModel( d->mBookmarkModel );
    ui_bookmarksview_base.bookmarksView->setItemDelegate( itemDelegate );
    
    connect( ui_bookmarksview_base.collectionsView, SIGNAL( currentChanged( Akonadi::Collection ) ),
        this, SLOT( setRootCollection( const Akonadi::Collection& ) ) );
    
    d->mMapper = new QDataWidgetMapper(this);
    d->mMapper->setModel(d->mBookmarkModel);
    d->mMapper->addMapping(ui_bookmarksview_base.titleBox, Akonadi::KonqBookmarkModel::Title);
    d->mMapper->addMapping(ui_bookmarksview_base.addressBox, Akonadi::KonqBookmarkModel::Url);
    d->mMapper->addMapping(ui_bookmarksview_base.tagsBox, Akonadi::KonqBookmarkModel::Tags);
    d->mMapper->addMapping(ui_bookmarksview_base.descriptionBox, Akonadi::KonqBookmarkModel::Description);
    d->mMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

     connect(ui_bookmarksview_base.bookmarksView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
         d->mMapper, SLOT(setCurrentModelIndex(QModelIndex)));
    
    d->mModelWatcher = 0;
}

void BookmarksView::setRootCollection( const Akonadi::Collection& collection)
{
    d->mBookmarkModel->setRootCollection(collection);
}

void BookmarksView::addBookmark()
{
    KonqBookmark bookmark;
    
    Item item;
    item.setMimeType( KonqBookmark::mimetype() );
    item.setPayload<KonqBookmark>( bookmark );
    
    ItemCreateJob *job = new ItemCreateJob(item, d->mBookmarkModel->rootCollection());
    if( job->exec() )
    {
        delete d->mModelWatcher;
        d->mModelWatcher = new ModelWatcher(job->item().id(), d->mBookmarkModel->rootCollection().id(), d->mBookmarkModel, this);
        connect(d->mModelWatcher, SIGNAL(newItem(const QModelIndex &)), this, SLOT(slotBookmarkAdded(const QModelIndex &)));
    } else
        kWarning() << "job->exec() failed";
}


void BookmarksView::slotBookmarkAdded(const QModelIndex &newIndex)
{
    kDebug();
    d->mMapper->setCurrentIndex(newIndex.row());
    ui_bookmarksview_base.bookmarksView->setCurrentIndex(newIndex);
    delete d->mModelWatcher;
    d->mModelWatcher = 0;
}

void BookmarksView::slotDelete()
{
    d->mBookmarkModel->removeRows(ui_bookmarksview_base.bookmarksView->currentIndex().row(), 1);
}


#include "bookmarksview.moc"
