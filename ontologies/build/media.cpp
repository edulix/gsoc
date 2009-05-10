/*
 * This file is part of the Nepomuk KDE project.
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

/*
 * This file has been generated by the Nepomuk Resource class generator.
 * DO NOT EDIT THIS FILE.
 * ANY CHANGES WILL BE LOST.
 */

#include <nepomuk/tools.h>
#include <nepomuk/variant.h>
#include <nepomuk/resourcemanager.h>
#include "media.h"

#include "dataobject.h"
#include "compressiontype.h"
#include <QtCore/QDateTime>
#include <QtCore/QDate>
#include <QtCore/QTime>


Nepomuk::Media::Media()
  : InformationElement( QUrl(), QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Media") )
{
}


Nepomuk::Media::Media( ResourceManager* manager )
  : InformationElement( QUrl(), QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Media"), manager )
{
}


Nepomuk::Media::Media( const Media& res )
  : InformationElement( res )
{
}


Nepomuk::Media::Media( const Nepomuk::Resource& res )
  : InformationElement( res )
{
}


Nepomuk::Media::Media( const QString& uri )
  : InformationElement( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Media") )
{
}

Nepomuk::Media::Media( const QString& uri, ResourceManager* manager )
  : InformationElement( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Media"), manager )
{
}

Nepomuk::Media::Media( const QUrl& uri )
  : InformationElement( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Media") )
{
}

Nepomuk::Media::Media( const QUrl& uri, ResourceManager* manager )
  : InformationElement( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Media"), manager )
{
}

Nepomuk::Media::Media( const QString& uri, const QUrl& type )
  : InformationElement( uri, type )
{
}

Nepomuk::Media::Media( const QString& uri, const QUrl& type, ResourceManager* manager )
  : InformationElement( uri, type, manager )
{
}

Nepomuk::Media::Media( const QUrl& uri, const QUrl& type )
  : InformationElement( uri, type )
{
}

Nepomuk::Media::Media( const QUrl& uri, const QUrl& type, ResourceManager* manager )
  : InformationElement( uri, type, manager )
{
}

Nepomuk::Media::~Media()
{
}


Nepomuk::Media& Nepomuk::Media::operator=( const Media& res )
{
    Resource::operator=( res );
    return *this;
}


QString Nepomuk::Media::resourceTypeUri()
{
    return "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Media";
}

QList<QDateTime> Nepomuk::Media::durations() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#duration") ).toDateTimeList());
}

void Nepomuk::Media::setDurations( const QList<QDateTime>& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#duration"), Variant( value ) );
}

void Nepomuk::Media::addDuration( const QDateTime& value )
{
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#duration") );
    v.append( value );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#duration"), v );
}

QUrl Nepomuk::Media::durationUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#duration");
}

QList<qint64> Nepomuk::Media::counts() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#count") ).toInt64List());
}

void Nepomuk::Media::setCounts( const QList<qint64>& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#count"), Variant( value ) );
}

void Nepomuk::Media::addCount( const qint64& value )
{
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#count") );
    v.append( value );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#count"), v );
}

QUrl Nepomuk::Media::countUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#count");
}

QList<Nepomuk::CompressionType> Nepomuk::Media::compressionTypes() const
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    return convertResourceList<CompressionType>( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#compressionType") ).toResourceList() );
}

void Nepomuk::Media::setCompressionTypes( const QList<Nepomuk::CompressionType>& value )
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    QList<Resource> l;
    for( QList<CompressionType>::const_iterator it = value.constBegin();
         it != value.constEnd(); ++it ) {
        l.append( Resource( (*it) ) );
    }
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#compressionType"), Variant( l ) );
}

void Nepomuk::Media::addCompressionType( const Nepomuk::CompressionType& value )
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#compressionType") );
    v.append( Resource( value ) );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#compressionType"), v );
}

QUrl Nepomuk::Media::compressionTypeUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#compressionType");
}

QList<Nepomuk::DataObject> Nepomuk::Media::mediaStreams() const
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    return convertResourceList<DataObject>( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hasMediaStream") ).toResourceList() );
}

void Nepomuk::Media::setMediaStreams( const QList<Nepomuk::DataObject>& value )
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    QList<Resource> l;
    for( QList<DataObject>::const_iterator it = value.constBegin();
         it != value.constEnd(); ++it ) {
        l.append( Resource( (*it) ) );
    }
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hasMediaStream"), Variant( l ) );
}

void Nepomuk::Media::addMediaStream( const Nepomuk::DataObject& value )
{
    // We always store all Resource types as plain Resource objects.
    // It does not introduce any overhead (due to the implicit sharing of
    // the data and has the advantage that we can mix setProperty calls
    // with the special Resource subclass methods.
    // More importantly Resource loads the data as Resource objects anyway.
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hasMediaStream") );
    v.append( Resource( value ) );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hasMediaStream"), v );
}

QUrl Nepomuk::Media::mediaStreamUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hasMediaStream");
}

QStringList Nepomuk::Media::bitDepths() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#bitDepth") ).toStringList());
}

void Nepomuk::Media::setBitDepths( const QStringList& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#bitDepth"), Variant( value ) );
}

void Nepomuk::Media::addBitDepth( const QString& value )
{
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#bitDepth") );
    v.append( value );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#bitDepth"), v );
}

QUrl Nepomuk::Media::bitDepthUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#bitDepth");
}

QList<double> Nepomuk::Media::rates() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#rate") ).toDoubleList());
}

void Nepomuk::Media::setRates( const QList<double>& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#rate"), Variant( value ) );
}

void Nepomuk::Media::addRate( const double& value )
{
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#rate") );
    v.append( value );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#rate"), v );
}

QUrl Nepomuk::Media::rateUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#rate");
}

QStringList Nepomuk::Media::codecs() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#codec") ).toStringList());
}

void Nepomuk::Media::setCodecs( const QStringList& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#codec"), Variant( value ) );
}

void Nepomuk::Media::addCodec( const QString& value )
{
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#codec") );
    v.append( value );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#codec"), v );
}

QUrl Nepomuk::Media::codecUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#codec");
}

QStringList Nepomuk::Media::bitrateTypes() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#bitrateType") ).toStringList());
}

void Nepomuk::Media::setBitrateTypes( const QStringList& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#bitrateType"), Variant( value ) );
}

void Nepomuk::Media::addBitrateType( const QString& value )
{
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#bitrateType") );
    v.append( value );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#bitrateType"), v );
}

QUrl Nepomuk::Media::bitrateTypeUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#bitrateType");
}

QList<double> Nepomuk::Media::averageBitrates() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#averageBitrate") ).toDoubleList());
}

void Nepomuk::Media::setAverageBitrates( const QList<double>& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#averageBitrate"), Variant( value ) );
}

void Nepomuk::Media::addAverageBitrate( const double& value )
{
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#averageBitrate") );
    v.append( value );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#averageBitrate"), v );
}

QUrl Nepomuk::Media::averageBitrateUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#averageBitrate");
}

QList<Nepomuk::Media> Nepomuk::Media::allMedias()
{
    return Nepomuk::convertResourceList<Media>( ResourceManager::instance()->allResourcesOfType( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Media") ) );
}


