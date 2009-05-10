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
