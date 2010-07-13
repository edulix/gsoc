/* This file is part of the KDE libraries

   Copyright (C) 2007 Aaron Seigo <aseigo@kde.org>
   Copyright (c) 2009 Benjamin C. Meyer  <ben@meyerhome.net>
   Copyright (c) 2009 Eduardo Robles Elvira <edulix@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License (LGPL) as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later
   version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "locationbarbutton.h"

#include <QPainter>
#include <QTimeLine>
#include <KGlobalSettings>

using namespace Konqueror;

LocationBarButton::LocationBarButton(QWidget *parent)
    : QWidget(parent)
{
    m_timeline = new QTimeLine(200, this);
    m_timeline->setFrameRange(0, 255);
    m_timeline->setCurveShape(QTimeLine::EaseInOutCurve);
    m_timeline->setDirection(QTimeLine::Backward);
    connect(m_timeline, SIGNAL(finished()), this, SLOT(animationFinished()));
    connect(m_timeline, SIGNAL(frameChanged(int)), this, SLOT(update()));
}

QSize LocationBarButton::sizeHint() const
{
    return m_pixmap.size();
}

void LocationBarButton::animateVisible(bool visible)
{
    if (visible) {
        if (m_timeline->direction() == QTimeLine::Forward) {
            return;
        }

        m_timeline->setDirection(QTimeLine::Forward);
        m_timeline->setDuration(150);
        show();
    } else {
        if (m_timeline->direction() == QTimeLine::Backward) {
            return;
        }

        m_timeline->setDirection(QTimeLine::Backward);
        m_timeline->setDuration(250);
    }

    if (KGlobalSettings::graphicEffectsLevel() & KGlobalSettings::SimpleAnimationEffects) {
        if (m_timeline->state() != QTimeLine::Running)
            m_timeline->start();
    } else {
        setVisible(m_timeline->direction() == QTimeLine::Forward);
    }
}

void LocationBarButton::setPixmap(const QPixmap& p)
{
    m_pixmap = p;
    update();
}

QPixmap LocationBarButton::pixmap()
{
    return m_pixmap;
}

void LocationBarButton::setAnimationsEnabled(bool animationsEnabled)
{
    // We need to set the current time in the case that we had the clear
    // button shown, for it being painted on the paintEvent(). Otherwise
    // it wont be painted, resulting (m->timeLine->currentTime() == 0) true,
    // and therefore a bad painting. This is needed for the case that we
    // come from a non animated widget and want it animated. (ereslibre)
    if (animationsEnabled && m_timeline->direction() == QTimeLine::Forward) {
        m_timeline->setCurrentTime(150);
    }
}

void LocationBarButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    if (KGlobalSettings::graphicEffectsLevel() & KGlobalSettings::SimpleAnimationEffects) {
        if (m_pixmap.isNull() || m_timeline->currentTime() == 0) {
            return;
        }

        QPainter p(this);
        p.setOpacity(m_timeline->currentValue());
        p.drawPixmap((width() - m_pixmap.width()) / 2,
                    (height() - m_pixmap.height()) / 2 - 1,
                    m_pixmap);
    } else {
        QPainter p(this);
        p.setOpacity(1); // make sure
        p.drawPixmap((width() - m_pixmap.width()) / 2,
                    (height() - m_pixmap.height()) / 2 - 1,
                    m_pixmap);
    }
}

void LocationBarButton::animationFinished()
{
    if (m_timeline->direction() == QTimeLine::Forward) {
        update();
    } else {
        hide();
    }
}
