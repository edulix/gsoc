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
#include <nepomuk/ontologies/bookmark.h>
#include <nepomuk/ontologies/bookmarkfolder.h>
#include <nepomuk/ontologies/informationelement.h>
#include <nepomuk/ontologies/dataobject.h>
#include <nepomuk/result.h>
#include <Nepomuk/Resource>
#include <Nepomuk/Types/Class>
#include <Nepomuk/Tag>
#include <Nepomuk/ResourceManager>
#include <Nepomuk/Variant>
#include <Soprano/Vocabulary/Xesam>
#include <Soprano/Vocabulary/NAO>
#include <Soprano/Vocabulary/XMLSchema>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>

#include "konqbookmark.h"

class KonqBookmark::Private : public QSharedData
{
public:
    Private()
    {
        mNumVisits = 0;
        mCreated = mLastModified = mLastVisited = QDateTime::currentDateTime();
    }

    Private( const Private& other ) : QSharedData( other )
    {
        mUrl = other.mUrl;
        mTitle = other.mTitle;
        mUniqueUri = other.mUniqueUri;
        mTags = other.mTags;
        mDescription = other.mDescription;
        mNumVisits = other.mNumVisits;
        mCreated = other.mCreated;
        mLastModified = other.mLastModified;
        mLastVisited = other.mLastVisited;
    }
    
    
    Private( const QString& uniqueUri )
    {
        mUniqueUri = uniqueUri; 
        
        Nepomuk::Bookmark bookmark(mUniqueUri);
        
        if(!bookmark.bookmarkses().empty())
            mUrl = bookmark.bookmarkses().first().label();
        
        if(!bookmark.titles().empty())
            mTitle = bookmark.titles().first();
        
        mTags = QStringList();
        QList<Nepomuk::Tag> tags = bookmark.tags();
        foreach( const Nepomuk::Tag& tag, tags)
        {
            mTags.append(tag.label());
        }
        
        if(!bookmark.descriptions().empty())
            mDescription = bookmark.descriptions().first();
        
        //TODO: Store NumVisits
        mNumVisits = 0;
        
        mCreated = bookmark.contentCreated();
        
        mLastModified = bookmark.contentLastModified();
        
        if(!bookmark.informationElementDates().empty())
            mLastVisited = bookmark.informationElementDates().first();
    }

public:
    QUrl mUrl;
    QString mTitle;
    QString mUniqueUri;
    QStringList mTags;
    QString mDescription;
    long mNumVisits;
    QDateTime mCreated;
    QDateTime mLastModified;
    QDateTime mLastVisited;
};


KonqBookmark::KonqBookmark(const QString& uniqueUri) : d( new Private(uniqueUri) )
{
}

KonqBookmark::KonqBookmark(const QUrl& uniqueUri) : d( new Private(uniqueUri.toString()) )
{
}

KonqBookmark::KonqBookmark() : d( new Private )
{
    d->mUniqueUri = generateUniqueUri(); 
    
}

KonqBookmark::KonqBookmark( const KonqBookmark &other ) : d( other.d )
{
}

KonqBookmark::~KonqBookmark()
{
}

KonqBookmark &KonqBookmark::operator=( const KonqBookmark &other )
{
    if ( this != &other )
        d = other.d;

    return *this;
}

