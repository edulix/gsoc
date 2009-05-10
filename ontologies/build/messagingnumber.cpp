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
#include "messagingnumber.h"


#include <QtCore/QDateTime>
#include <QtCore/QDate>
#include <QtCore/QTime>


Nepomuk::MessagingNumber::MessagingNumber()
  : PhoneNumber( QUrl(), QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#MessagingNumber") )
{
}


Nepomuk::MessagingNumber::MessagingNumber( ResourceManager* manager )
  : PhoneNumber( QUrl(), QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#MessagingNumber"), manager )
{
}


Nepomuk::MessagingNumber::MessagingNumber( const MessagingNumber& res )
  : PhoneNumber( res )
{
}


Nepomuk::MessagingNumber::MessagingNumber( const Nepomuk::Resource& res )
  : PhoneNumber( res )
{
}


Nepomuk::MessagingNumber::MessagingNumber( const QString& uri )
  : PhoneNumber( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#MessagingNumber") )
{
}

Nepomuk::MessagingNumber::MessagingNumber( const QString& uri, ResourceManager* manager )
  : PhoneNumber( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#MessagingNumber"), manager )
{
}

Nepomuk::MessagingNumber::MessagingNumber( const QUrl& uri )
  : PhoneNumber( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#MessagingNumber") )
{
}

Nepomuk::MessagingNumber::MessagingNumber( const QUrl& uri, ResourceManager* manager )
  : PhoneNumber( uri, QUrl::fromEncoded("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#MessagingNumber"), manager )
{
}

Nepomuk::MessagingNumber::MessagingNumber( const QString& uri, const QUrl& type )
  : PhoneNumber( uri, type )
{
}

Nepomuk::MessagingNumber::MessagingNumber( const QString& uri, const QUrl& type, ResourceManager* manager )
  : PhoneNumber( uri, type, manager )
{
}

Nepomuk::MessagingNumber::MessagingNumber( const QUrl& uri, const QUrl& type )
  : PhoneNumber( uri, type )
{
}

Nepomuk::MessagingNumber::MessagingNumber( const QUrl& uri, const QUrl& type, ResourceManager* manager )
  : PhoneNumber( uri, type, manager )
{
}

Nepomuk::MessagingNumber::~MessagingNumber()
{
}


Nepomuk::MessagingNumber& Nepomuk::MessagingNumber::operator=( const MessagingNumber& res )
{
    Resource::operator=( res );
    return *this;
}


QString Nepomuk::MessagingNumber::resourceTypeUri()
{
    return "http://www.semanticdesktop.org/ontologies/2007/03/22/nco#MessagingNumber";
}

QList<Nepomuk::MessagingNumber> Nepomuk::MessagingNumber::allMessagingNumbers()
{
    return Nepomuk::convertResourceList<MessagingNumber>( ResourceManager::instance()->allResourcesOfType( QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#MessagingNumber") ) );
}


