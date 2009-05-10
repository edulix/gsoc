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
#include "image.h"


#include <QtCore/QDateTime>
#include <QtCore/QDate>
#include <QtCore/QTime>


Nepomuk::Image::Image()
  : Visual( QUrl(), QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Image") )
{
}


Nepomuk::Image::Image( ResourceManager* manager )
  : Visual( QUrl(), QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Image"), manager )
{
}


Nepomuk::Image::Image( const Image& res )
  : Visual( res )
{
}


Nepomuk::Image::Image( const Nepomuk::Resource& res )
  : Visual( res )
{
}


Nepomuk::Image::Image( const QString& uri )
  : Visual( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Image") )
{
}

Nepomuk::Image::Image( const QString& uri, ResourceManager* manager )
  : Visual( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Image"), manager )
{
}

Nepomuk::Image::Image( const QUrl& uri )
  : Visual( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Image") )
{
}

Nepomuk::Image::Image( const QUrl& uri, ResourceManager* manager )
  : Visual( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Image"), manager )
{
}

Nepomuk::Image::Image( const QString& uri, const QUrl& type )
  : Visual( uri, type )
{
}

Nepomuk::Image::Image( const QString& uri, const QUrl& type, ResourceManager* manager )
  : Visual( uri, type, manager )
{
}

Nepomuk::Image::Image( const QUrl& uri, const QUrl& type )
  : Visual( uri, type )
{
}

Nepomuk::Image::Image( const QUrl& uri, const QUrl& type, ResourceManager* manager )
  : Visual( uri, type, manager )
{
}

Nepomuk::Image::~Image()
{
}


Nepomuk::Image& Nepomuk::Image::operator=( const Image& res )
{
    Resource::operator=( res );
    return *this;
}


QString Nepomuk::Image::resourceTypeUri()
{
    return "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Image";
}

QList<qint64> Nepomuk::Image::verticalResolutions() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#verticalResolution") ).toInt64List());
}

void Nepomuk::Image::setVerticalResolutions( const QList<qint64>& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#verticalResolution"), Variant( value ) );
}

void Nepomuk::Image::addVerticalResolution( const qint64& value )
{
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#verticalResolution") );
    v.append( value );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#verticalResolution"), v );
}

QUrl Nepomuk::Image::verticalResolutionUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#verticalResolution");
}

QList<qint64> Nepomuk::Image::horizontalResolutions() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#horizontalResolution") ).toInt64List());
}

void Nepomuk::Image::setHorizontalResolutions( const QList<qint64>& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#horizontalResolution"), Variant( value ) );
}

void Nepomuk::Image::addHorizontalResolution( const qint64& value )
{
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#horizontalResolution") );
    v.append( value );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#horizontalResolution"), v );
}

QUrl Nepomuk::Image::horizontalResolutionUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#horizontalResolution");
}

QList<Nepomuk::Image> Nepomuk::Image::allImages()
{
    return Nepomuk::convertResourceList<Image>( ResourceManager::instance()->allResourcesOfType( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Image") ) );
}


