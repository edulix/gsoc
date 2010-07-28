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


#ifndef KONQUEROR_LOCATIONBARCLEARBUTTON_H
#define KONQUEROR_LOCATIONBARCLEARBUTTON_H

#include "locationbarbutton.h"
#include "locationbar.h"
#include "konqbookmark_export.h"

namespace Konqueror
{

class KONQBOOKMARK_EXPORT LocationBarClearButton : public LocationBarButton
{
    Q_OBJECT
public:
    LocationBarClearButton(LocationBar *parent);
    virtual ~LocationBarClearButton();

public Q_SLOTS:
    void updateIcon(const QString &);
    void updateNeeded();

Q_SIGNALS:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    class Private;
    Private * const d;
};
}

#endif // KONQUEROR_LOCATIONBARCLEARBUTTON_H
