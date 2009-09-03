/*****************************************************************************
 * Copyright (C) 2006 by Peter Penz <peter.penz@gmx.at>                      *
 * Copyright (C) 2006 by Aaron J. Seigo <aseigo@kde.org>                     *
 * Copyright (C) 2007 by Kevin Ottens <ervin@kde.org>                        *
 * Copyright (C) 2007 by Urs Wolfer <uwolfer @ kde.org>                      *
 * Copyright (c) 2009 Eduardo Robles Elvira <edulix@gmail.com>               *
 * Copyright (c) 2009 Stephen Kelly <steveire@gmail.com>                     *
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

#include "kbreadcrumbnavigator.h"
#include "kbreadcrumbnavigatorbutton_p.h"

#include <kglobalsettings.h>
#include <kicon.h>
#include <klocale.h>
#include <kdebug.h>
#include <kmenu.h>

#include <QAbstractItemModel>
#include <QAbstractProxyModel>
#include <QMimeData>
#include <KUrl>
#include <QList>
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

class KBreadCrumbNavigator::Private
{
public:
    Private(KBreadCrumbNavigator *q);

    /**
     * Appends the widget at the end of the URL navigator. It is assured
     * that the filler widget remains as last widget to fill the remaining
     * width.
     */
    void appendWidget(QWidget *widget, int stretch = 0);

    /** Emits the signal urlsDropped(). */
    void dropMimeData(const QModelIndex &destination, QDropEvent *event);
    
    void slotCurrentIndexChanged();

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
    
    /**
     * Traverses the proxy models between m_selectionModel and m_model.
     * Creating a chain as it goes.
     */
    void createProxyChain();
    
    /**
     * Converts an index in the source model (m_model) to an index
     * in the selection model parent (m_selectionModel->model()).
     */
    QModelIndex sourceIndexToSelectionIndexParent(const QModelIndex &index) const;
    
public:
    QList<QAbstractProxyModel *> m_proxyChain;
    QAbstractItemModel *m_model;
    QItemSelectionModel *m_selectionModel;
    QHBoxLayout *m_layout;
    QLinkedList<KBreadCrumbNavigatorButton *> m_navButtons;
    KBreadCrumbNavigator *q;
};

