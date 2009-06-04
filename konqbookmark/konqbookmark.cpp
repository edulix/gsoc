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
#include <QUrl>
#include <krandom.h>
#include <kdebug.h>

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


KonqBookmark::KonqBookmark(const QString& uniqueUri) : d( new Private )
{
    d->mUniqueUri = uniqueUri; 
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
