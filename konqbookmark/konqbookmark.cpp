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

#include "konqbookmark.h"

class KonqBookmark::Private : public QSharedData
{
public:
    Private() {}

    Private( const Private& other ) : QSharedData( other )
    {
        mUrl = other.mUrl;
        mTitle = other.mTitle;
        mUniqueUri = other.mUniqueUri;
    }

public:
    QUrl mUrl;
    QString mTitle;
    QString mUniqueUri;
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
