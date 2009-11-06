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

class FavIconWebGrabber : public QObject
{
    Q_OBJECT
public:
    FavIconWebGrabber(KParts::ReadOnlyPart *part, const KUrl &url);
    ~FavIconWebGrabber() {}

protected Q_SLOTS:
    void slotMimetype(KIO::Job *job, const QString &_type);
    void slotFinished(KJob *job);

private:
    KParts::ReadOnlyPart *m_part;
    KUrl m_url;
};

class FavIconBrowserInterface;

class FavIconUpdater : public QObject
{
    Q_OBJECT

public:
    FavIconUpdater(QObject *parent);
    ~FavIconUpdater();
    void downloadIcon(const KUrl &url);
    void downloadIconActual(const KUrl &url);

private Q_SLOTS:
    void setIconURL(const KUrl &iconURL);
    KUrl iconUrl() const { return m_iconUrl; }
    void slotCompleted();
    void notifyChange(bool isHost, const QString& hostOrURL, const QString& iconName);

Q_SIGNALS:
    void done(bool succeeded, KUrl iconURL);

private:
    KParts::ReadOnlyPart *m_part;
    FavIconBrowserInterface *m_browserIface;
    FavIconWebGrabber *m_webGrabber;
    KUrl m_iconUrl;
    KUrl m_url;
    bool m_webupdate;
    org::kde::FavIcon m_favIconModule;
};

class FavIconBrowserInterface : public KParts::BrowserInterface
{
    Q_OBJECT
public:
    FavIconBrowserInterface(FavIconUpdater *view)
        : KParts::BrowserInterface(view), m_view(view) {}
private:
    FavIconUpdater *m_view;
};


#endif // FAVICONUPDATER_H