KBreadCrumbNavigator::Private::Private(KBreadCrumbNavigator *q)
    :  m_model(0), m_selectionModel(0), m_layout(new QHBoxLayout), q(q)
{
    m_layout->setSpacing(0);
    m_layout->setMargin(0);
    m_layout->setAlignment(Qt::AlignLeft);

    q->setAutoFillBackground(false);
    q->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void KBreadCrumbNavigator::Private::createProxyChain()
{
    QAbstractItemModel *model = const_cast<QAbstractItemModel *>(m_selectionModel->model());
    QAbstractProxyModel *nextProxyModel;
    QAbstractProxyModel *proxyModel = qobject_cast<QAbstractProxyModel*>(model);

    QAbstractItemModel *rootModel;
    while (proxyModel) {

        if (proxyModel == m_model) {
            break;
        }

        m_proxyChain.prepend(proxyModel);

        nextProxyModel = qobject_cast<QAbstractProxyModel *>(proxyModel->sourceModel());

        if (!nextProxyModel) {
            rootModel = qobject_cast<QAbstractItemModel*>(proxyModel->sourceModel());
            // It's the final model in the chain, so it is necessarily m_model.
            Q_ASSERT(rootModel == m_model);
            break;
        }
        proxyModel = nextProxyModel;
    }
}


QModelIndex KBreadCrumbNavigator::Private::sourceIndexToSelectionIndexParent(const QModelIndex &index) const
{
    QModelIndex seekIndex = index;
    QListIterator<QAbstractProxyModel *> i(m_proxyChain);
    QAbstractProxyModel *proxy;
    while (i.hasNext()) {
        proxy = i.next();
        seekIndex = proxy->mapFromSource(seekIndex);
    }
    return seekIndex;
}

void KBreadCrumbNavigator::Private::appendWidget(QWidget *widget, int stretch)
{
    m_layout->insertWidget(m_layout->count(), widget, stretch);
}

void KBreadCrumbNavigator::Private::dropMimeData(const QModelIndex &destination, QDropEvent *event)
{
    bool match = q->haveCommonMimetypes(event->mimeData());
    if (match) {
        emit q->mimeDataDropped(destination, event);
    }
}

void KBreadCrumbNavigator::Private::updateButtons()
{
    if (!m_selectionModel || !m_selectionModel->currentIndex().isValid()) {
        return;
    }
    
    QLinkedList<KBreadCrumbNavigatorButton *>::iterator it = m_navButtons.begin();
    const QLinkedList<KBreadCrumbNavigatorButton *>::const_iterator itEnd = m_navButtons.end();
    bool createButton = false;

    // Get to the topmost parent model index
    QModelIndex index = m_selectionModel->currentIndex();
    QList<QModelIndex> indexes;
    while (index.parent().isValid()) {
        indexes.prepend(index);
        index = index.parent();
    }
    indexes.prepend(index);
    
    int count = indexes.size(), i = 1;
    kDebug() << count;
    
    foreach(index, indexes) {
        createButton = (it == itEnd);
        bool isLastButton = (i++ == count);

        const QString dirName = index.data().toString();
        kDebug() << dirName << (i - 1) << isLastButton;
        KBreadCrumbNavigatorButton* button = 0;
        if (createButton) {
            button = new KBreadCrumbNavigatorButton(index, q);
            connect(button, SIGNAL(mimeDataDropped(const QModelIndex &, QDropEvent *)),
                    q, SLOT(dropMimeData(const QModelIndex &, QDropEvent *)));
            appendWidget(button);
            // Don't show the button immediately, as setActive()
            // might change the size and a relayout gets triggered
            // after showing the button. So the showing of all buttons
            // is postponed until all buttons have the correct
            // activation state.
            button->setActive(isLastButton);
            m_navButtons.append(button);
        } else {
            button = *it;
            button->setIndex(index);
            button->setActive(isLastButton);
            it++;
        }
    }

    // delete buttons which are not used anymore
    QLinkedList<KBreadCrumbNavigatorButton *>::iterator itBegin = it;
    while (it != itEnd) {
        (*it)->hide();
        (*it)->deleteLater();
        ++it;
    }
    m_navButtons.erase(itBegin, m_navButtons.end());
    
    // all buttons have the correct activation state and
    // can be shown now
    foreach (KBreadCrumbNavigatorButton *button, m_navButtons) {
        button->show();
    }
    
    q->adjustSize();
}

void KBreadCrumbNavigator::Private::deleteButtons()
{
    foreach (KBreadCrumbNavigatorButton* button, m_navButtons) {
        button->hide();
        button->deleteLater();
    }
    m_navButtons.clear();
}


KBreadCrumbNavigator::KBreadCrumbNavigator(QWidget* parent)
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

KBreadCrumbNavigator::~KBreadCrumbNavigator()
{
    delete d;
}

void KBreadCrumbNavigator::setModel(QAbstractItemModel *model)
{
    d->m_model = model;
    d->updateButtons();
}

QAbstractItemModel *KBreadCrumbNavigator::model()
{
    return d->m_model;
}


void KBreadCrumbNavigator::setSelectionModel(QItemSelectionModel *selectionModel)
{
    d->m_selectionModel = selectionModel;
    connect(d->m_selectionModel, SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
        this, SLOT(slotCurrentIndexChanged()));
    d->updateButtons();
    d->createProxyChain();
}

QItemSelectionModel *KBreadCrumbNavigator::selectionModel()
{
    return d->m_selectionModel;
}

void KBreadCrumbNavigator::Private::slotCurrentIndexChanged()
{
    updateButtons();
}

void KBreadCrumbNavigator::setCurrentIndex(const QMimeData* mimeData)
{
    Q_UNUSED(mimeData);
}


QModelIndex KBreadCrumbNavigator::currentIndex()
{
    return d->sourceIndexToSelectionIndexParent(d->m_selectionModel->currentIndex());
}

void KBreadCrumbNavigator::currentChangedTriggered(const QModelIndex& index)
{
    kDebug();
    Q_ASSERT(d->m_selectionModel);
    d->m_selectionModel->setCurrentIndex(d->sourceIndexToSelectionIndexParent(index), 
        QItemSelectionModel::SelectCurrent);
}

bool KBreadCrumbNavigator::haveCommonMimetypes(const QMimeData *mimeData)
{
    Q_ASSERT(d->m_model);
    
    QStringList mimeTypes = mimeData->formats();
    foreach (QString mimeType, d->m_model->mimeTypes()) {
        if (mimeTypes.contains(mimeType)) {
            return true;
        }
    }
    
    return false;
}


void KBreadCrumbNavigator::mouseReleaseEvent(QMouseEvent *event)
{

    if (event->button() == Qt::MidButton) {
        QClipboard *clipboard = QApplication::clipboard();
        const QMimeData *mimeData = clipboard->mimeData();
        bool match = haveCommonMimetypes(mimeData);
        if (match) {
            setCurrentIndex(mimeData);
        }
    }
    QWidget::mouseReleaseEvent(event);
}

#include "kbreadcrumbnavigator.moc"
