/*
    This file is part of the KDE project
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

#include "konqbookmarkxmlio.h"
#include "konqbookmark.h"

#include <QDateTime>
#include <QIODevice>
#include <QList>
#include <QUrl>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

static void writeHeader( const KonqBookmark &konqBookmark, QXmlStreamWriter & writer )
{
    writer.writeStartElement( QLatin1String( "header" ) );
    writer.writeAttribute( QLatin1String( "url" ), konqBookmark.url().toString() );
    writer.writeAttribute( QLatin1String( "title" ), konqBookmark.title() );
    writer.writeAttribute( QLatin1String( "uniqueUri" ), konqBookmark.uniqueUri() );
    writer.writeEndElement();
}

bool KonqBookmarkXmlIo::writeBookmarkToXml( const KonqBookmark &konqBookmark, QIODevice *device )
{
    if ( device == 0 || !device->isWritable() )
        return false;

    QXmlStreamWriter writer( device );
    writer.setAutoFormatting( true );

    writer.writeStartDocument();

    writer.writeStartElement( QLatin1String( "konqbookmark" ) );

    writeHeader( konqBookmark, writer );

    writer.writeEndElement();

    writer.writeEndDocument();

    return true;
}

static bool readHeader( QXmlStreamReader &reader, KonqBookmark &konqBookmark )
{
    while ( !reader.atEnd() ) {
        reader.readNext();

        if ( reader.isStartElement() ) {
            if ( reader.name() == QLatin1String( "header" ) ) {
                const QXmlStreamAttributes attributes = reader.attributes();
                
                const QStringRef url = attributes.value( QLatin1String( "url" ) );
                konqBookmark.setUrl( QUrl( url.toString() ) );
                
                const QStringRef title = attributes.value( QLatin1String( "title" ) );
                konqBookmark.setTitle( title.toString() );
                
                const QStringRef uniqueUri = attributes.value( QLatin1String( "uniqueUri" ) );
                konqBookmark.setUniqueUri( uniqueUri.toString() );
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}

bool KonqBookmarkXmlIo::readBookmarkFromXml( QIODevice *device, KonqBookmark &konqBookmark )
{
    if ( device == 0 || !device->isReadable() )
        return false;

    konqBookmark = KonqBookmark();

    QXmlStreamReader reader( device );

    while ( !reader.atEnd() ) {
        reader.readNext();

        if ( reader.isStartElement() ) {
            if ( reader.name() == QLatin1String( "konqbookmark" ) ) {
                if ( !readHeader( reader, konqBookmark) )
                    return false;

                break;
            } else {
                return false;
            }
        }
    }

    return true;
}
