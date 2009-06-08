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

#include "konqbookmarkview.h"
#include "konqbookmarkmodel.h"
#include "konqbookmarksettings.h"

#include <QHeaderView>
#include <QPoint>
#include <QAction>
#include <QString>
#include <QStringList>

#include <kmenu.h>
#include <klocale.h>

using namespace Akonadi;

KonqBookmarkView::KonqBookmarkView( QWidget *parent )
  : ItemView( parent )
{
    QHeaderView* headerView = header();
    headerView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(headerView, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(configureSettings(const QPoint&)));
}


void KonqBookmarkView::setModel(QAbstractItemModel *model)
{
    ItemView::setModel(model);
    
    QStringList shownColumns = KonqBookmarkSettings::shownColumns();
        
    for(int i = KonqBookmarkModel::Url; i <= KonqBookmarkModel::LastVisited; ++i)
    {
        const QString columnName = model->headerData(i, Qt::Horizontal).toString();
        bool show = shownColumns.contains(columnName);
        setColumnHidden(i, !show);
    }
}

KonqBookmarkView::~KonqBookmarkView()
{
    KonqBookmarkSettings::self()->writeConfig();
}

void KonqBookmarkView::configureSettings(const QPoint& pos)
{
    KMenu popup(this);
    popup.addTitle(i18nc("@title:menu", "Columns"));

    // add checkbox items for each column
    QHeaderView* headerView = header();
    
    for (int i = KonqBookmarkModel::Url; i <= KonqBookmarkModel::LastVisited; ++i)
    {
        const QString text = model()->headerData(i, Qt::Horizontal).toString();
        QAction* action = popup.addAction(text);
        action->setCheckable(true);
        action->setChecked(!headerView->isSectionHidden(i));
        action->setData(i);
    }
    popup.addSeparator();

    QAction* activatedAction = popup.exec(headerView->mapToGlobal(pos));
    if (activatedAction != 0) {
        const bool show = activatedAction->isChecked();
        const int columnIndex = activatedAction->data().toInt();

        QStringList shownColumns = KonqBookmarkSettings::shownColumns();
        QString colName = activatedAction->text().remove(QChar('&'));
        if(show)
            shownColumns.append(colName);
        else
            shownColumns.removeOne(colName);
        
        KonqBookmarkSettings::setShownColumns(shownColumns);
        
        setColumnHidden(columnIndex, !show);
    }
}
