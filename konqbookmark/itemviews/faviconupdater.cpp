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

FavIconUpdater::FavIconUpdater(QObject *parent)
    : QObject(parent),
      m_favIconModule("org.kde.kded", "/modules/favicons", QDBusConnection::sessionBus())
{
    QObject::connect(&m_favIconModule, SIGNAL(iconChanged(bool,QString,QString)),
        this, SLOT(notifyChange(bool,QString,QString)) );
    m_part = 0;
    m_webGrabber = 0;
    m_browserIface = 0;
    m_iconUrl = m_url = KUrl();
}

void FavIconUpdater::slotCompleted()
{
    // kDebug() << "FavIconUpdater::slotCompleted";
    // kDebug() << "emit done(true)";
    emit done(m_iconUrl.isValid(), m_iconUrl);
}

void FavIconUpdater::downloadIcon(const KUrl &url)
{
    m_url = url;
    m_iconUrl = KMimeType::favIconForUrl(url);
    if (m_iconUrl.isValid()) {
        // kDebug() << "downloadIcon() - favicon" << m_iconUrl;
        // kDebug() << "emit done(true, favicon)";
        emit done(true, m_iconUrl);
    } else {
        kDebug()<<"no favicon found "<<endl;
        m_webupdate = false;
        m_favIconModule.downloadHostIcon(url.url());
    }
}

FavIconUpdater::~FavIconUpdater()
{
    // kDebug() << "~FavIconUpdater";
    delete m_browserIface;
    delete m_webGrabber;
    delete m_part;
}

void FavIconUpdater::downloadIconActual(const KUrl &url)
{
    kDebug() << "FavIconUpdater::downloadIconActual" << endl;
    m_url = url;
    m_iconUrl = KUrl();
    m_webupdate = true;

    if (!m_part) {
        KParts::ReadOnlyPart *part
            = KServiceTypeTrader
            ::createInstanceFromQuery<KParts::ReadOnlyPart>("text/html", QString());

        part->setProperty("pluginsEnabled", QVariant(false));
        part->setProperty("javaScriptEnabled", QVariant(false));
        part->setProperty("javaEnabled", QVariant(false));
        part->setProperty("autoloadImages", QVariant(false));

        connect(part, SIGNAL( canceled(const QString &) ),
                this, SLOT( slotCompleted() ));
        connect(part, SIGNAL( completed() ),
                this, SLOT( slotCompleted() ));

        KParts::BrowserExtension *ext = KParts::BrowserExtension::childObject(part);
        assert(ext);

        m_browserIface = new FavIconBrowserInterface(this);
        ext->setBrowserInterface(m_browserIface);

        connect(ext, SIGNAL( setIconURL(const KUrl &) ),
                this, SLOT( setIconURL(const KUrl &) ));

        m_part = part;
    }

    m_webGrabber = new FavIconWebGrabber(m_part, m_url);
}

// khtml callback
void FavIconUpdater::setIconURL(const KUrl &iconUrl)
{
    m_favIconModule.setIconForUrl(m_url.url(), iconUrl.url());
    m_iconUrl = iconUrl;
}

void FavIconUpdater::notifyChange(bool isHost,
    const QString& hostOrURL,
    const QString& iconName)
{
    Q_UNUSED(isHost);
    Q_UNUSED(hostOrURL);

    // kDebug() << "FavIconUpdater::notifyChange()";

    if (iconName.isNull() && !m_webupdate) {
        // no icon found, try webupdater 
        downloadIconActual(m_url);
    } else {
        // Either we have an icon or we already tried the webupdater
        m_iconUrl = iconName;
        emit done(!iconName.isNull(), iconName);
    } 
}

/* -------------------------- */

FavIconWebGrabber::FavIconWebGrabber(KParts::ReadOnlyPart *part, const KUrl &url)
    : m_part(part), m_url(url)
{

    // kDebug() << "FavIconWebGrabber::FavIconWebGrabber starting KIO::get()";

    // the use of KIO rather than directly using KHTML is to allow silently abort on error
    KIO::Job *job = KIO::get(m_url, KIO::NoReload, KIO::HideProgressInfo);
    job->addMetaData( QString("cookies"), QString("none") );
    connect(job, SIGNAL( result( KJob *)),
            this, SLOT( slotFinished(KJob *) ));
    connect(job, SIGNAL( mimetype( KIO::Job *, const QString &) ),
            this, SLOT( slotMimetype(KIO::Job *, const QString &) ));
}

void FavIconWebGrabber::slotMimetype(KIO::Job *job, const QString & /*type*/) {
    KIO::SimpleJob *sjob = static_cast<KIO::SimpleJob *>(job);
    m_url = sjob->url(); // allow for redirection
    sjob->putOnHold();

    // QString typeLocal = typeUncopied; // local copy
    // kDebug() << "slotMimetype : " << typeLocal;
    // TODO - what to do if typeLocal is not text/html ??

    m_part->openUrl(m_url);
}

void FavIconWebGrabber::slotFinished(KJob *job) {
    if (job->error()) {
        // kDebug() << "FavIconWebGrabber::slotFinished() " << job->errorString();
    }
}

#include "faviconupdater.moc"
