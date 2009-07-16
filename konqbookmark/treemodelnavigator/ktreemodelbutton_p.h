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

#ifndef KTREEMODELBUTTON_P_H
#define KTREEMODELBUTTON_P_H

#include <QtGui/QColor>
#include <QtGui/QPushButton>

class QEvent;
class KTreeModelNavigator;

/**
 * @brief Base class for buttons of the tree model navigator.
 *
 * Each button of the tree model navigator contains an index, which
 * is set as soon as the button has been clicked.
 */
class KTreeModelButton : public QPushButton
{
    Q_OBJECT

public:
    explicit KTreeModelButton(KTreeModelNavigator* parent);
    virtual ~KTreeModelButton();
    KTreeModelNavigator* treeModelNavigator() const;
    
protected:
    enum DisplayHint {
        ActivatedHint = 1,
        EnteredHint = 2,
        DraggedHint = 4,
        PopupActiveHint = 8
    };

    enum { BorderWidth = 2 };
    
    void setDisplayHintEnabled(DisplayHint hint, bool enable);
    bool isDisplayHintEnabled(DisplayHint hint) const;

    virtual void enterEvent(QEvent* event);
    virtual void leaveEvent(QEvent* event);
    virtual void contextMenuEvent(QContextMenuEvent* event);

    void drawHoverBackground(QPainter* painter);

    /** Returns the foreground color by respecting the current display hint. */
    QColor foregroundColor() const;

private:
    int m_displayHint;
    KTreeModelNavigator* m_treeModelNavigator;
};

inline KTreeModelNavigator* KTreeModelButton::treeModelNavigator() const
{
    return m_treeModelNavigator;
}

#endif // KTREEMODELBUTTON_P_H
