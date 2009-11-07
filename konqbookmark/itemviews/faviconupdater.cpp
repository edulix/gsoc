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

#include "faviconupdater.h"

#include <kdebug.h>
#include <klocale.h>
#include <kapplication.h>
#include <kio/job.h>
#include <kmimetype.h>
#include <kparts/part.h>
#include <kparts/browserextension.h>
#include <kservicetypetrader.h>
#include <assert.h>

class FavIconUpdater::Private
{
public:
    Private(FavIconUpdater *parent);
    ~Private();
    
    void setIconURL(const KUrl &iconURL);
    void slotCompleted();
    void notifyChange(bool isHost, const QString &hostOrURL, const QString &iconName);
    void slotMimetype(KIO::Job *job, const QString &_type);
    
public:
    FavIconUpdater *q;
    KParts::ReadOnlyPart *m_part;
    KUrl m_iconUrl;
    KUrl m_url;
    bool m_webupdate;
    org::kde::FavIcon m_favIconModule;
};

FavIconUpdater::Private::Private(FavIconUpdater *parent)
    : q(parent), m_favIconModule("org.kde.kded", "/modules/favicons", QDBusConnection::sessionBus())
{
    m_part = 0;
    m_iconUrl = m_url = KUrl();
    
    QObject::connect(&m_favIconModule, SIGNAL(iconChanged(bool,QString,QString)),
        q, SLOT(notifyChange(bool,QString,QString)) );
}

FavIconUpdater::Private::~Private()
{
    delete m_part;
}

void FavIconUpdater::Private::slotCompleted()
{
    emit q->done(m_iconUrl.isValid(), m_iconUrl);
}

// khtml callback
void FavIconUpdater::Private::setIconURL(const KUrl &iconUrl)
{
    m_favIconModule.setIconForUrl(m_url.url(), iconUrl.url());
    m_iconUrl = iconUrl;
}

void FavIconUpdater::Private::notifyChange(bool isHost,
    const QString& hostOrURL,
    const QString& iconName)
{
    Q_UNUSED(isHost);
    Q_UNUSED(hostOrURL);

    if (iconName.isNull() && !m_webupdate) {
        // no icon found, try webupdater 
        q->downloadIconActual(m_url);
    } else {
        // Either we have an icon or we already tried the webupdater
        m_iconUrl = iconName;
        emit q->done(!iconName.isNull(), iconName);
    } 
}

void FavIconUpdater::Private::slotMimetype(KIO::Job *job, const QString &/*type*/)
{
    KIO::SimpleJob *sjob = static_cast<KIO::SimpleJob *>(job);
    m_url = sjob->url(); // allow for redirection
    sjob->putOnHold();
    m_part->openUrl(m_url);
}


FavIconUpdater::FavIconUpdater(QObject *parent)
    : QObject(parent), d(new Private(this))
{
}

FavIconUpdater::~FavIconUpdater()
{
    delete d;
}

void FavIconUpdater::downloadIcon(const KUrl &url)
{
    d->m_url = url;
    d->m_iconUrl = KMimeType::favIconForUrl(url);
    if (d->m_iconUrl.isValid()) {
        emit done(true, d->m_iconUrl);
    } else {
        kDebug() << "no favicon found " << endl;
        d->m_webupdate = false;
        d->m_favIconModule.downloadHostIcon(url.url());
    }
}

void FavIconUpdater::downloadIconActual(const KUrl &url)
{
    kDebug() << "FavIconUpdater::downloadIconActual" << endl;
    d->m_url = url;
    d->m_iconUrl = KUrl();
    d->m_webupdate = true;

    if (!d->m_part) {
        KParts::ReadOnlyPart *part =
            KServiceTypeTrader::createInstanceFromQuery<KParts::ReadOnlyPart>("text/html", QString());

        part->setProperty("pluginsEnabled", QVariant(false));
        part->setProperty("javaScriptEnabled", QVariant(false));
        part->setProperty("javaEnabled", QVariant(false));
        part->setProperty("autoloadImages", QVariant(false));

        connect(part, SIGNAL(canceled(const QString &)),
                this, SLOT(slotCompleted()));
        connect(part, SIGNAL(completed()),
                this, SLOT(slotCompleted()));

        KParts::BrowserExtension *ext = KParts::BrowserExtension::childObject(part);
        assert(ext);

        connect(ext, SIGNAL(setIconURL(const KUrl &)),
            this, SLOT(setIconURL(const KUrl &)));

        d->m_part = part;
    }

    KIO::Job *job = KIO::get(d->m_url, KIO::NoReload, KIO::HideProgressInfo);
    job->addMetaData(QString("cookies"), QString("none"));
    connect(job, SIGNAL(mimetype( KIO::Job *, const QString &)),
        this, SLOT(slotMimetype(KIO::Job *, const QString &)));
}

#include "faviconupdater.moc"
