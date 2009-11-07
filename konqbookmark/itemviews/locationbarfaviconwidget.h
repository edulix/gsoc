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

#ifndef LOCATIONBARFAVICONWIDGET_H
#define LOCATIONBARFAVICONWIDGET_H

#include "klineeditviewbutton.h"
#include "locationbar.h"
#include "faviconupdater.h"
#include "konqbookmark_export.h"

#include <QString>
#include <QTimer>

namespace Konqueror
{
class Private;
/**
 * Shows the favicon for current url.
 */
class KONQBOOKMARK_EXPORT LocationBarFaviconWidget : public KLineEditViewButton
{
    Q_OBJECT
    
public:
    LocationBarFaviconWidget(LocationBar *locationBar);
    
    virtual ~LocationBarFaviconWidget();
    
private:
    // Move to d-pointer
    class Private;
    Private * const d;
    
    Q_PRIVATE_SLOT(d, void urlChanged())
    Q_PRIVATE_SLOT(d, void updateCurrentUrl())
    Q_PRIVATE_SLOT(d, void updateFavicon(bool, const KUrl &))
};

}

#endif
