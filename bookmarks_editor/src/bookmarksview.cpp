/*
 * bookmarksview.cpp
 *
 * Copyright (C) 2008 Eduardo Robles Elvira <edulix@gmail.com>
 */
#include "bookmarksview.h"
#include "settings.h"

#include <QtGui/QLabel>
 
#include <klocale.h>
#include <akonadi/collectionfilterproxymodel.h>
#include <akonadi/collectionmodel.h>
#include <akonadi/kmime/messagemodel.h>

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
    filterModel->addMimeTypeFilter( QLatin1String( "application/x-xbel" ) );
 
    Akonadi::ItemModel *itemModel = new Akonadi::MessageModel( this );
 
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
