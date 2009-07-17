/*****************************************************************************
 * Copyright (C) 2006 by Peter Penz <peter.penz@gmx.at>                      *
 * Copyright (C) 2006 by Aaron J. Seigo <aseigo@kde.org>                     *
 * Copyright (C) 2009 Eduardo Robles Elvira <edulix@gmail.com>               *
 *                                                                           *
 * This library is free software; you can redistribute it and/or             *
 * modify it under the terms of the GNU Library General Public               *
 * License version 2 as published by the Free Software Foundation.           *
 *                                                                           *
 * This library is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public License *
 * along with this library; see the file COPYING.LIB.  If not, write to      *
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,      *
 * Boston, MA 02110-1301, USA.                                               *
 *****************************************************************************/

#ifndef KTREEMODELNAVIGATORBUTTON_P_H
#define KTREEMODELNAVIGATORBUTTON_P_H

#include "ktreemodelbutton_p.h"
#include "ktreemodelnavigatormenu_p.h"

#include <kurl.h>

#include <QPointer>
#include <QModelIndex>

class KTreeModelNavigator;
class QDropEvent;
class QPaintEvent;

/**
 * @brief Button of the Tree Model Navigator which contains one part of the current path.
 *
 * It is possible to drop a various number of items to an KTreeModelNavigatorButton. In this case
 * a context menu is opened where the user must select whether he wants
 * to copy, move or link the dropped items to the URL part indicated by
 * the button.
 */
class KTreeModelNavigatorButton : public KTreeModelButton
{
    Q_OBJECT

public:
    explicit KTreeModelNavigatorButton(QModelIndex index, KTreeModelNavigator* parent);
    virtual ~KTreeModelNavigatorButton();

    void setIndex(QModelIndex index);
    QModelIndex index() const;

    void setActive(bool active);
    bool isActive() const;

    /** @see QWidget::sizeHint() */
    virtual QSize sizeHint() const;

Q_SIGNALS:
    /**
     * Is emitted if URLs have been dropped
     * to the destination \a destination.
     */
    void mimeDataDropped(const QModelIndex& destination, QDropEvent* event);

protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void enterEvent(QEvent* event);
    virtual void leaveEvent(QEvent* event);
    virtual void dropEvent(QDropEvent* event);
    virtual void dragEnterEvent(QDragEnterEvent* event);
    virtual void dragMoveEvent(QDragMoveEvent* event);
    virtual void dragLeaveEvent(QDragLeaveEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);

private Q_SLOTS:
    void updateNavigatorCurrentIndex();
    void startPopupDelay();
    void stopPopupDelay();
    void listChildItems();
    void mimeDataDropped(QAction* action, QDropEvent* event);

private:
    int arrowWidth() const;
    bool isAboveArrow(int x) const;
    bool isTextClipped() const;
    void updateMinimumWidth();

private:
    QModelIndex m_index;
    bool m_hoverArrow;
    QTimer* m_popupDelay;
    QList<QModelIndex> m_childItems;

    static QPointer<KTreeModelNavigatorMenu> m_childItemsMenu;
};

inline QModelIndex KTreeModelNavigatorButton::index() const
{
    return m_index;
}

#endif // KTREEMODELNAVIGATORBUTTON_P_H
