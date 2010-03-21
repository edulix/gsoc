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

#ifndef KONQUEROR_PLACE_H
#define KONQUEROR_PLACE_H

#include "konqbookmark_export.h"
#include "placesmanager.h"
#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QString>
#include <QtGui/QIcon>
#include <QtCore/QStringList>
#include <QtCore/QDateTime>

#include <Akonadi/Item>

class KonqHistoryEntry;
class KonqBookmark;

namespace Konqueror
{
    class KONQBOOKMARK_EXPORT Place : public QObject
    {
        Q_OBJECT
        friend class PlacesManager;
    protected:
        /**
         * Creates a Konqueror Place object given an history entry and a
         * konqueror bookmark.  A Place is used to refer to an URI that the user
         * visited that might or might not have some additional data coming from
         * a bookmark (like the tags, etc) or maybe the history entry.
         *
         * The data sources for this place will be both the given history entry
         * and the related KonqBookmark for the url of that history entry if
         * any.
         *
         * This constructor can only will be called by a PlacesManager which
         * will create a new Place given a bookmark, an history entry, or an
         * url. Example:
         *
         * @code
         * Place *place = PlacesManager::place("http://google.es");
         * @endcode
         *
         * @note At least one of the two items history entry or konqueror
         * bookmark) should  not be null. Note also that this Place won't take
         * ownership of those items so it's up to you to remove them when needed.
         * Note that even if the user visited this place at least once, we might
         * not have a KonqHistoryEntry to refer to it because it might have
         * already expired.
         */
        Place(KonqHistoryEntry *historyEntry = 0, KonqBookmark *konqBookmark = 0, QObject *parent = 0);

        /**
         * @brief Creates a place with no history entry o konqBookmark attached.
         *
         * Useful if you need to create a place for a match in a PlaceCompletionModel
         * where the place has not been visited or bookmarked.
         */
        Place(const QUrl &url, QObject *parent = 0);

        void setHistoryEntry(KonqHistoryEntry *historyEntry);

        void setBookmark(KonqBookmark *konqBookmark);

    public:
        /// To be used in Place related models
        enum
        {
            PlaceUrlRole = Qt::UserRole + 50,
            PlaceRelevanceRole
        };
        /**
         * Destructor.
         */
        virtual ~Place();

        /**
         * Copy constructor.
         * @overload
         */
        Place(const Place &other);

        /**
         * Copy operator, same as copy contructor.
         */
        const Place& operator=(const Place &other);

        /**
         * Compares if the url of both places is the same.
         */
        bool operator==(const Place &other) const;

        /**
         * @returns the title of the place.
         */
        QString title() const;

        /**
         * @returns the url of the place.
         */
        QUrl url() const;

        /**
         * Use PlacesManager::icon(place) instead.
         */
//         QIcon icon() const;

        QString description() const;

        QStringList tags() const;

        long numVisits() const;

        QDateTime created() const;

        QDateTime lastVisited() const;

        KonqBookmark *bookmark() const;

        KonqHistoryEntry *historyEntry() const;

        Akonadi::Item item() const;
        void setItem(const Akonadi::Item& item);

    Q_SIGNALS:
        /**
         * This signal is fired when this place is updated by calling to a setter function of
         * this class.
         */
        void updated();

    private:
        class Private;
        Private *const d;
    };
}

#endif // KONQUEROR_PLACE_H
