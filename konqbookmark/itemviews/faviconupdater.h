/*
    Copyright (C) 2003 Alexander Kellett <lypanov@kde.org>
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

#ifndef FAVICONUPDATER_H
#define FAVICONUPDATER_H

#include "konqbookmark_export.h"
#include "favicon_interface.h"

#include <kparts/part.h>
#include <kparts/browserinterface.h>
#include <kurl.h>

class FavIconUpdater : public QObject
{
    Q_OBJECT

public:
    FavIconUpdater(QObject *parent);
    virtual ~FavIconUpdater();
    void downloadIcon(const KUrl &url);
    void downloadIconActual(const KUrl &url);

Q_SIGNALS:
    void done(bool succeeded, KUrl iconURL);

private:
    // Move to d-pointer
    class Private;
    Private * const d;
    
    Q_PRIVATE_SLOT(d, void setIconURL(const KUrl &))
    Q_PRIVATE_SLOT(d, void slotCompleted())
    Q_PRIVATE_SLOT(d, void notifyChange(bool, const QString &, const QString &))
    Q_PRIVATE_SLOT(d, void slotMimetype(KIO::Job *, const QString &))
};

#endif // FAVICONUPDATER_H
