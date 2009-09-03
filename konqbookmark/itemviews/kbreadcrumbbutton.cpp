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

#include "kbreadcrumbbutton_p.h"

#include "kbreadcrumbnavigator.h"

#include <kcolorscheme.h>
#include <kicon.h>
#include <klocale.h>
#include <kmenu.h>
#include <kurl.h>

#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QStyle>
#include <QStyleOptionFocusRect>
#include <QModelIndexList>

KBreadCrumbButton::KBreadCrumbButton(KBreadCrumbNavigator *parent) :
    QPushButton(parent),
    m_displayHint(0),
    m_breadCrumbNavigator(parent)
{
    setFocusPolicy(Qt::NoFocus);
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    setMinimumHeight(parent->minimumHeight());
}

KBreadCrumbButton::~KBreadCrumbButton()
{
}

void KBreadCrumbButton::setDisplayHintEnabled(DisplayHint hint,
    bool enable)
{
    if (enable) {
        m_displayHint = m_displayHint | hint;
    } else {
        m_displayHint = m_displayHint & ~hint;
    }
    update();
}

bool KBreadCrumbButton::isDisplayHintEnabled(DisplayHint hint) const
{
    return (m_displayHint & hint) > 0;
}

void KBreadCrumbButton::enterEvent(QEvent *event)
{
    QPushButton::enterEvent(event);
    setDisplayHintEnabled(EnteredHint, true);
    update();
}

void KBreadCrumbButton::leaveEvent(QEvent *event)
{
    QPushButton::leaveEvent(event);
    setDisplayHintEnabled(EnteredHint, false);
    update();
}

void KBreadCrumbButton::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event);

    KMenu popup(this);

    // provide 'Copy' action, which copies the current URL of
    // the URL navigator into the clipboard
    QAction *copyAction = popup.addAction(KIcon("edit-copy"), i18n("Copy"));

    // provide 'Paste' action, which copies the current clipboard text
    // into the URL navigator
    QAction *pasteAction = popup.addAction(KIcon("edit-paste"), i18n("Paste"));
    QClipboard *clipboard = QApplication::clipboard();
    pasteAction->setEnabled(!clipboard->text().isEmpty());

    QAction *activatedAction = popup.exec(QCursor::pos());
    if (activatedAction == copyAction) {
        QModelIndexList currentIndex;
        currentIndex.append(m_breadCrumbNavigator->currentIndex());
        clipboard->setMimeData(m_breadCrumbNavigator->model()->mimeData(currentIndex));
    } else if (activatedAction == pasteAction) {
        m_breadCrumbNavigator->setCurrentIndex(clipboard->mimeData());
    }
}

void KBreadCrumbButton::drawHoverBackground(QPainter* painter)
{
    const bool isHighlighted = isDisplayHintEnabled(EnteredHint) ||
        isDisplayHintEnabled(DraggedHint) ||
        isDisplayHintEnabled(PopupActiveHint);

    QColor backgroundColor = isHighlighted ? palette().color(QPalette::Highlight) : Qt::transparent;

    if (backgroundColor != Qt::transparent) {
        // TODO: the backgroundColor should be applied to the style
        QStyleOptionViewItemV4 option;
        option.initFrom(this);
        option.state = QStyle::State_Enabled | QStyle::State_MouseOver;
        option.viewItemPosition = QStyleOptionViewItemV4::OnlyOne;
        style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter, this);
    }
}

QColor KBreadCrumbButton::foregroundColor() const
{
    const bool isHighlighted = isDisplayHintEnabled(EnteredHint) ||
        isDisplayHintEnabled(DraggedHint) ||
        isDisplayHintEnabled(PopupActiveHint);

    QColor foregroundColor = palette().color(foregroundRole());

    int alpha = 255;
    if (!isDisplayHintEnabled(ActivatedHint) && !isHighlighted) {
        alpha -= alpha / 4;
    }
    foregroundColor.setAlpha(alpha);

    return foregroundColor;
}

#include "kbreadcrumbbutton_p.moc"
