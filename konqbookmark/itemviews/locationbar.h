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

#include <QPlainTextEdit>
#include <QtGui/QCompleter>
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
    class KONQBOOKMARK_EXPORT LocationBar : public QPlainTextEdit
    {
        Q_OBJECT
        Q_PROPERTY(QString clickMessage READ clickMessage WRITE setClickMessage
            NOTIFY clickMessageChanged)
        Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
        Q_PROPERTY(int completionDelay READ completionDelay WRITE setCompletionDelay
            NOTIFY completionDelayChanged)

    public:
        enum WidgetPosition {
            LeftSide,
            RightSide
        };

        LocationBar(QWidget *parent = 0);
        virtual ~LocationBar();

        /**
         * Returns the text inside the lineedit split in words
         */
        QStringList words() const;
        QString clickMessage() const;
        QString text() const;

        /**
         * Minimum time between key strokes needed before a completion ievent is triggered.
         * This is used for making completion feel faster by creating less completion events.
         * 200 miliseconds by default.
         *
         * @returns delay in miliseconds
         */
        int completionDelay() const;

        /**
         *  Set the completion delay in miliseconds.
         */
        void setCompletionDelay(int miliseconds);

        void addWidget(QWidget *widget, WidgetPosition position);
        void removeWidget(QWidget *widget);
        int textMargin(WidgetPosition position) const;
        void setWidgetSpacing(int spacing);
        int widgetSpacing() const;

    public Q_SLOTS:
        void setURL(const QString &url);
        void setClickMessage(const QString &clickMessage);
        void setText(const QString &text);
        void slotTextChanged();

    Q_SIGNALS:
        /**
         * Specialized signal that emits the state of the modifier
         * keys along with the actual activated text.
         */
        void returnPressed(const QString &, Qt::KeyboardModifiers);

        /**
         * Emitted everytime the location bar text changes, even if it was because of
         * a completion event.
         */
        void textChanged(const QString &);

        /**
         * Emitted only when the location bar text changes because the user wrote something.
         * Not emitted when location bar text changes because of a completion event.
         */
        void userTextChanged(const QString &);

    protected Q_SLOTS:
        void init();

    protected:
        void paintEvent(QPaintEvent* ev);
        void keyPressEvent(QKeyEvent * e);

        /**
         * Re-implemented for internal reasons.  API not affected.
         *
         * See QLineEdit::resizeEvent().
         */
        void resizeEvent(QResizeEvent *);

        /**
         * Reimplemented so that text selection is emptied on focus out
         */
        void focusOutEvent(QFocusEvent *e);

        /**
         * Reimplemented so that all text gets selected on focus in
         */
        void focusInEvent(QFocusEvent *e);
    private:
        class Private;
        Private* const d;

        Q_PRIVATE_SLOT(d, void slotCompletionActivated(const QModelIndex &));
        Q_PRIVATE_SLOT(d, void slotCurrentCompletionChanged(const QModelIndex &));
        Q_PRIVATE_SLOT(d, void slotComplete());
        Q_PRIVATE_SLOT(d, void updateSideWidgetLocations());
        Q_PRIVATE_SLOT(d, void updateTextMargins());
    };

    class SideWidget : public QWidget
    {
        Q_OBJECT
    public:
        SideWidget(QWidget *parent = 0);

    protected:
        bool event(QEvent *event);
        void paintEvent(QPaintEvent* event);

    Q_SIGNALS:
        void sizeHintChanged();
    };
}

#endif // KONQUEROR_LOCATION_BAR_H
