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
#include "kcompletionview.h"
#include "placesmanager.h"
#include "klineeditview_p.h"
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

#include <kurlcompletion.h>
#include <kurl.h>
#include <klocale.h>

using namespace Konqueror;
using namespace Akonadi;

class LocationBar::Private
{
public:
    Private(LocationBar* parent);
    ~Private();
    
    LocationBar* q;
    KAggregatedModel* m_model;
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
    
    // FIXME for now, only one location bar is supported because of this
    // completion model. We just need to be able to create new completion models
    // that automatically connected to places manager, so it's quite easy.
    KCompletionModel* completionModel = PlacesManager::self()->urlCompletionModel();
//     connect(this, SIGNAL(textChanged(const QString&)),
//         completionModel->completion(), SLOT(slotMakeCompletion(const QString &)));
    
    PlacesProxyModel* placesProxyModel = new PlacesProxyModel(this);
    placesProxyModel->setSourceModel(PlacesManager::self()->bookmarkModel());
    connect(this, SIGNAL(textChanged(const QString&)),
        placesProxyModel, SLOT(setQuery(const QString &)));
        
    PlacesProxyModel* historyProxyModel = new PlacesProxyModel(this);
    placesProxyModel->setSourceModel(PlacesManager::self()->historyEntriesModel());
        
    KAggregatedModel* aggregatedModel = new KAggregatedModel(this);
    aggregatedModel->addSourceModel(completionModel);
    aggregatedModel->addSourceModel(historyProxyModel);
    aggregatedModel->addSourceModel(placesProxyModel);
    
    completionView()->setModel(aggregatedModel);
    
    KLineEditViewButton* tempButton = new KLineEditViewButton(this);
    tempButton->setCursor(Qt::ArrowCursor);
    tempButton->setToolTip(i18nc("@action:button Clear current text in the line edit", "Clear text"));
    tempButton->setPixmap(SmallIcon("bookmarks"));
    addWidget(tempButton, RightSide);
}

LocationBar::~LocationBar()
{
    
}

#include "locationbar.moc"
