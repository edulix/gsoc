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


#include <QTimer>
#include <QLabel>
#include <QHeaderView>
#include <QString>
#include <QAction>
#include <QDataWidgetMapper>
#include <QItemDelegate>

#include <kmessagebox.h>
#include <klocale.h>
#include <kmenu.h>
#include <klineedit.h>
#include <akonadi/collectionfilterproxymodel.h>
#include <akonadi/collectionmodel.h>
#include <akonadi/collectioncreatejob.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemfetchscope.h>
#include <akonadi/monitor.h>
#include <akonadi/session.h>
#include <akonadi_next/entitytreemodel.h>
#include <akonadi/item.h>
#include <kjob.h>

#include <konqbookmark/konqbookmark.h>
#include <konqbookmark/konqbookmarkproxymodel.h>
#include <konqbookmark/konqbookmarkmodel.h>
#include <konqbookmark/konqbookmarkdelegate.h>

using namespace Akonadi;

class BookmarksView::Private : public QSharedData
{
public:
    Private(BookmarksView */*parent*/) {}
    
    Akonadi::KonqBookmarkModel *mBookmarkModel;
    Akonadi::KonqBookmarkProxyModel *mBookmarkProxyModel;
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
    filterModel->addMimeTypeFilter( KonqBookmark::mimeType() );
 
    Akonadi::Session *session = new Akonadi::Session(QByteArray( "BookmarksView-" ) + QByteArray::number( qrand() ), this);

    d->mMonitor = new Monitor( this );
    
    d->mBookmarkModel = new Akonadi::KonqBookmarkModel( session, d->mMonitor, this );
    d->mBookmarkProxyModel = new Akonadi::KonqBookmarkProxyModel( this );
    d->mBookmarkProxyModel->setSourceModel(d->mBookmarkModel);
    
    Akonadi::KonqBookmarkDelegate *itemDelegate = new Akonadi::KonqBookmarkDelegate( this );
 
    ui_bookmarksview_base.collectionsView->setModel( filterModel );
    ui_bookmarksview_base.bookmarksView->setModel( d->mBookmarkProxyModel );
    ui_bookmarksview_base.navigatorBreadCrumb->setModel( d->mBookmarkProxyModel );
    ui_bookmarksview_base.bookmarksView->setItemDelegate( itemDelegate );
    ui_bookmarksview_base.searchBox->setTreeView( ui_bookmarksview_base.bookmarksView );
    ui_bookmarksview_base.searchBox->setClickMessage(i18n("Search in bookmarks.."));
    
    connect( ui_bookmarksview_base.navigatorBreadCrumb, SIGNAL( currentChanged( const QModelIndex& ) ),
        this, SLOT( setRootIndex( const QModelIndex& ) ) );
        
    connect( ui_bookmarksview_base.collectionsView, SIGNAL( currentChanged( Akonadi::Collection ) ),
        this, SLOT( setRootCollection( const Akonadi::Collection& ) ) );
        
    d->mMapper = new QDataWidgetMapper(this);
    d->mMapper->setModel(d->mBookmarkProxyModel);
    d->mMapper->addMapping(ui_bookmarksview_base.titleBox, Akonadi::KonqBookmarkModel::Title);
    d->mMapper->addMapping(ui_bookmarksview_base.addressBox, Akonadi::KonqBookmarkModel::Url);
    d->mMapper->addMapping(ui_bookmarksview_base.tagsBox, Akonadi::KonqBookmarkModel::Tags);
    d->mMapper->addMapping(ui_bookmarksview_base.descriptionBox, Akonadi::KonqBookmarkModel::Description, "plainText");
    d->mMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

     connect(ui_bookmarksview_base.bookmarksView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
         this, SLOT(setCurrentModelIndex(QModelIndex, QModelIndex)));
    
    d->mModelWatcher = 0;
    
    // Setup drag & drop
    ui_bookmarksview_base.bookmarksView->setDragEnabled(true);
    ui_bookmarksview_base.bookmarksView->viewport()->setAcceptDrops(true);
    ui_bookmarksview_base.bookmarksView->setDropIndicatorShown(true);
//     ui_bookmarksview_base.bookmarksView->setDragDropMode(QAbstractItemView::InternalMove);
    ui_bookmarksview_base.bookmarksView->setSortingEnabled(true);
    ui_bookmarksview_base.bookmarksView->setAnimated(true);
    ui_bookmarksview_base.bookmarksView->setFocus();
    
