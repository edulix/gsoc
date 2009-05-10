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

class QDateTime;
class QString;
class QUrl;

template <class T> class QList;

class KONQBOOKMARK_EXPORT KonqBookmark
{
public:

  KonqBookmark();

  KonqBookmark( const KonqBookmark &other );

  ~KonqBookmark();

  KonqBookmark &operator=( const KonqBookmark &other );

  void setUrl( const QUrl &url );

  QUrl url() const;

  void setTitle( const QString &title );

  QString title() const;

private:
  class Private;
  QSharedDataPointer<Private> d;
};

#endif /* KONQBOOKMARK_H */
