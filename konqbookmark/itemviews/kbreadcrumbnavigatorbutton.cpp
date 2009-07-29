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

#include "kbreadcrumbnavigatorbutton_p.h"

#include "kbreadcrumbnavigator.h"
#include "kbreadcrumbnavigatormenu_p.h"

#include <kglobalsettings.h>
#include <kstringhandler.h>

#include <QtCore/QTimer>
#include <QtGui/QPainter>
#include <QtGui/QKeyEvent>
#include <QtGui/QStyleOption>
#include <QModelIndex>

QPointer<KBreadCrumbNavigatorMenu> KBreadCrumbNavigatorButton::m_childItemsMenu;

KBreadCrumbNavigatorButton::KBreadCrumbNavigatorButton(QModelIndex index, KBreadCrumbNavigator* parent) :
    KBreadCrumbButton(parent),
    m_index(index),
    m_hoverArrow(false),
    m_popupDelay(0)
{
    setAcceptDrops(true);
    setIndex(index);
    setMouseTracking(true);
    connect(this, SIGNAL(clicked()), this, SLOT(updateNavigatorCurrentIndex()));
    
    m_popupDelay = new QTimer(this);
    m_popupDelay->setSingleShot(true);
    connect(m_popupDelay, SIGNAL(timeout()), this, SLOT(listChildItems()));
    connect(this, SIGNAL(pressed()), this, SLOT(startPopupDelay()));
}

KBreadCrumbNavigatorButton::~KBreadCrumbNavigatorButton()
{
}

void KBreadCrumbNavigatorButton::setIndex(QModelIndex index)
{
    m_index = index;
    setText(index.data().toString());
    updateMinimumWidth();
}

void KBreadCrumbNavigatorButton::setActive(bool active)
{   
    QFont adjustedFont(font());
    if (active) {
        setDisplayHintEnabled(ActivatedHint, true);
        adjustedFont.setBold(true);
    } else {
        setDisplayHintEnabled(ActivatedHint, false);
        adjustedFont.setBold(false);
    }

    setFont(adjustedFont);
    updateMinimumWidth();
    update();
}

bool KBreadCrumbNavigatorButton::isActive() const
{
    return isDisplayHintEnabled(ActivatedHint);
}

QSize KBreadCrumbNavigatorButton::sizeHint() const
{
    // the minimum size is textWidth + arrowWidth() + 2 * BorderWidth; for the
    // preferred size we add the BorderWidth 2 times again for having an uncluttered look
    const int width = fontMetrics().width(text()) + arrowWidth() + 4 * BorderWidth;
    return QSize(width, KBreadCrumbButton::sizeHint().height());
}

void KBreadCrumbNavigatorButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    int buttonWidth  = width();
    int preferredWidth = sizeHint().width();
    if (preferredWidth < minimumWidth()) {
        preferredWidth = minimumWidth();
    }
    if (buttonWidth > preferredWidth) {
        buttonWidth = preferredWidth;
    }
    const int buttonHeight = height();

    const QColor fgColor = foregroundColor();
    drawHoverBackground(&painter);

    int textLeft = 0;
    int textWidth = buttonWidth;

    const bool leftToRight = (layoutDirection() == Qt::LeftToRight);

    if (!isDisplayHintEnabled(ActivatedHint)) {
        // draw arrow
        const int arrowSize = arrowWidth();
        const int arrowX = leftToRight ? (buttonWidth - arrowSize) - BorderWidth : BorderWidth;
        const int arrowY = (buttonHeight - arrowSize) / 2;

        QStyleOption option;
        option.initFrom(this);
        option.rect = QRect(arrowX, arrowY, arrowSize, arrowSize);
        option.palette = palette();
        option.palette.setColor(QPalette::Text, fgColor);
        option.palette.setColor(QPalette::WindowText, fgColor);
        option.palette.setColor(QPalette::ButtonText, fgColor);

        if (m_hoverArrow) {
            // highlight the background of the arrow to indicate that the directories
            // popup can be opened by a mouse click
            QColor hoverColor = palette().color(QPalette::HighlightedText);
            hoverColor.setAlpha(96);
            painter.setPen(Qt::NoPen);
            painter.setBrush(hoverColor);

            int hoverX = arrowX;
            if (!leftToRight) {
                hoverX -= BorderWidth;
            }
            painter.drawRect(QRect(hoverX, 0, arrowSize + BorderWidth, buttonHeight));
        }

        if (leftToRight) {
            style()->drawPrimitive(QStyle::PE_IndicatorArrowRight, &option, &painter, this);
        } else {
            style()->drawPrimitive(QStyle::PE_IndicatorArrowLeft, &option, &painter, this);
            textLeft += arrowSize + 2 * BorderWidth;
        }

        textWidth -= arrowSize + 2 * BorderWidth;
    }

    painter.setPen(fgColor);
    const bool clipped = isTextClipped();
    const int align = clipped ? Qt::AlignVCenter : Qt::AlignCenter;
    const QRect textRect(textLeft, 0, textWidth, buttonHeight);
    if (clipped) {
        QColor bgColor = fgColor;
        bgColor.setAlpha(0);
        QLinearGradient gradient(textRect.topLeft(), textRect.topRight());
        if (leftToRight) {
            gradient.setColorAt(0.8, fgColor);
            gradient.setColorAt(1.0, bgColor);
        } else {
            gradient.setColorAt(0.0, bgColor);
            gradient.setColorAt(0.2, fgColor);
        }

        QPen pen;
        pen.setBrush(QBrush(gradient));
        painter.setPen(pen);
    }
    painter.drawText(textRect, align, text());
}

