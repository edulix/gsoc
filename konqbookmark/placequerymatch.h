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

#ifndef KONQUEROR_PLACE_QUERY_MATCH_H
#define KONQUEROR_PLACE_QUERY_MATCH_H

#include "konqbookmark_export.h"
#include "place.h"

#include <QVariant>
#include <QMetaType>

namespace Konqueror
{
    class Place;
    
    /**
     * @brief Place with information attached describing it's relevance in a query.
     * 
     * Usually created by a PlaceCompletionModel to describe a match. This
     * match has some properties like its relevance or the  query to which it's
     * related.
     */
    class KONQBOOKMARK_EXPORT PlaceQueryMatch : public Place
    {
        Q_OBJECT

    public:
        enum
        {
            /// Role to be used in models returning an PlaceQueryMAtch
            PlaceQueryMatchRole = Place::PlaceUrlRole + 1
        };
        
        PlaceQueryMatch();
        PlaceQueryMatch(const Place& place);
        virtual ~PlaceQueryMatch();
        
        /**
         * Sets the relevance of this action for the search
         * it was created for.
         *
         * @param relevance a number between 0 and 1.
         */
        void setRelevance(qreal relevance);

        /**
         * The relevance of this action to the search. By default,
         * the relevance is 0.5.
         *
         * @return a number between 0 and 1
         */
        qreal relevance() const;
        
        /**
         * Sets data to be used internally by the associated
         * AbstractRunner.
         *
         * When set, it is also used to form
         * part of the id() for this match. If that is innapropriate
         * as an id, the runner may generate its own id and set that
         * with setId(const QString&) directly after calling setData
         */
        void setData(const QVariant &data);

        /**
         * @return the data associated with this match; usually runner-specific
         */
        QVariant data() const;
        
    private:
        class Private;
        Private* const d;
    };
}

Q_DECLARE_METATYPE(Konqueror::PlaceQueryMatch)

#endif // KONQUEROR_PLACE_QUERY_MATCH_H
