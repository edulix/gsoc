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
#include "audio.h"


#include <QtCore/QDateTime>
#include <QtCore/QDate>
#include <QtCore/QTime>


Nepomuk::Audio::Audio()
  : Media( QUrl(), QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Audio") )
{
}


Nepomuk::Audio::Audio( ResourceManager* manager )
  : Media( QUrl(), QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Audio"), manager )
{
}


Nepomuk::Audio::Audio( const Audio& res )
  : Media( res )
{
}


Nepomuk::Audio::Audio( const Nepomuk::Resource& res )
  : Media( res )
{
}


Nepomuk::Audio::Audio( const QString& uri )
  : Media( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Audio") )
{
}

Nepomuk::Audio::Audio( const QString& uri, ResourceManager* manager )
  : Media( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Audio"), manager )
{
}

Nepomuk::Audio::Audio( const QUrl& uri )
  : Media( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Audio") )
{
}

Nepomuk::Audio::Audio( const QUrl& uri, ResourceManager* manager )
  : Media( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Audio"), manager )
{
}

Nepomuk::Audio::Audio( const QString& uri, const QUrl& type )
  : Media( uri, type )
{
}

Nepomuk::Audio::Audio( const QString& uri, const QUrl& type, ResourceManager* manager )
  : Media( uri, type, manager )
{
}

Nepomuk::Audio::Audio( const QUrl& uri, const QUrl& type )
  : Media( uri, type )
{
}

Nepomuk::Audio::Audio( const QUrl& uri, const QUrl& type, ResourceManager* manager )
  : Media( uri, type, manager )
{
}

Nepomuk::Audio::~Audio()
{
}


Nepomuk::Audio& Nepomuk::Audio::operator=( const Audio& res )
{
    Resource::operator=( res );
    return *this;
}


QString Nepomuk::Audio::resourceTypeUri()
{
    return "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Audio";
}

QList<qint64> Nepomuk::Audio::channelses() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#channels") ).toInt64List());
}

void Nepomuk::Audio::setChannelses( const QList<qint64>& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#channels"), Variant( value ) );
}

void Nepomuk::Audio::addChannels( const qint64& value )
{
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#channels") );
    v.append( value );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#channels"), v );
}

QUrl Nepomuk::Audio::channelsUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#channels");
}

QList<qint64> Nepomuk::Audio::sampleCounts() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#sampleCount") ).toInt64List());
}

void Nepomuk::Audio::setSampleCounts( const QList<qint64>& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#sampleCount"), Variant( value ) );
}

void Nepomuk::Audio::addSampleCount( const qint64& value )
{
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#sampleCount") );
    v.append( value );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#sampleCount"), v );
}

QUrl Nepomuk::Audio::sampleCountUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#sampleCount");
}

QList<qint64> Nepomuk::Audio::bitsPerSamples() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#bitsPerSample") ).toInt64List());
}

void Nepomuk::Audio::setBitsPerSamples( const QList<qint64>& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#bitsPerSample"), Variant( value ) );
}

void Nepomuk::Audio::addBitsPerSample( const qint64& value )
{
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#bitsPerSample") );
    v.append( value );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#bitsPerSample"), v );
}

QUrl Nepomuk::Audio::bitsPerSampleUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#bitsPerSample");
}

QList<double> Nepomuk::Audio::sampleRates() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#sampleRate") ).toDoubleList());
}

void Nepomuk::Audio::setSampleRates( const QList<double>& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#sampleRate"), Variant( value ) );
}

void Nepomuk::Audio::addSampleRate( const double& value )
{
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#sampleRate") );
    v.append( value );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#sampleRate"), v );
}

QUrl Nepomuk::Audio::sampleRateUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#sampleRate");
}

QList<Nepomuk::Audio> Nepomuk::Audio::allAudios()
{
    return Nepomuk::convertResourceList<Audio>( ResourceManager::instance()->allResourcesOfType( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Audio") ) );
}


