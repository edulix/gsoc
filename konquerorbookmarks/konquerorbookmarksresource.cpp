#include "konquerorbookmarksresource.h"

#include "settings.h"
#include "settingsadaptor.h"

#include <QtDBus/QDBusConnection>

using namespace Akonadi;


class KonquerorBookmarksResource::Private : public QSharedData
{
public:
    Private() {}

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
};

KonquerorBookmarksResource::KonquerorBookmarksResource( const QString &id )
  : ResourceBase( id ),  d( new Private )
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
    collectionsRetrieved( d->mList );
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

AKONADI_RESOURCE_MAIN( KonquerorBookmarksResource )

#include "konquerorbookmarksresource.moc"
