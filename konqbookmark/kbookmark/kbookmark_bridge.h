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

#ifndef KBOOKMARK_BRIDGE_H
#define KBOOKMARK_BRIDGE_H

#include "konqbookmark_export.h"

#include <QStringList>
#include <kconfig.h>
#include <kdemacros.h>

#include <kbookmark.h>
#include <kbookmarkimporter.h>
#include <kbookmarkexporter.h>

namespace Konqueror
{

/**
 * A class for importing Konqueror Bookmarks into old bookmarks model
 */
class KONQBOOKMARK_EXPORT KBookmarkImporterImpl : public KBookmarkImporterBase
{
public:
    KBookmarkImporterImpl() {}
    virtual void parse();
    virtual QString findDefaultLocation(bool = false) const;
// private:
//     class KBookmarkImporterImplPrivate *d;
};

class KONQBOOKMARK_EXPORT KBookmarkExporterImpl : public KBookmarkExporterBase
{
public:
    KBookmarkExporterImpl(KBookmarkManager* mgr, const QString & path)
      : KBookmarkExporterBase(mgr, path) {}
    virtual ~KBookmarkExporterImpl() {}
    virtual void write(const KBookmarkGroup&);
// private:
//     class KBookmarkExporterImplPrivate *d;
};

}

#endif /* KBOOKMARK_BRIDGE_H */
