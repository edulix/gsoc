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

#ifndef KONQUEROR_LOCATION_BAR_DELEGATE_H
#define KONQUEROR_LOCATION_BAR_DELEGATE_H

#include "konqbookmark_export.h"

#include <QtGui/QStyledItemDelegate>

class QObject;
class QPainter;

namespace Konqueror
{
    class LocationBar;

    /**
     * @short Delegate that display places information in the location bar completion view.
     */
    class KONQBOOKMARK_EXPORT LocationBarDelegate : public QStyledItemDelegate
    {
        Q_OBJECT
    public:

        LocationBarDelegate(LocationBar *parent = 0);

        void paint(QPainter *painter, const QStyleOptionViewItem &option,
            const QModelIndex &index) const;

        virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

        /**
         * This way we ensure that when calling to @p QStyledItemDelegate::paint() it doesn't
         * render any text, because that's what we'll do.
         */
        QString displayText(const QVariant &value, const QLocale& locale) const
        { return QString(); }
    protected:
        /**
         * Used by @p paintText helper function.
         */
        enum TextAlignment {AlignLeft, AlignRight};

        /**
         * Paints a text underlining the part of the text matching with the
         * current location bar query.
         *
         * @return the width that will be used by the painted text.
         */
        int paintText(QPainter *painter, int x, int bottomY, QString text, TextAlignment textAlignment = AlignLeft, int maxWidth = -1) const;

    private:
        LocationBar *m_parent;
    };
}

#endif // KONQUEROR_LOCATION_BAR_DELEGATE_H
