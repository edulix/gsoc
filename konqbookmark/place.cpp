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

#include "place.h"
#include "konqbookmark.h"
#include <konq_historyentry.h> // libkonq
#include <Nepomuk/Resource>
#include <Nepomuk/Tag>

using namespace Konqueror;

/**
 * Private class that helps to provide binary compatibility between releases.
 * @internal
 */
//@cond PRIVATE
class Place::Private
{
public:
    Private(Place *parent);
    ~Private();

    Place *q;

    KonqHistoryEntry *m_historyEntry;
    KonqBookmark *m_konqBookmark;
    QUrl m_url;
    Akonadi::Item m_item;
};

Place::Private::Private(Place *parent)
    : q(parent), m_historyEntry(0), m_konqBookmark(0)
{

}

Place::Private::~Private()
{
}
//@endcond

Place::Place(KonqHistoryEntry *historyEntry, KonqBookmark *konqBookmark, QObject *parent)
    : QObject(parent), d(new Private(this))
{
    d->m_historyEntry = historyEntry;
    d->m_konqBookmark = konqBookmark;
}

Place::Place(const QUrl &url, QObject *parent)
    : QObject(parent), d(new Private(this))
{
    d->m_url = url;
}

Place::~Place()
{
    delete d;
}

Place::Place(const Place &other)
    : QObject(other.parent()), d(new Private(this))
{
    operator=(other);
}

const Place& Place::operator=(const Place &other)
{
    d->m_historyEntry = other.historyEntry();
    d->m_konqBookmark = other.bookmark();
    return other;
}

bool Place::operator==(const Place &other) const
{
    return other.url() == url();
}

void Place::setHistoryEntry(KonqHistoryEntry *historyEntry)
{
    d->m_historyEntry = historyEntry;
    emit updated();
}

void Place::setBookmark(KonqBookmark *konqBookmark)
{
    d->m_konqBookmark = konqBookmark;
    emit updated();
}

QString Place::title() const
{
    if (d->m_historyEntry) {
        return d->m_historyEntry->title;
    }

    if (d->m_konqBookmark) {
        return d->m_konqBookmark->title();
    }

    if (!d->m_url.isEmpty() && d->m_url.isValid()) {
        KUrl url(d->m_url.path());
        return url.fileName();
    }

    return QString();
}

QUrl Place::url() const
{
    if (d->m_historyEntry) {
        return d->m_historyEntry->url;
    }

    if (d->m_konqBookmark) {
        return d->m_konqBookmark->url();
    }

    if (!d->m_url.isEmpty() && d->m_url.isValid()) {
        return d->m_url;
    }

    return QUrl();
}

bool Place::isCompletedItem() const
{
    return !d->m_historyEntry && !d->m_konqBookmark;
}


QString Place::description() const
{
    if (d->m_konqBookmark) {
        return d->m_konqBookmark->description();
    }

    return QString();
}

QStringList Place::tags() const
{
    if (d->m_konqBookmark) {
        return d->m_konqBookmark->tags();
    }

    return QStringList();
}

long Place::numVisits() const
{
   if (d->m_historyEntry) {
        return d->m_historyEntry->numberOfTimesVisited;
   }

   if (d->m_konqBookmark) {
        return d->m_konqBookmark->numVisits();
   }

    return 0;
}

QDateTime Place::created() const
{
   if (d->m_historyEntry) {
        return d->m_historyEntry->firstVisited;
   }

   if(d->m_konqBookmark) {
        return d->m_konqBookmark->created();
   }

    return QDateTime();
}

QDateTime Place::lastVisited() const
{
   if (d->m_historyEntry) {
        return d->m_historyEntry->lastVisited;
   }

   if (d->m_konqBookmark) {
        return d->m_konqBookmark->lastVisited();
   }

    return QDateTime();
}

KonqBookmark *Place::bookmark() const
{
    return d->m_konqBookmark;
}

KonqHistoryEntry *Place::historyEntry() const
{
    return d->m_historyEntry;
}

Akonadi::Item Place::item() const
{
    return d->m_item;
}

void Place::setItem(const Akonadi::Item &item)
{
    d->m_item = item;
}


#include "place.moc"

