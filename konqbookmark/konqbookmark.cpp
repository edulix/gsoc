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

#include "konqbookmark.h"

class KonqBookmark::Private : public QSharedData
{
public:
    Private() {}

    Private( const Private& other ) : QSharedData( other )
    {
        mUrl = other.mUrl;
        mTitle = other.mTitle;
    }

public:
    QUrl mUrl;
    QString mTitle;
};

KonqBookmark::KonqBookmark() : d( new Private )
{
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
