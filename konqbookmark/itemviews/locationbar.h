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

#include <QCompleter>
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

    protected:
        virtual void paintEvent(QPaintEvent* ev);

    private:
        class Private;
        Private* const d;

        Q_PRIVATE_SLOT(d, void slotReturnPressed(const QString &));
        Q_PRIVATE_SLOT(d, void slotCompletionActivated(const QModelIndex &));
        Q_PRIVATE_SLOT(d, void slotCurrentCompletionChanged(const QModelIndex &));
        Q_PRIVATE_SLOT(d, void slotTextChanged(const QString &));
        Q_PRIVATE_SLOT(d, void slotIgnoreNextTextChanged());
    };

    /**
     * HACK
     * The purpose of this class is to be able to create a QCompleter which emits
     * ignoreNextTextChanged() signal when text of the completed widget changed but it was
     * only because the user selected using up & down a new item in the completion list.
     *
     * THis is necessary because basically the completion model of the location bar is non 
     * trivial and thus the completion is used in QCompleter::UnfilteredPopupCompletion mode, 
     * but then the model itself is a QSortFilterProxyModel that gets updated, filtered and 
     * resorted each time the user enters a new query. Normally this is done automatically by 
     * QCompleter, which uses an internal QCompleterModel proxy model, but in our case we need 
     * to do ourselves, and the problem is that QCompleter does not emit a prefixTextChanged() 
     * signal so we use KLineEdit textChanged() signal instead for user input query updates. 
     * Problem is, this text gets also always updated:
     *
     *  1. when the user types in.
     *  2. when the user selects a new option in the popup completion box using key up/down.
     *
     * We are only insterested in type 1. text changes. This class detects them.
     */
    class KCompleter : public QCompleter
    {
        Q_OBJECT

    public:
        KCompleter(QObject *parent = 0) : QCompleter(parent) {}

    protected:
        virtual bool eventFilter(QObject* o, QEvent* e);

        /**
         * When the widget loses focus QLineEdit unexpectadly disconnects all signals from
         * the completer to the QLineEdit, so here we reimplement disconnectNotify() to
         * reconnect the slotIgnoreNextTextChanged() in that case.
         */
        void disconnectNotify(const char *signal);

    Q_SIGNALS:
        void ignoreNextTextChanged();
    };

}

#endif // KONQUEROR_LOCATION_BAR_H
