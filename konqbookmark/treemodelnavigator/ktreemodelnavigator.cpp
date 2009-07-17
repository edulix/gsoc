/*****************************************************************************
 * Copyright (C) 2006 by Peter Penz <peter.penz@gmx.at>                      *
 * Copyright (C) 2006 by Aaron J. Seigo <aseigo@kde.org>                     *
 * Copyright (C) 2007 by Kevin Ottens <ervin@kde.org>                        *
 * Copyright (C) 2007 by Urs Wolfer <uwolfer @ kde.org>                      *
 * Copyright (c) 2009 Eduardo Robles Elvira <edulix@gmail.com>               *
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

#include "ktreemodelnavigator.h"
#include "ktreemodelnavigatorbutton_p.h"

#include <kglobalsettings.h>
#include <kicon.h>
#include <klocale.h>
#include <kdebug.h>
#include <kmenu.h>

#include <QAbstractItemModel>
#include <QMimeData>
#include <KUrl>
#include <QtCore/QLinkedList>
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QBoxLayout>
#include <QtGui/QClipboard>
#include <QtGui/QDropEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QPainter>
#include <QtGui/QStyleOption>

#include <fixx11h.h>

class KTreeModelNavigator::Private
{
public:
    Private(KTreeModelNavigator* q);

    /**
     * Appends the widget at the end of the URL navigator. It is assured
     * that the filler widget remains as last widget to fill the remaining
     * width.
     */
    void appendWidget(QWidget* widget, int stretch = 0);

    /** Emits the signal urlsDropped(). */
    void dropMimeData(const QModelIndex& destination, QDropEvent* event);

    /**
     * Updates all buttons to have one button for each part of the
     * path. Existing buttons, which are available by m_navButtons,
     * are reused if possible. If the path is longer, new buttons will be
     * created, if the path is shorter, the remaining buttons will be deleted.
     */
    void updateButtons();
    
    /**
     * Deletes all URL navigator buttons. m_navButtons is
     * empty after this operation.
     */
    void deleteButtons();
    
public:
    QModelIndex m_currentIndex;
    QAbstractItemModel* m_model;
    QHBoxLayout* m_layout;
    QLinkedList<KTreeModelNavigatorButton*> m_navButtons;
    KTreeModelNavigator* q;
};

KTreeModelNavigator::Private::Private(KTreeModelNavigator* q)
    :  m_model(0), m_layout(new QHBoxLayout), q(q)
{
    m_layout->setSpacing(0);
    m_layout->setMargin(0);
    m_layout->setAlignment(Qt::AlignLeft);

    q->setAutoFillBackground(false);
    q->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void KTreeModelNavigator::Private::appendWidget(QWidget* widget, int stretch)
{
    m_layout->insertWidget(m_layout->count(), widget, stretch);
}

void KTreeModelNavigator::Private::dropMimeData(const QModelIndex& destination, QDropEvent* event)
{
    bool match = q->haveCommonMimetypes(event->mimeData());
    if (match) {
        emit q->mimeDataDropped(destination, event);
    }
}

void KTreeModelNavigator::Private::updateButtons()
{
    if(!m_currentIndex.isValid())
        return;
    
    QLinkedList<KTreeModelNavigatorButton*>::iterator it = m_navButtons.begin();
    const QLinkedList<KTreeModelNavigatorButton*>::const_iterator itEnd = m_navButtons.end();
    bool createButton = false;

    // Get to the topmost parent model index
    QModelIndex& index = m_currentIndex;
    QList<QModelIndex> indexes;
    while(index.parent().isValid())
    {
        indexes.prepend(index);
        index = index.parent();
    }
    indexes.prepend(index);
    
    int count = indexes.size(), i = 1;
    kDebug() << count;
    
    foreach(index, indexes)
    {
        createButton = (it == itEnd);
        bool isLastButton = (i++ == count);

        const QString dirName = index.data().toString();
        kDebug() << dirName << (i - 1) << isLastButton;
        KTreeModelNavigatorButton* button = 0;
        if (createButton)
        {
            button = new KTreeModelNavigatorButton(index, q);
            connect(button, SIGNAL(mimeDataDropped(const QModelIndex&, QDropEvent*)),
                    q, SLOT(dropMimeData(const QModelIndex&, QDropEvent*)));
            appendWidget(button);
            // Don't show the button immediately, as setActive()
            // might change the size and a relayout gets triggered
            // after showing the button. So the showing of all buttons
            // is postponed until all buttons have the correct
            // activation state.
            button->setActive(isLastButton);
            m_navButtons.append(button);
        } else
        {
            button = *it;
            button->setIndex(index);
            button->setActive(isLastButton);
            it++;
        }
    }

    // delete buttons which are not used anymore
    QLinkedList<KTreeModelNavigatorButton*>::iterator itBegin = it;
    while (it != itEnd) {
        (*it)->hide();
        (*it)->deleteLater();
        ++it;
    }
    m_navButtons.erase(itBegin, m_navButtons.end());
    
    // all buttons have the correct activation state and
    // can be shown now
    foreach (KTreeModelNavigatorButton* button, m_navButtons) {
        button->show();
    }
    
    q->adjustSize();
}

void KTreeModelNavigator::Private::deleteButtons()
{
    foreach (KTreeModelNavigatorButton* button, m_navButtons) {
        button->hide();
        button->deleteLater();
    }
    m_navButtons.clear();
}


KTreeModelNavigator::KTreeModelNavigator(QWidget* parent)
    : QWidget(parent), d(new Private(this))
{
    setLayoutDirection(Qt::LeftToRight);
    
    const QFont font = KGlobalSettings::generalFont();
    setFont(font);

    setLayout(d->m_layout);
    setMinimumWidth(100);
    show();
    d->updateButtons();
}

KTreeModelNavigator::~KTreeModelNavigator()
{
    delete d;
}

void KTreeModelNavigator::setModel(QAbstractItemModel *model)
{
    d->m_model = model;
    d->updateButtons();
}

QAbstractItemModel *KTreeModelNavigator::model()
{
    return d->m_model;
}

void KTreeModelNavigator::currentChangedTriggered(const QModelIndex& index)
{
    setCurrentIndex(index);
    emit currentChanged(index);
}

void KTreeModelNavigator::setCurrentIndex(const QModelIndex& index)
{
    if(index == d->m_currentIndex)
        return;
    
    d->m_currentIndex = index;
    d->updateButtons();
}

void KTreeModelNavigator::setCurrentIndex(const QMimeData* mimeData)
{
    Q_UNUSED(mimeData);
}


bool KTreeModelNavigator::haveCommonMimetypes(const QMimeData* mimeData)
{
    Q_ASSERT(d->m_model);
    
    QStringList mimeTypes = mimeData->formats();
    foreach(QString mimeType, d->m_model->mimeTypes())
    {
        if(mimeTypes.contains(mimeType))
            return true;
    }
    
    return false;
}

QModelIndex KTreeModelNavigator::currentIndex() const
{
    return d->m_currentIndex;
}


void KTreeModelNavigator::mouseReleaseEvent(QMouseEvent* event)
{

    if (event->button() == Qt::MidButton) {
        QClipboard* clipboard = QApplication::clipboard();
        const QMimeData* mimeData = clipboard->mimeData();
        bool match = haveCommonMimetypes(mimeData);
        if (match) {
            setCurrentIndex(mimeData);
        }
    }
    QWidget::mouseReleaseEvent(event);
}

#include "ktreemodelnavigator.moc"
