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
#include "filehash.h"

#include "filedataobject.h"
#include <QtCore/QDateTime>
#include <QtCore/QDate>
#include <QtCore/QTime>


Nepomuk::FileHash::FileHash()
  : Resource( QUrl(), QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#FileHash") )
{
}


Nepomuk::FileHash::FileHash( ResourceManager* manager )
  : Resource( QUrl(), QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#FileHash"), manager )
{
}


Nepomuk::FileHash::FileHash( const FileHash& res )
  : Resource( res )
{
}


Nepomuk::FileHash::FileHash( const Nepomuk::Resource& res )
  : Resource( res )
{
}


Nepomuk::FileHash::FileHash( const QString& uri )
  : Resource( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#FileHash") )
{
}

Nepomuk::FileHash::FileHash( const QString& uri, ResourceManager* manager )
  : Resource( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#FileHash"), manager )
{
}

Nepomuk::FileHash::FileHash( const QUrl& uri )
  : Resource( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#FileHash") )
{
}

Nepomuk::FileHash::FileHash( const QUrl& uri, ResourceManager* manager )
  : Resource( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#FileHash"), manager )
{
}

Nepomuk::FileHash::FileHash( const QString& uri, const QUrl& type )
  : Resource( uri, type )
{
}

Nepomuk::FileHash::FileHash( const QString& uri, const QUrl& type, ResourceManager* manager )
  : Resource( uri, type, manager )
{
}

Nepomuk::FileHash::FileHash( const QUrl& uri, const QUrl& type )
  : Resource( uri, type )
{
}

Nepomuk::FileHash::FileHash( const QUrl& uri, const QUrl& type, ResourceManager* manager )
  : Resource( uri, type, manager )
{
}

Nepomuk::FileHash::~FileHash()
{
}


Nepomuk::FileHash& Nepomuk::FileHash::operator=( const FileHash& res )
{
    Resource::operator=( res );
    return *this;
}


QString Nepomuk::FileHash::resourceTypeUri()
{
    return "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#FileHash";
}

QStringList Nepomuk::FileHash::hValues() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hashValue") ).toStringList());
}

void Nepomuk::FileHash::setHValues( const QStringList& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hashValue"), Variant( value ) );
}

void Nepomuk::FileHash::addHValue( const QString& value )
{
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hashValue") );
    v.append( value );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hashValue"), v );
}

QUrl Nepomuk::FileHash::hValueUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hashValue");
}

QStringList Nepomuk::FileHash::hAlgorithms() const
{
    return ( property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hashAlgorithm") ).toStringList());
}

void Nepomuk::FileHash::setHAlgorithms( const QStringList& value )
{
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hashAlgorithm"), Variant( value ) );
}

void Nepomuk::FileHash::addHAlgorithm( const QString& value )
{
    Variant v = property( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hashAlgorithm") );
    v.append( value );
    setProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hashAlgorithm"), v );
}

QUrl Nepomuk::FileHash::hAlgorithmUri()
{
    return QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hashAlgorithm");
}

QList<Nepomuk::FileDataObject> Nepomuk::FileHash::hashOf() const
{
    return convertResourceList<FileDataObject>( manager()->allResourcesWithProperty( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hasHash"), *this ) );
}

QList<Nepomuk::FileHash> Nepomuk::FileHash::allFileHashs()
{
    return Nepomuk::convertResourceList<FileHash>( ResourceManager::instance()->allResourcesOfType( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#FileHash") ) );
}


