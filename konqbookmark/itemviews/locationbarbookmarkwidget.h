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

#ifndef LOCATIONBARBOOKMARKWIDGET_H
#define LOCATIONBARBOOKMARKWIDGET_H

#include "klineeditviewbutton.h"
#include "locationbar.h"
#include "konqbookmark_export.h"


namespace Konqueror
{
class Private;
/**
 * This location bar widget shows a star that is filled if the current location bar url
 * is bookmarked, and a greyed out star if it's not. It also lets you add a bookmark clicking in
 * the star icon.
 */
class KONQBOOKMARK_EXPORT LocationBarBookmarkWidget : public KLineEditViewButton
{
    Q_OBJECT

public:
    LocationBarBookmarkWidget(LocationBar *locationBar);
    virtual ~LocationBarBookmarkWidget();


protected:
    void mouseReleaseEvent(QMouseEvent *event);

private:
    class Private;
    Private * const d;

    Q_PRIVATE_SLOT(d, void urlChanged())
    Q_PRIVATE_SLOT(d, void updateCurrentUrl())
    Q_PRIVATE_SLOT(d, void placeUpdated())

};

}

#endif // LOCATIONBARBOOKMARKWIDGET_H
