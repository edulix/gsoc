/*
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

#include "locationbarfaviconwidget.h"

#include <kdebug.h>
#include <kurl.h>
#include <kiconloader.h>
#include <kmimetype.h>
#include <klocale.h>

#include <QTimer>
#include <QDBusConnection>

using namespace Konqueror;

class LocationBarFaviconWidget::Private
{
public:
    Private(LocationBarFaviconWidget *parent);
    
    void urlChanged();
    
    void updateCurrentUrl();
    
    void updateFavicon(bool succeed, const KUrl &favicon);
    
public:
    LocationBarFaviconWidget *q;
    
    QString m_currentUrl;
    QString m_nextUrl;
    QTimer m_wait;
    LocationBar *m_locationBar;
    FavIconUpdater m_faviconUpdater;
};

LocationBarFaviconWidget::Private::Private(LocationBarFaviconWidget *parent)
    : q(parent), m_faviconUpdater(0)
{
    
}

LocationBarFaviconWidget::LocationBarFaviconWidget(LocationBar *locationBar)
    : LocationBarButton(locationBar), d(new Private(this))
{
    d->m_locationBar = locationBar;
    connect(&d->m_wait, SIGNAL(timeout()), this, SLOT(updateCurrentUrl()));
    connect(locationBar, SIGNAL(textChanged(const QString &)),
        this, SLOT(urlChanged()));
    connect(locationBar, SIGNAL(returnPressed(const QString &)),
        this, SLOT(urlChanged()));
        
    connect(&d->m_faviconUpdater, SIGNAL(done(bool, KUrl)),
        this, SLOT(updateFavicon(bool,KUrl)));
    
    setCursor(Qt::ArrowCursor);
    setToolTip(i18nc("@action:button Favicon", "Favicon"));
    setPixmap(SmallIcon("folder"));
    animateVisible(true);
}

LocationBarFaviconWidget::~LocationBarFaviconWidget()
{

}

void LocationBarFaviconWidget::Private::urlChanged()
{
    kDebug();
    m_nextUrl = m_locationBar->text();
    m_wait.start(10);
}

void LocationBarFaviconWidget::Private::updateCurrentUrl()
{
    kDebug() << m_nextUrl;
    m_wait.stop();
    
    m_currentUrl = m_nextUrl;
    
    m_faviconUpdater.downloadIcon(m_currentUrl);
}

void LocationBarFaviconWidget::Private::updateFavicon(bool succeed, const KUrl &favicon)
{
    kDebug() << "favicon = " << favicon;
    if (!succeed) {
        return;
    }
    
    if (favicon.isValid()) {
        q->setPixmap(SmallIcon(favicon.url()));
    }
}

#include "locationbarfaviconwidget.moc"
