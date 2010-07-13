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

#ifndef KLINEEDITVIEWBUTTON_H
#define KLINEEDITVIEWBUTTON_H

#include "konqbookmark_export.h"
#include "favicon_interface.h"

#include <QObject>
#include <QWidget>
#include <QPixmap>
#include <QSize>

class QTimeLine;

namespace Konqueror
{
    /**
     * Icon Button to be shown inside the LocationBar.
     *
     * It can be animated with fade in/out when it appears and disappears. It
     * displays a given icon.
     *
     * @author Eduardo Robles Elvira <edulix@gmail.com>
     */
    class KONQBOOKMARK_EXPORT LocationBarButton : public QWidget
    {
        Q_OBJECT

    public:
        LocationBarButton(QWidget *parent);

        QSize sizeHint() const;

        void animateVisible(bool visible);

        void setPixmap(const QPixmap& p);

        QPixmap pixmap();

        void setAnimationsEnabled(bool animationsEnabled);

    protected:
        void paintEvent(QPaintEvent *event);

    protected Q_SLOTS:
        void animationFinished();

    private:
        QTimeLine *m_timeline;
        QPixmap m_pixmap;
    };
}

#endif // KLINEEDITVIEWBUTTON_H
