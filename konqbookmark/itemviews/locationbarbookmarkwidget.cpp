/*
    Copyright (c) 2010 Eduardo Robles Elvira <edulix@gmail.com>

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

#include "locationbarbookmarkwidget.h"
#include "placesmanager.h"
#include "place.h"
#include "konqbookmark.h"

#include <Akonadi/Item>
#include <Akonadi/Collection>
#include <Akonadi/ItemCreateJob>
#include <KDebug>
#include <KUrl>
#include <KIconEffect>
#include <KIconLoader>
#include <KLocale>

#include <QtCore/QTimer>
#include <QtCore/QDateTime>
#include <QtGui/QMouseEvent>
#include <QDBusConnection>

using namespace Konqueror;

class LocationBarBookmarkWidget::Private
{
public:
    Private(LocationBarBookmarkWidget *parent);

    void urlChanged();
    void updateCurrentUrl();
    void placeUpdated();

public:
    LocationBarBookmarkWidget *q;

    QString m_currentUrl;
    QString m_nextUrl;
    Place *m_currentPlace;
    QTimer m_wait;
    LocationBar *m_locationBar;
};

LocationBarBookmarkWidget::Private::Private(LocationBarBookmarkWidget *parent)
    : q(parent)
{

}

LocationBarBookmarkWidget::LocationBarBookmarkWidget(LocationBar *locationBar)
    : KLineEditViewButton(locationBar), d(new Private(this))
{
    d->m_locationBar = locationBar;
    d->m_currentPlace = 0;
    connect(&d->m_wait, SIGNAL(timeout()), this, SLOT(updateCurrentUrl()));
    connect(locationBar, SIGNAL(textChanged(const QString &)),
        this, SLOT(urlChanged()));
    connect(locationBar, SIGNAL(returnPressed(const QString &)),
        this, SLOT(urlChanged()));

    setCursor(Qt::ArrowCursor);
    setToolTip(i18nc("@action:button Bookmark", "Bookmark"));
    setPixmap(KIconEffect().apply(SmallIcon("rating"), KIconEffect::ToGray, 1.0, QColor(), false));
    animateVisible(true);
}

LocationBarBookmarkWidget::~LocationBarBookmarkWidget()
{

}

void LocationBarBookmarkWidget::Private::urlChanged()
{
    kDebug();
    m_nextUrl = m_locationBar->text();
    m_wait.start(500);
}

void LocationBarBookmarkWidget::Private::updateCurrentUrl()
{
    kDebug() << m_nextUrl;
    m_wait.stop();

    m_currentUrl = m_nextUrl;
    if (m_currentUrl == m_nextUrl) {
        return;
    }

    Place* place = PlacesManager::self()->place(m_currentUrl);
    if (m_currentPlace) {
        disconnect(m_currentPlace, SIGNAL(updated()), q, SLOT(placeUpdated()));
    }
    m_currentPlace = place;
    connect(place, SIGNAL(updated()), q, SLOT(placeUpdated()));
}

void LocationBarBookmarkWidget::Private::placeUpdated()
{
    kDebug();
    QPixmap ratingPix = SmallIcon("rating");
    if (m_currentPlace->bookmark()) {
        q->setPixmap(ratingPix);
    } else {
        q->setPixmap(KIconEffect().apply(ratingPix, KIconEffect::DeSaturate, 1.0, QColor(), true));
    }
}

void LocationBarBookmarkWidget::mouseReleaseEvent(QMouseEvent* event)
{
    kDebug();
    if (event->button() != Qt::LeftButton) {
        return;
    }

    Place* place = PlacesManager::self()->place(d->m_currentUrl);

    if (place->bookmark()) {
        // Remove bookmark
        // TODO: show bookmark information widget
    } else {
        // Add bookmark
        KonqBookmark bookmark;
        bookmark.setTitle(place->title());
        bookmark.setUrl(place->url());
        Akonadi::Item item;
        item.setMimeType(KonqBookmark::mimeType());
        item.setPayload<KonqBookmark>(bookmark);

        // TODO: use model watcher to only set the icon to "bookmark" state when it has
        // actually been added
        new Akonadi::ItemCreateJob(item, PlacesManager::self()->unsortedBookmarksFolder());
    }
}


#include "locationbarbookmarkwidget.moc"
