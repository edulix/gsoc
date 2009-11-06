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

#include <QAbstractItemView>
#include <QAbstractItemModel>
#include <QTreeView>
#include <QStringListModel>
#include <QStringList>
#include <QWidget>
#include <QSortFilterProxyModel>
#include <QCompleter>

#include <kurlcompletion.h>
#include <kurl.h>
#include <klocale.h>
#include <kiconloader.h>

using namespace Konqueror;
using namespace Akonadi;

class LocationBar::Private
{
public:
    Private(LocationBar* parent);
    ~Private();

    void sort();
    
    LocationBar* q;
    PlacesProxyModel* m_unsortedModel;
    LocationBarCompletionModel* m_model;
    QAbstractItemView* m_view;
};

LocationBar::Private::Private(LocationBar* parent)
    : q(parent)
{
}

LocationBar::Private::~Private()
{
    
}

LocationBar::LocationBar(QWidget* parent)
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
    
    d->m_unsortedModel = new PlacesProxyModel(this);
    d->m_model = new LocationBarCompletionModel(d->m_unsortedModel, this);

    connect(this, SIGNAL(textChanged(const QString&)),
        d->m_unsortedModel, SLOT(setQuery(const QString &)));
    completionView()->setModel(d->m_model);
    
    addWidget(new LocationBarFaviconWidget(this), LeftSide);
}

LocationBar::~LocationBar()
{
    
}

#include "locationbar.moc"
