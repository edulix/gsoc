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
#include "mindmap.h"


#include <QtCore/QDateTime>
#include <QtCore/QDate>
#include <QtCore/QTime>


Nepomuk::MindMap::MindMap()
  : Document( QUrl(), QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#MindMap") )
{
}


Nepomuk::MindMap::MindMap( ResourceManager* manager )
  : Document( QUrl(), QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#MindMap"), manager )
{
}


Nepomuk::MindMap::MindMap( const MindMap& res )
  : Document( res )
{
}


Nepomuk::MindMap::MindMap( const Nepomuk::Resource& res )
  : Document( res )
{
}


Nepomuk::MindMap::MindMap( const QString& uri )
  : Document( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#MindMap") )
{
}

Nepomuk::MindMap::MindMap( const QString& uri, ResourceManager* manager )
  : Document( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#MindMap"), manager )
{
}

Nepomuk::MindMap::MindMap( const QUrl& uri )
  : Document( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#MindMap") )
{
}

Nepomuk::MindMap::MindMap( const QUrl& uri, ResourceManager* manager )
  : Document( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#MindMap"), manager )
{
}

Nepomuk::MindMap::MindMap( const QString& uri, const QUrl& type )
  : Document( uri, type )
{
}

Nepomuk::MindMap::MindMap( const QString& uri, const QUrl& type, ResourceManager* manager )
  : Document( uri, type, manager )
{
}

Nepomuk::MindMap::MindMap( const QUrl& uri, const QUrl& type )
  : Document( uri, type )
{
}

Nepomuk::MindMap::MindMap( const QUrl& uri, const QUrl& type, ResourceManager* manager )
  : Document( uri, type, manager )
{
}

Nepomuk::MindMap::~MindMap()
{
}


Nepomuk::MindMap& Nepomuk::MindMap::operator=( const MindMap& res )
{
    Resource::operator=( res );
    return *this;
}


QString Nepomuk::MindMap::resourceTypeUri()
{
    return "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#MindMap";
}

QList<Nepomuk::MindMap> Nepomuk::MindMap::allMindMaps()
{
    return Nepomuk::convertResourceList<MindMap>( ResourceManager::instance()->allResourcesOfType( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#MindMap") ) );
}


