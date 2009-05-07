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
