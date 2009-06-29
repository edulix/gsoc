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

#ifndef KONQBOOKMARK_H
#define KONQBOOKMARK_H

#include "konqbookmark_export.h"

#include <QtCore/QSharedDataPointer>
#include <QMetaType>
#include <QDateTime>
#include <QStringList>
#include <QLatin1String>

class QDateTime;
class QString;
class QUrl;

template <class T> class QList;


/**
 *  @short KonqBookmark stores information about a konqueror bookmark.
 *  
 *  It's used for:
 * - move the information about a bookmark from akonadi server to akonadi 
 *   clients (thanks to the XmlIO serilization/deserialization utilities)
 * - Simply the storage, modification and retrieval of konqueror bookmarks in
 *   Nepomuk.
 *   
 *  @author Eduardo Robles Elvira <edulix@gmail.com>
 */
class KONQBOOKMARK_EXPORT KonqBookmark
{
public:
    /**
     * Default constructor.
     */
    KonqBookmark();

    /**
     * Given the URI of the bookmark, reads the bookmark related data from the
     * backend (Nepomuk).
     */
    KonqBookmark(const QString& uniqueUri);
    
    
    /**
     * Given the URI of the bookmark, reads the bookmark related data from the
     * backend (Nepomuk).
     * @overload
     */
    KonqBookmark(const QUrl& uniqueUri);
    
    /**
     * Copy constructor.
     * @overload
     */
    KonqBookmark( const KonqBookmark &other );

    /**
     * Destructor.
     */
    ~KonqBookmark();

    KonqBookmark &operator=( const KonqBookmark &other );
    
    /**
     * Stores the bookmark into the backend. At the moment, this means storing
     * it into Nepomuk, in the future we might add support for storing bookmarks
     * in other backends.
     * @returns if the storing procedure was sucessful
     */
    bool store();

// Properties
    
    void setUrl( const QUrl &url );

    QUrl url() const;

    void setTitle( const QString &title );

    QString title() const;
    
    QString uniqueUri() const;
    
    void setUniqueUri(const QString &uniqueUri);
    
    static QString generateUniqueUri();

    QStringList tags() const;
    
    void addTag(const QString& tag);
    
    void removeTag(const QString& tag);
    
    void setTags(const QStringList& tags);
    
    bool hasTag(const QString& tag) const;
    
    QString description() const;
    
    void setDescription(const QString& description);
    
    long numVisits() const;
    
    void incrementVisits();
    
    void setNumVisits(const long visits);
    
    QDateTime created() const;
    
    void setCreated(const QDateTime& created);
    
    QDateTime lastModified() const;
    
    void setLastModified(const QDateTime& lastModified);
    
    QDateTime lastVisited() const;
    
    void setLastVisited(const QDateTime& lastVisited);
    
    static QString mimeType()
    {
        return QLatin1String("application/x-vnd.kde.konqbookmark");
    };
    
private:
    class Private;
    QSharedDataPointer<Private> d;
};

Q_DECLARE_METATYPE(KonqBookmark);

#endif /* KONQBOOKMARK_H */
