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

#ifndef LOCATIONBARENTERBUTTON_H
#define LOCATIONBARENTERBUTTON_H

#include "locationbarbutton.h"
#include "locationbar.h"
#include "konqbookmark_export.h"

namespace Konqueror
{
class KONQBOOKMARK_EXPORT LocationBarEnterButton : public LocationBarButton
{
    Q_OBJECT
public:
    LocationBarEnterButton(LocationBar *parent);
    virtual ~LocationBarEnterButton();

private Q_SLOTS:
    /**
     * Called everytime the location bar changes its displayed text, even if it was because
     * a completion event.
     */
    void slotTextChanged(const QString& text);

    /**
     * Called everytime current url changes.
     */
    void slotCurrentUrlChanged(const QString& text);

Q_SIGNALS:
        void returnPressed(const QString &, Qt::KeyboardModifiers);

protected:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);

private:
    void checkVisibility();

private:
    class Private;
    Private * const d;
};
}

#endif // LOCATIONBARENTERBUTTON_H
