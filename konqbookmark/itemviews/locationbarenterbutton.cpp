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

#include "locationbarenterbutton.h"

#include <QtGui/QApplication>

#include <KDebug>
#include <KLocale>
#include <KIconLoader>

using namespace Konqueror;

class LocationBarEnterButton::Private
{
public:
    LocationBar* parent;
    bool clickInClear:1;
    QString currentText;
    QString currentUrl;
};

LocationBarEnterButton::LocationBarEnterButton(LocationBar* parent)
    : LocationBarButton(parent), d(new Private)
{
    connect(parent, SIGNAL(textChanged(QString)), this, SLOT(slotTextChanged(QString)));
    connect(parent, SIGNAL(urlChanged(QString)), this, SLOT(slotCurrentUrlChanged(QString)));
    connect(this, SIGNAL(returnPressed(QString,Qt::KeyboardModifiers)), parent,
            SIGNAL(returnPressed(QString,Qt::KeyboardModifiers)));

    d->parent = parent;
    hide();
    setPixmap(SmallIcon("go-next-view"));
    setCursor(Qt::ArrowCursor);
    setToolTip(i18nc("@action:button Enter current location in the location bar", "Enter location"));
}

LocationBarEnterButton::~LocationBarEnterButton()
{
    delete d;
}

void LocationBarEnterButton::checkVisibility()
{
    animateVisible(d->currentText != d->currentUrl && !d->currentText.isEmpty());
}

void LocationBarEnterButton::slotTextChanged(const QString &text)
{
    d->currentText = text;
    checkVisibility();
}

void LocationBarEnterButton::slotCurrentUrlChanged(const QString& text)
{
    d->currentUrl = text;
    checkVisibility();
}

void LocationBarEnterButton::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton || e->button() == Qt::MidButton) {
        d->clickInClear = true;
    }
}

void LocationBarEnterButton::mouseReleaseEvent(QMouseEvent *e)
{
    if (d->clickInClear && e->button() != Qt::MidButton) {
        d->clickInClear = false;

        kDebug() << "bingo";
        emit returnPressed(d->currentText, qApp->keyboardModifiers());
        return;
    }
}
