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

#include "locationbarclearbutton.h"
#include <KLocale>
#include <KIconLoader>

class Konqueror::LocationBarClearButton::Private
{
public:
    LocationBar *parent;
    bool wideEnoughForClear:1;
    bool clickInClear:1;
};

Konqueror::LocationBarClearButton::LocationBarClearButton(Konqueror::LocationBar* parent): LocationBarButton(parent), d(new Private)
{
    setCursor(Qt::ArrowCursor);
    setToolTip(i18nc("@action:button Clear current text in the location bar", "Clear text"));
    d->clickInClear = false;
    d->wideEnoughForClear = false;
    d->parent = parent;

    hide();
    updateIcon(parent->text());
    updateNeeded();
    connect(parent, SIGNAL(textChanged(QString)), this, SLOT(updateIcon(QString)));
    connect(parent, SIGNAL(readOnlyChanged(bool)), this, SLOT(updateNeeded()));
}

Konqueror::LocationBarClearButton::~LocationBarClearButton()
{
    disconnect(parent(), SIGNAL(textChanged(QString)), this, SLOT(updateIcon(QString)));
    disconnect(parent(), SIGNAL(readOnlyChanged(bool)), this, SLOT(updateNeeded()));
    delete d;
}

void Konqueror::LocationBarClearButton::updateIcon(const QString &text)
{
    if (d->parent->isReadOnly()) {
        return;
    }

    int clearButtonState = KIconLoader::DefaultState;

    if (d->wideEnoughForClear && text.length() > 0) {
        animateVisible(true);
    } else {
        animateVisible(false);
    }

    if (d->parent->layoutDirection() == Qt::LeftToRight) {
        setPixmap(SmallIcon("edit-clear-locationbar-rtl", 0, clearButtonState));
    } else {
        setPixmap(SmallIcon("edit-clear-locationbar-ltr", 0, clearButtonState));
    }
}

void Konqueror::LocationBarClearButton::updateNeeded()
{
    if (d->parent->isReadOnly()) {
        return;
    }

    const QSize geom = d->parent->size();
    const QFontMetrics fm(d->parent->font());
    const int em = fm.width("m");

    // make sure we have enough room for the clear button
    // no point in showing it if we can't also see a few characters as well
    const bool wideEnough = d->parent->viewport()->width() > 4 * em;

    if (wideEnough != d->wideEnoughForClear) {
        // we may (or may not) have been showing the button, but now our
        // positiong on that matter has shifted, so let's ensure that it
        // is properly visible (or not)
        d->wideEnoughForClear = wideEnough;
        updateIcon(d->parent->text());
    }
}

void Konqueror::LocationBarClearButton::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton || e->button() == Qt::MidButton ) {
        d->clickInClear = true;
    }
}

void Konqueror::LocationBarClearButton::mouseReleaseEvent(QMouseEvent* e)
{
    if (d->clickInClear && e->button() != Qt::MidButton) {
        d->parent->setText(QString());
        d->clickInClear = false;
        emit clicked();
        return;
    }
}
