/* This file is part of the KDE libraries

   Copyright (c) 2000 Carsten Pfeiffer <pfeiffer@kde.org>
                 2000 Stefan Schimanski <1Stein@gmx.de>
                 2000,2001,2002,2003,2004 Dawit Alemayehu <adawit@kde.org>
                 2009 Eduardo Robles Elvira <edulix@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License (LGPL) as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KCOMPLETIONBOX_H
#define KCOMPLETIONBOX_H

#include "konqbookmark_export.h"

#include <QtGui/QListView>
#include <QtCore/QModelIndex>

class QEvent;

/**
 * @short A helper widget for "completion-widgets" (KLineEdit, KComboBox))
 *
 * A little utility class for "completion-widgets", like KLineEdit or
 * KComboBox. KCompletionView is a listbox, displayed as a rectangle without
 * any window decoration, usually directly under the lineedit or combobox.
 * It is filled with all possible matches for a completion, so the user
 * can select the one he wants.
 *
 * It is used when KGlobalSettings::Completion == CompletionPopup or CompletionPopupAuto.
 *
 * @author Carsten Pfeiffer <pfeiffer@kde.org>
 */
class KONQBOOKMARK_EXPORT KCompletionView : public QListView
{
    Q_OBJECT
    Q_PROPERTY(bool isTabHandling READ isTabHandling WRITE setTabHandling)
    Q_PROPERTY(QString cancelledText READ cancelledText WRITE setCancelledText)

public:
    /**
     * Constructs a KCompletionView.
     *
     * The parent widget is used to give the focus back when pressing the
     * up-button on the very first item.
     */
    explicit KCompletionView(QWidget *parent = 0);

    /**
     * Destroys the box
     */
    ~KCompletionView();

    virtual QSize sizeHint() const;

public Q_SLOTS:
    /**
     * Adjusts the size of the box to fit the width of the parent given in the
     * constructor and pops it up at the most appropriate place, relative to
     * the parent.
     *
     * Depending on the screensize and the position of the parent, this may
     * be a different place, however the default is to pop it up and the
     * lower left corner of the parent.
     *
     * Make sure to hide() the box when appropriate.
     */
    virtual void popup();

    /**
     * Makes this widget (when visible) capture Tab-key events to traverse the
     * items in the dropdown list (Tab goes down, Shift+Tab goes up).
     *
     * On by default, but should be turned off when used in combination with KUrlCompletion.
     * When off, KLineEdit handles Tab itself, making it select the current item from the completion box,
     * which is particularly useful when using KUrlCompletion.
     *
     * @see isTabHandling
     */
    void setTabHandling(bool enable);

    /**
     * @returns true if this widget is handling Tab-key events to traverse the
     * items in the dropdown list, otherwise false.
     *
     * Default is true.
     *
     * @see setTabHandling
     */
    bool isTabHandling() const;

    /**
     * Sets the text to be emitted if the user chooses not to
     * pick from the available matches.
     *
     * If the canceled text is not set through this function, the
     * userCancelled signal will not be emitted.
     *
     * @see userCancelled(const QString &)
     * @param text  the text to be emitted if the user cancels this box
     */
    void setCancelledText(const QString &text);

    /**
     * @returns the text set via setCancelledText() or QString().
     */
    QString cancelledText() const;

    /**
     * Moves the selection one line down or select the first item if nothing is selected yet.
     */
    void down();

    /**
     * Moves the selection one line up or select the first item if nothing is selected yet.
     */
    void up();

    /**
     * Moves the selection one page down.
     */
    void pageDown();

    /**
     * Moves the selection one page up.
     */
    void pageUp();

    /**
     * Moves the selection up to the first item.
     */
    void home();

    /**
     * Moves the selection down to the last item.
     */
    void end();

    /**
     * Re-implemented for internal reasons.  API is unaffected.
     */
    virtual void setVisible(bool visible);

Q_SIGNALS:
    /**
     * Emitted whenever the user chooses to ignore the available
     * selections and close the this box.
     */
    void userCancelled(const QString &text);

protected:
    /**
     * This calculates the size of the dropdown and the relative position of the top
     * left corner with respect to the parent widget. This matches the geometry and position
     * normally used by K/QComboBox when used with one.
     */
    QRect calculateGeometry() const;

    /**
     * This properly sizes and positions the listbox.
     */
    friend class KLineEditView;
    void sizeAndPosition();

    /**
     * Reimplemented from KListWidget to get events from the viewport (to hide
     * this widget on mouse-click, Escape-presses, etc.
     */
    virtual bool eventFilter(QObject *o, QEvent *e);

    /**
     * The preferred global coordinate at which the completion box's top left corner
     * should be positioned.
     */
    virtual QPoint globalPositionHint() const;

protected Q_SLOTS:
    /**
     * Called when an item was activated. Emits
     * activated() with the item.
     */
    virtual void slotActivated(const QModelIndex &index);

    /**
     * Reimplemented to ensure the selected items are visible.
     */
    void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);

private Q_SLOTS:
    void canceled();

private:
    class Private;
    Private* const d;
};


#endif // KCOMPLETIONBOX_H
