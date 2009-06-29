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
#include <QString>
#include <QList>
#include <QUrl>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <kdebug.h>

static void writeHeader( const KonqBookmark &konqBookmark, QXmlStreamWriter & writer )
{
    kDebug() << "writeHeader";
    writer.writeStartElement( QLatin1String( "header" ) );
    writer.writeAttribute( QLatin1String( "url" ), konqBookmark.url().toString() );
    writer.writeAttribute( QLatin1String( "title" ), konqBookmark.title() );
    writer.writeAttribute( QLatin1String( "uniqueUri" ), konqBookmark.uniqueUri() );
    // split tags using "," so a tag can't contain a ',' character inside!
    kDebug() << "TAGS" << konqBookmark.tags();
    kDebug() << "TAGS joined" << konqBookmark.tags().join(",");
    writer.writeAttribute( QLatin1String( "tags" ), konqBookmark.tags().join(",") );
    writer.writeAttribute( QLatin1String( "description" ), konqBookmark.description() );
    writer.writeAttribute( QLatin1String( "numVisits" ), QString::number(konqBookmark.numVisits()) );
    writer.writeAttribute( QLatin1String( "created" ), konqBookmark.created().toString() );
    writer.writeAttribute( QLatin1String( "lastModified" ), konqBookmark.lastModified().toString() );
    writer.writeAttribute( QLatin1String( "lastVisited" ), konqBookmark.lastVisited().toString() );
}

bool KonqBookmarkXmlIo::writeBookmarkToXml( const KonqBookmark &konqBookmark, QIODevice *device )
{
    kDebug();
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
    kDebug();
    while ( !reader.atEnd() ) {
        reader.readNext();

        if ( reader.isStartElement() ) {
            if ( reader.name() == QLatin1String( "header" ) ) {
                const QXmlStreamAttributes attributes = reader.attributes();
                
                QStringRef stringRef = attributes.value( QLatin1String( "url" ) );
                konqBookmark.setUrl( QUrl( stringRef.toString() ) );
                
                stringRef = attributes.value( QLatin1String( "title" ) );
                konqBookmark.setTitle( stringRef.toString() );
                
                stringRef = attributes.value( QLatin1String( "uniqueUri" ) );
                konqBookmark.setUniqueUri( stringRef.toString() );
                
                stringRef = attributes.value( QLatin1String( "tags" ) );
                
                kDebug() << "TAGS" << stringRef.toString();
                kDebug() << "TAGS joined" << stringRef.toString().split(",");
                konqBookmark.setTags( stringRef.toString().split(",") );
                kDebug() << "konqbookmark TAGS: " << konqBookmark.tags();
                
                stringRef = attributes.value( QLatin1String( "description" ) );
                konqBookmark.setDescription( stringRef.toString() );
                
                stringRef = attributes.value( QLatin1String( "numVisits" ) );
                konqBookmark.setNumVisits( stringRef.toString().toLong() );
                
                stringRef = attributes.value( QLatin1String( "created" ) );
                konqBookmark.setCreated( QDateTime::fromString(stringRef.toString()) );
                
                stringRef = attributes.value( QLatin1String( "lastModified" ) );
                konqBookmark.setLastModified( QDateTime::fromString(stringRef.toString()) );
                
                stringRef = attributes.value( QLatin1String( "lastVisited" ) );
                konqBookmark.setLastVisited( QDateTime::fromString(stringRef.toString()) );

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
    kDebug();
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