    ui_bookmarksview_base.collectionsView->setFocusPolicy(Qt::NoFocus);
    ui_bookmarksview_base.collectionsView->header()->hide();
    ui_bookmarksview_base.collectionsView->setSortingEnabled(false);
    ui_bookmarksview_base.collectionsView->setAnimated(true);
    ui_bookmarksview_base.collectionsView->setDragEnabled(true);
    ui_bookmarksview_base.collectionsView->viewport()->setAcceptDrops(true);
    ui_bookmarksview_base.collectionsView->setDropIndicatorShown(true);
//     ui_bookmarksview_base.collectionsView->setDragDropMode(QAbstractItemView::InternalMove);
    ui_bookmarksview_base.collectionsView->setStyleSheet("QTreeView { background: transparent; border-style: none; }");
    connect(ui_bookmarksview_base.collectionsView->model(), SIGNAL(rowsInserted(const QModelIndex&, int, int)),
            ui_bookmarksview_base.collectionsView, SLOT(expand(const QModelIndex&)));
    connect(ui_bookmarksview_base.bookmarksView->model(), SIGNAL(rowsInserted(const QModelIndex&, int, int)),
            ui_bookmarksview_base.bookmarksView, SLOT(expand(const QModelIndex&)));
    
}

void BookmarksView::setRootCollection( const Akonadi::Collection& collection)
{
    d->mBookmarkModel->setRootCollection(collection);
    
    kDebug() << d->mBookmarkModel->indexForCollection(collection).data().toString();
    ui_bookmarksview_base.navigatorBreadCrumb->setCurrentIndex(d->mBookmarkModel->indexForCollection(collection));
    
    // Set an invalid current model index
    setCurrentModelIndex(QModelIndex(), QModelIndex());
}


void BookmarksView::setCurrentModelIndex(const QModelIndex &index, const QModelIndex &/*prev*/)
{
    d->mMapper->setRootIndex(index.parent());
    d->mMapper->setCurrentModelIndex(index);
}


void BookmarksView::setRootIndex(const QModelIndex &index)
{
    Akonadi::Collection collection = index.data(EntityTreeModel::CollectionRole).value<Akonadi::Collection>();
    if(collection.isValid())
    {
        setRootCollection(collection);
    } else
        kDebug() << "Failed setting current index: " << index;
}

void BookmarksView::addBookmark()
{
    //TODO: disable newbookmark and some other actions when needed, connecting to the 
    // appropiate konqbookmarkmodel signals (which don't exist yet)
    
    QModelIndex current = ui_bookmarksview_base.bookmarksView->selectionModel()->currentIndex();
    Collection parent = getParentCollection(current);
    if(parent == Collection::root())
    {
        KMessageBox::sorry(this, i18n("Please select first a valid parent folder in which to insert the new bookmark"),
            i18n("Can't create new bookmark"));
        return;
    }
    
    KonqBookmark bookmark;
    
    Item item;
    item.setMimeType( KonqBookmark::mimeType() );
    item.setPayload<KonqBookmark>( bookmark );
    
    ItemCreateJob *job = new ItemCreateJob(item, parent);
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
    d->mBookmarkModel->removeRows(ui_bookmarksview_base.bookmarksView->currentIndex().row(), 1, ui_bookmarksview_base.bookmarksView->currentIndex().parent());
}

Akonadi::Collection BookmarksView::getParentCollection(QModelIndex current)
{
    Collection defaultCollection = d->mBookmarkModel->rootCollection();
    return getParentCollection(current, defaultCollection);
}

Akonadi::Collection BookmarksView::getParentCollection(QModelIndex current, Collection defaultCollection)
{
    Akonadi::Collection parent = defaultCollection;
    if(!current.isValid())
        return parent;
    
    QVariant var = current.data(EntityTreeModel::ItemRole);
    Akonadi::Item item = var.value<Akonadi::Item>();
    // If current selection is an item, then the parent collection for our new
    // collection will be the same parent collection as the current item. else
    // ff current selection is a collection, that will be our new collection's parent
    if ( item.isValid() )
    {
        current = current.parent();
        if(!current.isValid())
            return parent;
    }

    var = current.data(EntityTreeModel::CollectionRole);
    Akonadi::Collection collection = var.value<Akonadi::Collection>();
    if(collection.isValid())
        parent = collection;
    
    return parent;
}

void BookmarksView::slotAddFolder(const QString &folderName)
{
    // TODO see addBookmark()
    QModelIndex current = ui_bookmarksview_base.bookmarksView->selectionModel()->currentIndex();
    Collection parent = getParentCollection(current);
    if(parent == Collection::root())
    {
        KMessageBox::sorry(this, i18n("Please select first a valid parent folder in which to insert the new folder"),
            i18n("Can't create new folder"));
        return;
    }
    Akonadi::Collection collection;
    collection.setParent( parent );
    collection.setName( folderName );
    collection.setContentMimeTypes( QStringList( KonqBookmark::mimeType() ) );
    
    new Akonadi::CollectionCreateJob( collection );
}


#include "bookmarksview.moc"
