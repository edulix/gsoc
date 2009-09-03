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

#include <QDateTime>
#include <QList>
#include <QString>
#include <QStringList>
#include <QUrl>

#include <krandom.h>
#include <kdebug.h>
#include <nepomuk/ontologies/bookmarkfolder.h>
#include <nepomuk/ontologies/informationelement.h>
#include <nepomuk/ontologies/dataobject.h>
#include <nepomuk/result.h>
#include <Nepomuk/Resource>
#include <Nepomuk/Types/Class>
#include <Nepomuk/Tag>
#include <Nepomuk/ResourceManager>
#include <Nepomuk/Variant>
#include <Soprano/Vocabulary/NAO>
#include "konqbookmark.h"

class KonqBookmark::Private : public QSharedData
{
public:
    Private()
        : m_bookmark(KonqBookmark::generateUniqueUri())
    {
    }

    Private( const Private& other )
        : QSharedData( other ), m_bookmark(other.m_bookmark)
    {
    }
    
    
    Private( const QString& uniqueUri )
        : m_bookmark(uniqueUri)
    {
    }

public:
    Nepomuk::Bookmark m_bookmark;
};


KonqBookmark::KonqBookmark(const QString& uniqueUri) : d( new Private(uniqueUri) )
{
}

KonqBookmark::KonqBookmark(const QUrl& uniqueUri) : d( new Private(uniqueUri.toString()) )
{
}

KonqBookmark::KonqBookmark() : d(new Private)
{   
}

KonqBookmark::KonqBookmark(const KonqBookmark &other) : d(other.d)
{
}

KonqBookmark::~KonqBookmark()
{
}

KonqBookmark &KonqBookmark::operator=(const KonqBookmark &other)
{
    if (this != &other) {
        d = other.d;
    }

    return *this;
}

bool KonqBookmark::operator==(const KonqBookmark &other)
{
    return other.uniqueUri() == uniqueUri();
}

Nepomuk::Bookmark KonqBookmark::bookmark() const
{
    return d->m_bookmark;
}


void KonqBookmark::setUrl( const QUrl &url )
{
    Nepomuk::DataObject dataObject(url);
    dataObject.setLabel(url.toString());
    d->m_bookmark.setBookmarkses( QList<Nepomuk::DataObject>() << dataObject );
    
}

QUrl KonqBookmark::url() const
{
    if(!d->m_bookmark.bookmarkses().empty())
        return d->m_bookmark.bookmarkses().first().label();
    
    return QUrl();
}

void KonqBookmark::setTitle( const QString &title )
{
    d->m_bookmark.setTitles( QStringList(title) );
}

QString KonqBookmark::title() const
{
    if(!d->m_bookmark.titles().empty())
        return d->m_bookmark.titles().first();
    
    return QString();
}

void KonqBookmark::setUniqueUri( const QString &uniqueUri )
{
//     delete d;
    d = new Private(uniqueUri);
}

QString KonqBookmark::uniqueUri() const
{
    return d->m_bookmark.resourceUri().toString();
}

QString KonqBookmark::generateUniqueUri()
{
    // if you get two equal random strings.. go buy a lottery ticket afterwards
    return "konqbookmark:/" + KRandom::randomString( 40 ); 
}


QStringList KonqBookmark::tags() const
{
    QStringList tagStrings;
    QList<Nepomuk::Tag> tags = d->m_bookmark.tags();
    foreach( const Nepomuk::Tag& tag, tags)
    {
        tagStrings.append(tag.label());
    }
    return tagStrings;
}

void KonqBookmark::addTag(const QString& tag)
{
    Nepomuk::Tag nTag(tag);
    d->m_bookmark.addTag(nTag);
}

void KonqBookmark::removeTag(const QString& tag)
{
    QList<Nepomuk::Tag> tags = d->m_bookmark.tags();
    Nepomuk::Tag nTag(tag);
    if(tags.removeOne(tag))
        d->m_bookmark.setTags(tags);
}

void KonqBookmark::setTags(const QStringList& tags)
{
    QList<Nepomuk::Tag> currentTags = d->m_bookmark.tags();
    QList<Nepomuk::Tag> newTags;
    
    foreach( const QString& tag, tags)
    {
        Nepomuk::Tag newTag(tag);
        newTag.setLabel(tag);
        newTags.append(newTag);
    }
    d->m_bookmark.setTags(newTags);    
}

bool KonqBookmark::hasTag(const QString& tag) const
{
    QList<Nepomuk::Tag> tags = d->m_bookmark.tags();
    Nepomuk::Tag nTag(tag);
    return tags.contains(tag);
}

QString KonqBookmark::description() const
{
    if(!d->m_bookmark.descriptions().empty())
        return d->m_bookmark.descriptions().first();
    
    return QString();
}

void KonqBookmark::setDescription(const QString& description)
{
    d->m_bookmark.setDescriptions( QStringList(description) );
}

long KonqBookmark::numVisits() const
{
    // TODO
    return 0;
}

void KonqBookmark::incrementVisits()
{
    // TODO
}

void KonqBookmark::setNumVisits(const long visits)
{
    Q_UNUSED(visits);
    // TODO
}

QDateTime KonqBookmark::created() const
{
    return d->m_bookmark.property(Soprano::Vocabulary::NAO::created().toString()).toDateTime();
}

void KonqBookmark::setCreated(const QDateTime& created)
{
    d->m_bookmark.setProperty(Soprano::Vocabulary::NAO::created().toString(), created);
}

QDateTime KonqBookmark::lastModified() const
{
    return d->m_bookmark.contentLastModified();
}

void KonqBookmark::setLastModified(const QDateTime& lastModified)
{
    d->m_bookmark.setContentCreated(lastModified);
}

QDateTime KonqBookmark::lastVisited() const
{
    if(!d->m_bookmark.informationElementDates().empty())
        return d->m_bookmark.informationElementDates().first();
    
    return QDateTime();
}

void KonqBookmark::setLastVisited(const QDateTime& lastVisited)
{
    d->m_bookmark.setInformationElementDates(QList<QDateTime>() << lastVisited);
}
