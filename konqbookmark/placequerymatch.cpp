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

#include "placequerymatch.h"

using namespace Konqueror;

class PlaceQueryMatch::Private
{
public:
    Private(PlaceQueryMatch *placeQueryMatch);
    ~Private();
    
    PlaceQueryMatch *q;
    
    QVariant m_data;
    qreal m_relevance;
};


PlaceQueryMatch::Private::Private(PlaceQueryMatch *placeQueryMatch)
    : q(placeQueryMatch)
{
}

PlaceQueryMatch::Private::~Private()
{

}

PlaceQueryMatch::PlaceQueryMatch(const Place& place)
    : Place(place), d(new Private(this))
{

}

PlaceQueryMatch::~PlaceQueryMatch()
{
    delete d;
}

void PlaceQueryMatch::setRelevance(qreal relevance)
{
    d->m_relevance = relevance;
}

qreal PlaceQueryMatch::relevance() const
{
    return d->m_relevance;
}

void PlaceQueryMatch::setData(const QVariant &data)
{
    d->m_data = data;
}

QVariant PlaceQueryMatch::data() const
{
    return d->m_data;
}
