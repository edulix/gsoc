/* This file is part of the KDE libraries

   Copyright (c) 2000,2001,2002 Carsten Pfeiffer <pfeiffer@kde.org>
   Copyright (c) 2000 Stefan Schimanski <1Stein@gmx.de>
   Copyright (c) 2000,2001,2002,2003,2004 Dawit Alemayehu <adawit@kde.org>
   Copyright (c) 2009 Eduardo Robles Elvira <edulix@gmail.com>

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


#include "kcompletionview.h"
#include <klineedit.h>

#include <QtCore/QEvent>
#include <QtGui/QApplication>
#include <QtGui/QComboBox>
#include <QtGui/QStyle>
#include <QtGui/QScrollBar>
#include <QtGui/QKeyEvent>

#include <kdebug.h>
#include <kconfig.h>
#include <kglobalsettings.h>

class KCompletionView::Private
{
public:
    QWidget *m_parent; // necessary to set the focus back
    QString cancelText;
    bool tabHandling : 1;
    bool upwardBox : 1;
};

KCompletionView::KCompletionView(QWidget *parent)
 : QListView(parent), d(new Private)
{
    d->m_parent        = parent;
    d->tabHandling     = true;
    d->upwardBox       = false;

    setWindowFlags(Qt::ToolTip); // calls setVisible, so must be done after initializations

    setLineWidth(1);
    setFrameStyle(QFrame::Box | QFrame::Plain);

    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(this, SIGNAL(activated(const QModelIndex&)),
             SLOT(slotActivated(const QModelIndex&)));
}

KCompletionView::~KCompletionView()
{
    d->m_parent = 0L;
    delete d;
}

void KCompletionView::slotActivated(const QModelIndex& index)
{
    if(!index.isValid()) {
        return;
    }

    hide();
}

bool KCompletionView::eventFilter(QObject* o, QEvent* e)
{
    int type = e->type();
    QWidget *wid = qobject_cast<QWidget*>(o);

    if(o == this) {
        return false;
    }

    if(wid && (wid == d->m_parent || wid->windowFlags() & Qt::Window) &&
        (type == QEvent::Move || type == QEvent::Resize)) {
        hide();
        return false;
    }

    if(type == QEvent::MouseButtonPress && (wid && !isAncestorOf(wid))) {
        hide();
        e->accept();
        return true;
    }

    if(wid && wid->isAncestorOf(d->m_parent) && isVisible()) {
        if(type == QEvent::KeyPress) {
            QKeyEvent *ev = static_cast<QKeyEvent *>(e);
            switch(ev->key()) {
            case Qt::Key_Backtab:
                if(d->tabHandling && (ev->modifiers() == Qt::NoButton ||
                    (ev->modifiers() & Qt::ShiftModifier))) {
                    up();
                    ev->accept();
                    return true;
                }
                break;
            case Qt::Key_Tab:
                if(d->tabHandling && (ev->modifiers() == Qt::NoButton)) {
                    down();
                    // #65877: Key_Tab should complete using the first
                    // (or selected) item, and then offer completions again
                    //TODO change when Location Bar is implemented
                    if(model() && model()->rowCount(QModelIndex()) == 1) {
                        KLineEdit* parent = qobject_cast<KLineEdit*>(d->m_parent);
                        if(parent) {
                            parent->doCompletion(currentIndex().data().toString());
                        } else {
                            hide();
                        }
                    }
                    ev->accept();
                    return true;
                }
                break;
            case Qt::Key_Down:
                down();
                ev->accept();
                return true;
            case Qt::Key_Up:
                // If there is no selected item and we've popped up above
                // our parent, select the first item when they press up.
                if(!currentIndex().isValid() ||
                    mapToGlobal(QPoint(0, 0)).y() >
                    d->m_parent->mapToGlobal(QPoint(0, 0)).y()) {
                    up();
                } else {
                    down();
                }
                ev->accept();
                return true;
            case Qt::Key_PageUp:
                pageUp();
                ev->accept();
                return true;
            case Qt::Key_PageDown:
                pageDown();
                ev->accept();
                return true;
            case Qt::Key_Escape:
                canceled();
                ev->accept();
                return true;
            case Qt::Key_Enter:
            case Qt::Key_Return:
                if(ev->modifiers() & Qt::ShiftModifier) {
                    hide();
                    ev->accept();  // Consume the Enter event
                    return true;
                }
                break;
            case Qt::Key_End:
                if(ev->modifiers() & Qt::ControlModifier)
                {
                    end();
                    ev->accept();
                    return true;
                }
                break;
            case Qt::Key_Home:
                if(ev->modifiers() & Qt::ControlModifier)
                {
                    home();
                    ev->accept();
                    return true;
                }
            default:
                break;
            }
        } else if(type == QEvent::ShortcutOverride) {
            // Override any accelerators that match
            // the key sequences we use here...
            QKeyEvent *ev = static_cast<QKeyEvent *>( e );
            switch(ev->key()) {
            case Qt::Key_Down:
            case Qt::Key_Up:
            case Qt::Key_PageUp:
            case Qt::Key_PageDown:
            case Qt::Key_Escape:
            case Qt::Key_Enter:
            case Qt::Key_Return:
                ev->accept();
                return true;
                break;
            case Qt::Key_Tab:
            case Qt::Key_Backtab:
                if(ev->modifiers() == Qt::NoButton ||
                     (ev->modifiers() & Qt::ShiftModifier))
                {
                    ev->accept();
                    return true;
                }
                break;
            case Qt::Key_Home:
            case Qt::Key_End:
                if(ev->modifiers() & Qt::ControlModifier)
                {
                    ev->accept();
                    return true;
                }
                break;
            default:
                break;
            }
        } else if(type == QEvent::FocusOut) {
            QFocusEvent* event = static_cast<QFocusEvent*>(e);
            if(event->reason() != Qt::PopupFocusReason
#ifdef Q_WS_WIN
                && (event->reason() != Qt::ActiveWindowFocusReason || QApplication::activeWindow() != this)
#endif
                )
                hide();
        }
    }

    return QListView::eventFilter(o, e);
}

void KCompletionView::popup()
{
    if (model() && !model()->hasChildren()) {
        hide();
    } else {
        //TODO KDE 4 - Port: ensureCurrentVisible();
        bool block = signalsBlocked();
        blockSignals(true);
        setCurrentIndex(QModelIndex());
        blockSignals(block);
        clearSelection();
        if(!isVisible()) {
            show();
        } else if(size().height() != sizeHint().height()) {
            sizeAndPosition();
        }
    }
}

void KCompletionView::sizeAndPosition()
{
    int currentGeom = height();
    QPoint currentPos = pos();
    QRect geom = calculateGeometry();
    resize(geom.size());

    int x = currentPos.x(), y = currentPos.y();
    if(d->m_parent) {
      if(!isVisible()) {
        QRect screenSize = KGlobalSettings::desktopGeometry(d->m_parent);

        QPoint orig = globalPositionHint();
        x = orig.x() + geom.x();
        y = orig.y() + geom.y();

        if(x + width() > screenSize.right())
            x = screenSize.right() - width();
        if(y + height() > screenSize.bottom()) {
            y = y - height() - d->m_parent->height();
            d->upwardBox = true;
        }
      }
      else {
        // Are we above our parent? If so we must keep bottom edge anchored.
        if(d->upwardBox) {
          y += (currentGeom - height());
        }
      }
      move(x, y);
    }
}

QPoint KCompletionView::globalPositionHint() const
{
    if (!d->m_parent)
        return QPoint();
    return d->m_parent->mapToGlobal(QPoint(0, d->m_parent->height()));
}

void KCompletionView::setVisible(bool visible)
{
    if (visible) {
        d->upwardBox = false;
        if (d->m_parent) {
            sizeAndPosition();
            qApp->installEventFilter(this);
        }

        // ### we shouldn't need to call this, but without this, the scrollbars
        // are pretty b0rked.
        //triggerUpdate( true );

        // Workaround for I'm not sure whose bug - if this KCompletionView' parent
        // is in a layout, that layout will detect inserting new child (posted
        // ChildInserted event), and will trigger relayout (post LayoutHint event).
        // QWidget::show() sends also posted ChildInserted events for the parent,
        // and later all LayoutHint events, which causes layout updating.
        // The problem is, KCompletionView::eventFilter() detects resizing
        // of the parent, and calls hide() - and this hide() happen in the middle
        // of show(), causing inconsistent state. I'll try to submit a Qt patch too.
        qApp->sendPostedEvents();
    } else {
        if(d->m_parent) {
            qApp->removeEventFilter(this);
        }
        d->cancelText.clear();
    }

    QListView::setVisible(visible);
}

QRect KCompletionView::calculateGeometry() const
{
    QRect visualRect;
    if(!model() || model()->rowCount(QModelIndex()) == 0 ||
        !(visualRect = rectForIndex(model()->index(0,0))).isValid())
        return QRect();

    int x = 0, y = 0;
    int ih = visualRect.height();
    int h = qMin(15 * ih, (int) model()->rowCount(QModelIndex()) * ih) + 2 * frameWidth();

    int w = (d->m_parent) ? d->m_parent->width() : QListView::minimumSizeHint().width();
    w = qMax( QListView::minimumSizeHint().width(), w );

  //### M.O.: Qt4 doesn't actually honor SC_ComboBoxListBoxPopup ???
#if 0
    //If we're inside a combox, Qt by default makes the dropdown
    // as wide as the combo, and gives the style a chance
    // to adjust it. Do that here as well, for consistency
    const QObject* combo;
    if ( d->m_parent && (combo = d->m_parent->parent() ) &&
        qobject_cast<QComboBox*>(combo) )
    {
        const QComboBox* cb = static_cast<const QComboBox*>(combo);

        //Expand to the combo width
        w = qMax( w, cb->width() );

        QPoint parentCorner = d->m_parent->mapToGlobal(QPoint(0, 0));
        QPoint comboCorner  = cb->mapToGlobal(QPoint(0, 0));

        //We need to adjust our horizontal position to also be WRT to the combo
        x += comboCorner.x() -  parentCorner.x();

        //The same with vertical one
        y += cb->height() - d->m_parent->height() +
             comboCorner.y() - parentCorner.y();

        //Ask the style to refine this a bit
        QRect styleAdj = style().querySubControlMetrics(QStyle::CC_ComboBox,
                                    cb, QStyle::SC_ComboBoxListBoxPopup,
                                    QStyleOption(x, y, w, h));
        //QCommonStyle returns QRect() by default, so this is what we get if the
        //style doesn't implement this
        if (!styleAdj.isNull())
            return styleAdj;

    }
#endif
    return QRect(x, y, w, h);
}

QSize KCompletionView::sizeHint() const
{
    return calculateGeometry().size();
}

void KCompletionView::down()
{
    if(!model() || !model()->hasChildren()) {
        return;
    }
    
    if(selectionModel()->selectedIndexes().empty()) {
        selectionModel()->select(model()->index(0,0), QItemSelectionModel::SelectCurrent);
        return;
    }
    
    QModelIndex current = selectionModel()->selectedIndexes().first();
    if(current.row() + 1 < model()->rowCount()) {
        selectionModel()->select(model()->index(current.row() + 1, 0), QItemSelectionModel::SelectCurrent);
    } else {
        selectionModel()->select(model()->index(0,0), QItemSelectionModel::SelectCurrent);
    }
}

void KCompletionView::up()
{
    if(!model() || !model()->hasChildren()) {
        return;
    }
    
    if(selectionModel()->selectedIndexes().empty()) {
        selectionModel()->select(model()->index(model()->rowCount() - 1, 0), QItemSelectionModel::SelectCurrent);
        return;
    }
    
    QModelIndex current = selectionModel()->selectedIndexes().first();
    if(current.row() > 0) {
        selectionModel()->select(model()->index(current.row() - 1, 0), QItemSelectionModel::SelectCurrent);
    } else {
        selectionModel()->select(model()->index(model()->rowCount() - 1,0), QItemSelectionModel::SelectCurrent);
    }
    
    if(!model() || !model()->hasChildren()) {
        return;
    }
}

void KCompletionView::pageDown()
{
    //int i = currentItem() + numItemsVisible();
    //i = i > (int)count() - 1 ? (int)count() - 1 : i;
    //setCurrentRow( i );
    moveCursor(QAbstractItemView::MovePageDown , Qt::NoModifier);
}

void KCompletionView::pageUp()
{
    //int i = currentItem() - numItemsVisible();
    //i = i < 0 ? 0 : i;
    //setCurrentRow( i );

    moveCursor(QAbstractItemView::MovePageUp , Qt::NoModifier);
}

void KCompletionView::home()
{
    setCurrentIndex( QModelIndex() );
}

void KCompletionView::end()
{
    if(!model()) {
        return;
    }
    
    setCurrentIndex(model()->index(model()->rowCount(QModelIndex()) - 1, 0));
}

void KCompletionView::setTabHandling(bool enable)
{
    d->tabHandling = enable;
}

bool KCompletionView::isTabHandling() const
{
    return d->tabHandling;
}

void KCompletionView::setCancelledText(const QString& text)
{
    d->cancelText = text;
    if(d->cancelText.isEmpty()) {
        kDebug() << "set empty";
    }
}

QString KCompletionView::cancelledText() const
{
    return d->cancelText;
}

void KCompletionView::canceled()
{
    if(!d->cancelText.isNull()) {
        emit userCancelled( d->cancelText );
    }
    
    if(isVisible()) {
        hide();
    }
}

#include "kcompletionview.moc"
