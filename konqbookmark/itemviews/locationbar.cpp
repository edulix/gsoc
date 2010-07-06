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

#include "locationbar.h"
#include "locationbarfaviconwidget.h"
#include "locationbardelegate.h"
#include "kcompletionview.h"
#include "placesmanager.h"
#include "klineeditviewbutton.h"
#include "itemmodels/kcompletionmodel.h"
#include "itemmodels/konqbookmarkmodel.h"
#include "itemmodels/kaggregatedmodel.h"
#include "itemmodels/placesproxymodel.h"

#include <QtGui/QAbstractItemView>
#include <QtCore/QAbstractItemModel>
#include <QtGui/QTreeView>
#include <QtGui/QStringListModel>
#include <QtCore/QStringList>
#include <QtGui/QWidget>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QCompleter>
#include <QtGui/QApplication>

#include <KUrlCompletion>
#include <KUrl>
#include <KLocale>
#include <KIconLoader>

using namespace Konqueror;
using namespace Akonadi;

class LocationBar::Private
{
public:
    Private(LocationBar *parent);
    ~Private();

    void sort();

    void slotReturnPressed(const QString &text);
    void slotCompletionActivated(const QModelIndex &index);

    LocationBar *q;
    PlacesProxyModel *m_unsortedModel;
    LocationBarCompletionModel *m_model;
    QAbstractItemView *m_view;
};

LocationBar::Private::Private(LocationBar *parent)
    : q(parent)
{
}

LocationBar::Private::~Private()
{

}

void LocationBar::Private::slotReturnPressed(const QString &text)
{
    kDebug() << text;
    emit q->returnPressed(text, qApp->keyboardModifiers());
}

void LocationBar::Private::slotCompletionActivated(const QModelIndex& index)
{
    kDebug() << index.data().toString();
    emit q->returnPressed(index.data().toString(), qApp->keyboardModifiers());
}

LocationBar::LocationBar(QWidget *parent)
    : KLineEditView(parent), d(new Private(this))
{
    init();
}

void LocationBar::init()
{
    setCompletionMode(KGlobalSettings::CompletionPopup);
    setClearButtonShown(true);
    setClickMessage(i18n("Search Bookmarks and History"));
    completionView()->setItemDelegate(new LocationBarDelegate(this));

    // Setting up models
    d->m_unsortedModel = new PlacesProxyModel(this);
    d->m_model = new LocationBarCompletionModel(d->m_unsortedModel, this);
    connect(this, SIGNAL(textChanged(const QString &)),
        d->m_unsortedModel, SLOT(setQuery(const QString &)));


    completionView()->setModel(d->m_model);
    connect(completionView(), SIGNAL(activated(QModelIndex)), this,
        SLOT(slotCompletionActivated(QModelIndex)));

    connect(this, SIGNAL(returnPressed(const QString &)),
        this, SLOT(slotReturnPressed(const QString &)));

    addWidget(new LocationBarFaviconWidget(this), LeftSide);
}

LocationBar::~LocationBar()
{

}

void LocationBar::setURL(const QString &url)
{
    //TODO: see konqcombo.cpp KonqCombo::setURL() for ideas
    setText(url);
}

#include "locationbar.moc"
