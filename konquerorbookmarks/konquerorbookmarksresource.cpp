/*
    Copyright (c) 2009 Eduardo Robles Elvira <edulix@gmail.com>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#include "konquerorbookmarksresource.h"

#include "settings.h"
#include "settingsadaptor.h"

#include <konqbookmark/konqbookmark.h>

#include <QtDBus/QDBusConnection>
#include <QObject>

#include <nepomuk/result.h>
#include <Nepomuk/Resource>
#include <Nepomuk/Types/Class>
#include <Nepomuk/ResourceManager>
#include <Nepomuk/Variant>
#include <Soprano/Vocabulary/Xesam>
#include <Soprano/Vocabulary/NAO>
// #include <Soprano/Vocabulary/NFO>
#include <Soprano/Vocabulary/XMLSchema>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>

using namespace Akonadi;

class KonquerorBookmarksResource::Private : public QSharedData
{
public:
    Private(KonquerorBookmarksResource *parent);
    
public:
    Collection mBookmarksRootCollection;
    Collection mMenuCollection;
    Collection mToolbarCollection;
    Collection mSavedSearchesCollection;
    Collection mMostRecentCollection;
    Collection mRecentTagsCollection;
    Collection mMostVisitedCollection;
    Collection mUnclasifiedCollection;
    Collection::List mList;
    QUrl mNFOBookmark;
    QUrl mNFOBookmarkFolder;
    
    /**
     * We use this to query the nepomuk database everytime we need to fetch
     * nfo:Bookmark's belonging to a nfo:BookmarksFolder.
     */
    Nepomuk::Search::QueryServiceClient *mQueryServiceClient;

private:
    KonquerorBookmarksResource *mParent;
};

KonquerorBookmarksResource::Private::Private(KonquerorBookmarksResource *parent) :
        mNFOBookmark( QUrl::fromEncoded( "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Bookmark", QUrl::StrictMode ) ),
        mNFOBookmarkFolder( QUrl::fromEncoded( "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#BookmarkFolder", QUrl::StrictMode ) ),
        mParent( parent )
{
    mQueryServiceClient = new Nepomuk::Search::QueryServiceClient( parent );
    connect( mQueryServiceClient, SIGNAL(newEntries( const QList<Nepomuk::Search::Result>& )),
             parent, SLOT(slotNewEntries( const QList<Nepomuk::Search::Result>& )) );
}

KonquerorBookmarksResource::KonquerorBookmarksResource( const QString &id )
  : ResourceBase( id ),  d( new Private ( this ) )
{
  new SettingsAdaptor( Settings::self() );
  QDBusConnection::sessionBus().registerObject( QLatin1String( "/Settings" ),
                            Settings::self(), QDBusConnection::ExportAdaptors );


    QStringList mimeTypes;
    mimeTypes << "application/x-vnd.kde.konqbookmark" << Collection::mimeType();
    
    d->mBookmarksRootCollection.setParent( Collection::root() );
    d->mBookmarksRootCollection.setRemoteId( "/konquerorbookmarks/" );
    d->mBookmarksRootCollection.setName( "Konqueror Bookmarks" );
    d->mBookmarksRootCollection.setContentMimeTypes( mimeTypes );
    
    d->mMenuCollection.setParent( d->mBookmarksRootCollection );
    d->mMenuCollection.setRemoteId( "/konquerorbookmarks/menu/" );
    d->mMenuCollection.setName( "Bookmarks Menu" );
    d->mMenuCollection.setContentMimeTypes( mimeTypes );
    
    d->mToolbarCollection.setParent( d->mBookmarksRootCollection );
    d->mToolbarCollection.setRemoteId( "/konquerorbookmarks/toolbar/" );
    d->mToolbarCollection.setName( "Bookmarks Toolbar" );
    d->mToolbarCollection.setContentMimeTypes( mimeTypes );
    
    d->mSavedSearchesCollection.setParent( d->mMenuCollection );
    d->mSavedSearchesCollection.setRemoteId( "/konquerorbookmarks/menu/savedsearches/" );
    d->mSavedSearchesCollection.setName( "Saved Searches" );
    d->mSavedSearchesCollection.setContentMimeTypes( mimeTypes );
    
    d->mMostRecentCollection.setParent( d->mMenuCollection );
    d->mMostRecentCollection.setRemoteId( "/konquerorbookmarks/menu/mostrecent/" );
    d->mMostRecentCollection.setName( "Most Recent Bookmarks" );
    d->mMostRecentCollection.setContentMimeTypes( mimeTypes );
    
    d->mRecentTagsCollection.setParent( d->mMenuCollection );
    d->mRecentTagsCollection.setRemoteId( "/konquerorbookmarks/menu/mostrecenttags/" );
    d->mRecentTagsCollection.setName( "Most Recent Tags" );
    d->mRecentTagsCollection.setContentMimeTypes( mimeTypes );

    d->mMostVisitedCollection.setParent( d->mMenuCollection );
    d->mMostVisitedCollection.setRemoteId( "/konquerorbookmarks/menu/mostvisited/" );
    d->mMostVisitedCollection.setName( "Most Visited Bookmarks" );
    d->mMostVisitedCollection.setContentMimeTypes( mimeTypes );
    
    d->mUnclasifiedCollection.setParent( d->mMenuCollection );
    d->mUnclasifiedCollection.setRemoteId( "/konquerorbookmarks/menu/unclasified/" );
    d->mUnclasifiedCollection.setName( "Unclasified Bookmarks" );
    d->mUnclasifiedCollection.setContentMimeTypes( mimeTypes );
    
    d->mList << d->mBookmarksRootCollection;
    d->mList << d->mMenuCollection;
    d->mList << d->mToolbarCollection;
    d->mList << d->mSavedSearchesCollection;
    d->mList << d->mMostRecentCollection;
    d->mList << d->mRecentTagsCollection;
    d->mList << d->mMostVisitedCollection;
    d->mList << d->mUnclasifiedCollection;
}

