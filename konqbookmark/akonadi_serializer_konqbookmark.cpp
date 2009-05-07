#include "akonadi_serializer_konqbookmark.h"
 
#include "konqbookmark.h"
#include "konqbookmarkxmlio.h"
 
#include <akonadi/item.h>

#include <QtCore/qplugin.h>

using namespace Akonadi;

bool SerializerPluginKonqBookmark::deserialize( Item& item, const QByteArray& label, QIODevice& data, int version )
{
  Q_UNUSED( version );
 
  if ( label != Item::FullPayload )
    return false;
 
  KonqBookmark konqBookmark;
  if ( !KonqBookmarkXmlIo::readBookmarkFromXml( &data, konqBookmark ) )
    return false;
 
  item.setPayload<KonqBookmark>( konqBookmark );
 
  return true;
}

void SerializerPluginKonqBookmark::serialize( const Item& item, const QByteArray& label, QIODevice& data, int &version )
{
    Q_UNUSED( version );
    
    if ( label != Item::FullPayload || !item.hasPayload<KonqBookmark>() )
        return;
    
    const KonqBookmark konqBookmark = item.payload<KonqBookmark>();
    
    KonqBookmarkXmlIo::writeBookmarkToXml( konqBookmark, &data );
}

QSet<QByteArray> SerializerPluginKonqBookmark::parts( const Item &item ) const
{
    // only need to reimplement this when implementing partial serialization
    // i.e. when using the "label" parameter of the other two methods
    return ItemSerializerPlugin::parts( item );
}

Q_EXPORT_PLUGIN2( akonadi_serializer_konqbookmark, Akonadi::SerializerPluginKonqBookmark )

#include "akonadi_serializer_konqbookmark.moc"