void KBreadCrumbNavigatorButton::enterEvent(QEvent* event)
{
    KBreadCrumbButton::enterEvent(event);

    // if the text is clipped due to a small window width, the text should
    // be shown as tooltip
    if (isTextClipped()) {
        setToolTip(text());
    }
}

void KBreadCrumbNavigatorButton::leaveEvent(QEvent* event)
{
    KBreadCrumbButton::leaveEvent(event);
    setToolTip(QString());

    if (m_hoverArrow) {
        m_hoverArrow = false;
        update();
    }
}

void KBreadCrumbNavigatorButton::dropEvent(QDropEvent* event)
{
    if (!m_index.isValid()) {
        return;
    }

    bool match = breadCrumbNavigator()->haveCommonMimetypes(event->mimeData());
    if (match) {
        setDisplayHintEnabled(DraggedHint, true);

        emit mimeDataDropped(m_index, event);

        setDisplayHintEnabled(DraggedHint, false);
        update();
    }
}

void KBreadCrumbNavigatorButton::dragEnterEvent(QDragEnterEvent* event)
{
    bool match = breadCrumbNavigator()->haveCommonMimetypes(event->mimeData());
    if (match) {
        setDisplayHintEnabled(DraggedHint, true);
        event->acceptProposedAction();

        update();
    }
}

void KBreadCrumbNavigatorButton::dragMoveEvent(QDragMoveEvent* event)
{
    QRect rect = event->answerRect();
    if (isAboveArrow(rect.center().x())) {
        m_hoverArrow = true;
        update();

        if (m_childItemsMenu == 0) {
            startPopupDelay();
        } else if (m_childItemsMenu->parent() != this) {
            m_childItemsMenu->close();
            m_childItemsMenu->deleteLater();
            m_childItemsMenu = 0;

            startPopupDelay();
        }
    } else {
        if (m_popupDelay->isActive()) {
            stopPopupDelay();
        }
        delete m_childItemsMenu;
        m_childItemsMenu = 0;
        m_hoverArrow = false;
        update();
    }
}

void KBreadCrumbNavigatorButton::dragLeaveEvent(QDragLeaveEvent* event)
{
    KBreadCrumbButton::dragLeaveEvent(event);

    m_hoverArrow = false;
    setDisplayHintEnabled(DraggedHint, false);
    update();
}

void KBreadCrumbNavigatorButton::mousePressEvent(QMouseEvent* event)
{
    if (isAboveArrow(event->x()) && (event->button() == Qt::LeftButton)) {
        listChildItems();
    } else {
        // the mouse is pressed above the text area
        KBreadCrumbButton::mousePressEvent(event);
    }
}

void KBreadCrumbNavigatorButton::mouseReleaseEvent(QMouseEvent* event)
{
    if (!isAboveArrow(event->x()) || (event->button() != Qt::LeftButton)) {
        // the mouse is released above the text area
        KBreadCrumbButton::mouseReleaseEvent(event);
    }
}

void KBreadCrumbNavigatorButton::mouseMoveEvent(QMouseEvent* event)
{
    KBreadCrumbButton::mouseMoveEvent(event);

    const bool hoverArrow = isAboveArrow(event->x());
    if (hoverArrow != m_hoverArrow) {
        m_hoverArrow = hoverArrow;
        update();
    }
}

void KBreadCrumbNavigatorButton::updateNavigatorCurrentIndex()
{
    stopPopupDelay();

    if (!m_index.isValid()) {
        return;
    }

    breadCrumbNavigator()->currentChangedTriggered(m_index);
}

void KBreadCrumbNavigatorButton::startPopupDelay()
{
    if (m_popupDelay->isActive() || !m_index.isValid()) {
        return;
    }

    m_popupDelay->start(300);
}

void KBreadCrumbNavigatorButton::stopPopupDelay()
{
    m_popupDelay->stop();
}

void KBreadCrumbNavigatorButton::mimeDataDropped(QAction* action, QDropEvent* event)
{
    const int result = action->data().toInt();
    emit mimeDataDropped(m_childItems.at(result), event);
}

/**
 * Helper function for listJobFinished
 */