KonquerorBookmarksResource::~KonquerorBookmarksResource()
{
}

void KonquerorBookmarksResource::retrieveCollections()
{
    collectionsRetrievedIncremental( d->mList, Collection::List() );
}

void KonquerorBookmarksResource::retrieveItems( const Akonadi::Collection &collection )
{
  Q_UNUSED( collection );

  // TODO: this method is called when Akonadi wants to know about all the
  // items in the given collection. You can but don't have to provide all the
  // data for each item, remote ID and MIME type are enough at this stage.
  // Depending on how your resource accesses the data, there are several
  // different ways to tell Akonadi when you are done.
}

bool KonquerorBookmarksResource::retrieveItem( const Akonadi::Item &item, const QSet<QByteArray> &parts )
{
  Q_UNUSED( item );
  Q_UNUSED( parts );

  // TODO: this method is called when Akonadi wants more data for a given item.
  // You can only provide the parts that have been requested but you are allowed
  // to provide all in one go

  return true;
}

void KonquerorBookmarksResource::aboutToQuit()
{
  // TODO: any cleanup you need to do while there is still an active
  // event loop. The resource will terminate after this method returns
}

void KonquerorBookmarksResource::configure( WId windowId )
{
  Q_UNUSED( windowId );
  
  // TODO Here we will show the "Organize bookmarks.." dialog.
}

void KonquerorBookmarksResource::itemAdded( const Akonadi::Item &item, const Akonadi::Collection &collection )
{
  Q_UNUSED( item );
  Q_UNUSED( collection );

  // TODO: this method is called when somebody else, e.g. a client application,
  // has created an item in a collection managed by your resource.

  // NOTE: There is an equivalent method for collections, but it isn't part
  // of this template code to keep it simple
}

void KonquerorBookmarksResource::itemChanged( const Akonadi::Item &item, const QSet<QByteArray> &parts )
{
  Q_UNUSED( item );
  Q_UNUSED( parts );

  // TODO: this method is called when somebody else, e.g. a client application,
  // has changed an item managed by your resource.

  // NOTE: There is an equivalent method for collections, but it isn't part
  // of this template code to keep it simple
}

void KonquerorBookmarksResource::itemRemoved( const Akonadi::Item &item )
{
  Q_UNUSED( item );

  // TODO: this method is called when somebody else, e.g. a client application,
  // has deleted an item managed by your resource.

  // NOTE: There is an equivalent method for collections, but it isn't part
  // of this template code to keep it simple
}


void KonquerorBookmarksResource::collectionAdded( const Akonadi::Collection &collection, const Akonadi::Collection &parent )
{
    //TODO
}

void KonquerorBookmarksResource::collectionChanged( const Akonadi::Collection &collection )
{
    //TODO
}

void KonquerorBookmarksResource::collectionRemoved( const Akonadi::Collection &collection )
{
    //TODO
}


void KonquerorBookmarksResource::slotNewEntries( const QList<Nepomuk::Search::Result>& results )
{
    foreach( const Nepomuk::Search::Result& result, results )
    {
        Nepomuk::Resource res( result.resourceUri() );

        if( res.hasType( d->mNFOBookmark ) )
        {
            KonqBookmark konqBookmark;
            Nepomuk::Bookmark bookmark( result.resourceUri() );
        }
    }
}

AKONADI_RESOURCE_MAIN( KonquerorBookmarksResource )

#include "konquerorbookmarksresource.moc"
