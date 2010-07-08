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

#ifndef KONQUEROR_LOCATION_BAR_H
#define KONQUEROR_LOCATION_BAR_H

#include "konqbookmark_export.h"
#include <KLineEdit>

#include <QtCore/QObject>

namespace Konqueror
{
    /**
     * Konqueror Location Bar. It has support for plugins that can be added to
     * the left and right of the line edit. It also features a completion popup.
     * The completion popup is an items view with a custom item delegate called
     * LocationBarDelegate. The items model used is a sorted PlacesProxy. The
     * tree of proxies models is like this:
     *
     *   QSortedFilterProxyModel
     *             ^
     *             |
     *             |
     *     PlacesProxyModel
     *             ^
     *             |
     *             |
     *      PlacesManager
     */
    class KONQBOOKMARK_EXPORT LocationBar : public KLineEdit
    {
        Q_OBJECT
    public:
        LocationBar(QWidget *parent = 0);
        virtual ~LocationBar();

        /**
         * Returns the text inside the lineedit split in words
         */
        QStringList words() const;

    public Q_SLOTS:
        void setURL(const QString &url);

    Q_SIGNALS:
        /**
         * Specialized signal that emits the state of the modifier
         * keys along with the actual activated text.
         */
        void returnPressed(const QString &, Qt::KeyboardModifiers);

    protected Q_SLOTS:
        void init();

    private:
        class Private;
        Private* const d;

        Q_PRIVATE_SLOT(d, void slotReturnPressed(const QString &));
        Q_PRIVATE_SLOT(d, void slotCompletionActivated(const QModelIndex &));
        Q_PRIVATE_SLOT(d, void slotCurrentCompletionChanged(const QModelIndex &));
        Q_PRIVATE_SLOT(d, void updateWords(const QString &));
    };
}

#endif // KONQUEROR_LOCATION_BAR_H