static bool naturalLessThan(const QModelIndex& indexLeft, const QModelIndex& indexRight)
{
    QString s1 = indexLeft.data().toString();
    QString s2 = indexRight.data().toString();
    return KStringHandler::naturalCompare(s1, s2, Qt::CaseInsensitive) < 0;
}

/// Helper function for listChildItems()
static QModelIndex getSelectedChildItem(const QModelIndex& navigatorIndex, const QModelIndex& buttonIndex)
{
    QModelIndex index = navigatorIndex;
    while(index.parent().isValid())
    {
        if(index.parent() == buttonIndex)
            return index;
        index = index.parent();
    }
    return QModelIndex();
}

void KBreadCrumbNavigatorButton::listChildItems()
{
    if (!m_index.isValid()) {
        return;
    }
    
    m_childItems.clear();
    int count = breadCrumbNavigator()->model()->rowCount(m_index);
    for(int i = 0; i < count; i++)
    {
        m_childItems.append(m_index.child(i, 0));
    }
    
    qSort(m_childItems.begin(), m_childItems.end(), naturalLessThan);
    setDisplayHintEnabled(PopupActiveHint, true);
    update(); // ensure the button is drawn highlighted

    if (m_childItemsMenu != 0) {
        m_childItemsMenu->close();
        m_childItemsMenu->deleteLater();
        m_childItemsMenu = 0;
    }

    m_childItemsMenu = new KBreadCrumbNavigatorMenu(breadCrumbNavigator(), this);
    connect(m_childItemsMenu, SIGNAL(mimeDataDropped(QAction*, QDropEvent*)),
            this, SLOT(mimeDataDropped(QAction*, QDropEvent*)));

    m_childItemsMenu->setLayoutDirection(Qt::LeftToRight);
    int i = 0;
    QModelIndex selectedChildItem = getSelectedChildItem(breadCrumbNavigator()->currentIndex(), m_index);
    foreach (const QModelIndex& childIndex, m_childItems)
    {
        QString text = KStringHandler::csqueeze(childIndex.data().toString(), 60);
        text.replace('&', "&&");
        QAction* action = new QAction(text, this);
        if (selectedChildItem == childIndex) {
            QFont font(action->font());
            font.setBold(true);
            action->setFont(font);
        }
        action->setData(i++);
        m_childItemsMenu->addAction(action);

        if (i > 50) {
            // Opening a menu with more than 50 items makes no sense from
            // a usability view. Also there are implementation issues in
            // QMenu if the number of menu items don't fit into the available
            // screen -> skip remaining items
            QAction* limitReached = new QAction("...", this);
            limitReached->setEnabled(false);
            m_childItemsMenu->addAction(limitReached);
            break;
        }
    }

    const bool leftToRight = (layoutDirection() == Qt::LeftToRight);
    const int popupX = leftToRight ? width() - arrowWidth() - BorderWidth : 0;
    const QPoint popupPos  = breadCrumbNavigator()->mapToGlobal(geometry().bottomLeft() + QPoint(popupX, 0));

    const QAction* action = m_childItemsMenu->exec(popupPos);
    if (action != 0) {
        const int result = action->data().toInt();
        breadCrumbNavigator()->currentChangedTriggered(m_childItems.at(result));
    }

    m_childItems.clear();
    delete m_childItemsMenu;
    m_childItemsMenu = 0;

    setDisplayHintEnabled(PopupActiveHint, false);
}

int KBreadCrumbNavigatorButton::arrowWidth() const
{
    // if there isn't arrow then return 0
    int width = 0;
    if (!isDisplayHintEnabled(ActivatedHint)) {
        width = height() / 2;
        if (width < 4) {
            width = 4;
        }
    }

    return width;
}

bool KBreadCrumbNavigatorButton::isAboveArrow(int x) const
{
    const bool leftToRight = (layoutDirection() == Qt::LeftToRight);
    return leftToRight ? (x >= width() - arrowWidth()) : (x < arrowWidth());
}

bool KBreadCrumbNavigatorButton::isTextClipped() const
{
    int availableWidth = width() - 2 * BorderWidth;
    if (!isDisplayHintEnabled(ActivatedHint)) {
        availableWidth -= arrowWidth() - BorderWidth;
    }

    QFontMetrics fontMetrics(font());
    return fontMetrics.width(text()) >= availableWidth;
}

void KBreadCrumbNavigatorButton::updateMinimumWidth()
{
    const int oldMinWidth = minimumWidth();

    int minWidth = sizeHint().width();
    if (minWidth < 40) {
        minWidth = 40;
    }
    else if (minWidth > 150) {
        // don't let an overlong path name waste all the URL navigator space
        minWidth = 150;
    }
    if (oldMinWidth != minWidth) {
        setMinimumWidth(minWidth);
    }
}

#include "kbreadcrumbnavigatorbutton_p.moc"