bool KonqBookmark::store()
{
    // unique uri needs to be set previously
    if(d->mUniqueUri.isEmpty())
        return false;
    Nepomuk::Bookmark bookmark(d->mUniqueUri);
    
    // Probably an error related with an ill-formed unique uri
    if(!bookmark.isValid())
        return false;
    
    /// Set the data. Note that we only set it if it really changed.
    
    // Set Bookmark
    QList<Nepomuk::DataObject> bookmarkses = bookmark.bookmarkses();
    if(d->mUrl.isEmpty() != bookmarkses.empty()
        || (!bookmarkses.empty() && bookmarkses.first().resourceUri() != d->mUrl))
    {
        Nepomuk::DataObject url(d->mUrl.toString());
        url.setLabel(d->mUrl.toString());
        bookmark.setBookmarkses( QList<Nepomuk::DataObject>() << url );
    }
    
    // Set title
    QStringList titles = bookmark.titles();
    if((d->mTitle.isEmpty() != titles.empty())
        || (!titles.empty() && titles.first() != d->mTitle))
    {
            bookmark.setTitles( QStringList(d->mTitle) );
    }
    
    // Set tags
    QList<Nepomuk::Tag> currentTags = bookmark.tags();
    QList<Nepomuk::Tag> tags;
    
    // tags: Fill current tags
    foreach(const QString& tag, d->mTags)
    {
        Nepomuk::Tag nTag(tag);
        
        nTag.setLabel(tag);
        tags.append(nTag);
    }
    
    // tags: Compare
    bool equal = currentTags.size() == d->mTags.size();
    if(equal)
    {
        int i = 0;
        foreach( const Nepomuk::Tag& tag, currentTags)
        {
            if(tag.label() != tags.at(i).label())
            {
                equal = false;
                break;
            }
            i++;
        }
    }
    // tags: Store
    if(!equal)
        bookmark.setTags(tags);
    
    
    // Set description
    QStringList descriptions = bookmark.descriptions();
    if(d->mDescription.isEmpty() != descriptions.empty()
        || (!descriptions.empty() && descriptions.first() != d->mDescription))
    {
        bookmark.setDescriptions( QStringList(d->mDescription) );
    }
    
    //TODO: Set numVisits
    
    // Set created
    if(bookmark.contentCreated() != d->mCreated)
        bookmark.setContentCreated(d->mCreated);
    
    // Set last modified
    if(bookmark.contentLastModified() != d->mLastModified)
        bookmark.setContentLastModified(d->mLastModified);
    
    // Set last visited
    QList<QDateTime> infoelDates = bookmark.informationElementDates();
    if(bookmark.informationElementDates().isEmpty()
        || d->mLastVisited != bookmark.informationElementDates().first())
        bookmark.setInformationElementDates(QList<QDateTime>() << d->mLastVisited);
    
    return true;
}

void KonqBookmark::setUrl( const QUrl &url )
{
    d->mUrl = url;
}

QUrl KonqBookmark::url() const
{
    return d->mUrl;
}

void KonqBookmark::setTitle( const QString &title )
{
    d->mTitle = title;
}

QString KonqBookmark::title() const
{
    return d->mTitle;
}

void KonqBookmark::setUniqueUri( const QString &uniqueUri )
{
    d->mUniqueUri = uniqueUri;
}

QString KonqBookmark::uniqueUri() const
{
    return d->mUniqueUri;
}

QString KonqBookmark::generateUniqueUri()
{
    // if you get two equal random strings.. go buy a lottery ticket afterwards
    return "konqbookmark:/" + KRandom::randomString( 40 ); 
}


QStringList KonqBookmark::tags() const
{
    return d->mTags;
}

void KonqBookmark::addTag(const QString& tag)
{
    if(!d->mTags.contains(tag))
        d->mTags.append(tag);
}

void KonqBookmark::removeTag(const QString& tag)
{
    d->mTags.removeOne(tag);
}

void KonqBookmark::setTags(const QStringList& tags)
{
    d->mTags = tags;
}

bool KonqBookmark::hasTag(const QString& tag) const
{
    return d->mTags.contains(tag);
}

QString KonqBookmark::description() const
{
    return d->mDescription;
}

void KonqBookmark::setDescription(const QString& description)
{
    d->mDescription = description;
}

long KonqBookmark::numVisits() const
{
    return d->mNumVisits;
}

void KonqBookmark::incrementVisits()
{
    d->mNumVisits++;
}

void KonqBookmark::setNumVisits(const long visits)
{
    d->mNumVisits = visits;
}

QDateTime KonqBookmark::created() const
{
    return d->mCreated;
}

void KonqBookmark::setCreated(const QDateTime& created)
{
    d->mCreated = created;
}

QDateTime KonqBookmark::lastModified() const
{
    return d->mLastModified;
}

void KonqBookmark::setLastModified(const QDateTime& lastModified)
{
    d->mLastModified = lastModified;
}

QDateTime KonqBookmark::lastVisited() const
{
    return d->mLastVisited;
}

void KonqBookmark::setLastVisited(const QDateTime& lastVisited)
{
    d->mLastVisited = lastVisited;
}
