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
#include "placesmanager.h"
#include "itemmodels/kcompletionmodel.h"
#include "itemmodels/konqbookmarkmodel.h"
#include "itemmodels/kaggregatedmodel.h"
#include "itemmodels/placesproxymodel.h"

#include <QAbstractItemView>
#include <QTreeView>
#include <QStringListModel>
#include <QStringList>

#include <kurlcompletion.h>
#include <kurl.h>

using namespace Konqueror;
using namespace Akonadi;

class LocationBar::Private
{
public:
    Private(LocationBar* parent);
    ~Private();
    
    void slotTextChanged(const QString& text);
    
    LocationBar* q;
    KAggregatedModel* m_model;
    QAbstractItemView* m_view;
};

LocationBar::Private::Private(LocationBar* parent)
    : q(parent)
{
    KCompletionModel *completionModel = PlacesManager::self()->urlCompletionModel();
//     connect(q, SIGNAL(textChanged(const QString&)),
//         completionModel, SLOT(slotMakeCompletion(const QString &)));
    
    PlacesProxyModel* placesProxyModel = new PlacesProxyModel(q);
    placesProxyModel->setSourceModel(PlacesManager::self()->bookmarkModel());
    connect(q, SIGNAL(textChanged(const QString&)),
        placesProxyModel, SLOT(setQuery(const QString &)));
        
    m_model = new KAggregatedModel(q);
    m_model->addSourceModel(placesProxyModel);
    m_model->addSourceModel(completionModel);
    
//     m_view = new QTreeView(q);
//     QStringListModel *stringModel = new QStringListModel(q);
//     stringModel->setStringList(QStringList() << "a" << "b" << "c");
//     q->setView(m_view);
//     q->setCompletionObject(completionModel->completion());
}

LocationBar::Private::~Private()
{
    
}

LocationBar::LocationBar(QWidget* parent)
    : KComboBox(parent), d(new Private(this))
{
    
}

LocationBar::~LocationBar()
{
    
}

#include "locationbar.moc"
