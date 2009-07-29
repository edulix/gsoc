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
#include "itemmodels/konqbookmarkmodel.h"
#include "konqbookmarksettings.h"

#include <QHeaderView>
#include <QApplication>
#include <QPoint>
#include <QFontMetrics>
#include <QAction>
#include <QString>
#include <QStringList>

#include <kmenu.h>
#include <klocale.h>

using namespace Akonadi;

class KonqBookmarkView::Private
{
public:
    Private()
    {
        mAutoResize = true;
    }

    Private( const Private& other )
    {
        mAutoResize = other.mAutoResize;
    }
    
public:
    bool mAutoResize : 1;        // if true, the columns are resized automatically to the available width
};

KonqBookmarkView::KonqBookmarkView( QWidget *parent )
  : EntityTreeView( parent ), d( new Private() )
{
    QHeaderView* headerView = header();
    headerView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(headerView, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(configureSettings(const QPoint&)));
    connect(headerView, SIGNAL(sectionResized(int, int, int)),
        this, SLOT(slotHeaderSectionResized(int, int, int)));
    connect(headerView, SIGNAL(sectionHandleDoubleClicked(int)),
        this, SLOT(disableAutoResizing()));
}

KonqBookmarkView::~KonqBookmarkView()
{
    KonqBookmarkSettings::self()->writeConfig();
    delete d;
}


void KonqBookmarkView::setModel(QAbstractItemModel *model)
{
    EntityTreeView::setModel(model);
    
    QStringList shownColumns = KonqBookmarkSettings::shownColumns();
        
    for(int i = KonqBookmarkModel::Url; i <= KonqBookmarkModel::LastVisited; ++i)
    {
        const QString columnName = model->headerData(i, Qt::Horizontal).toString();
        bool show = shownColumns.contains(columnName);
        setColumnHidden(i, !show);
    }
    resizeColumns();
}

void KonqBookmarkView::configureSettings(const QPoint& pos)
{
    KMenu popup(this);
    popup.addTitle(i18nc("@title:menu", "Columns"));

    // add checkbox items for each column
    QHeaderView* headerView = header();
    
    for (int i = KonqBookmarkModel::Url; i < KonqBookmarkModel::ColumnCount; ++i)
    {
        const QString text = model()->headerData(i, Qt::Horizontal).toString();
        QAction* action = popup.addAction(text);
        action->setCheckable(true);
        action->setChecked(!headerView->isSectionHidden(i));
        action->setData(i);
    }
    popup.addSeparator();

    QAction* activatedAction = popup.exec(headerView->mapToGlobal(pos));
    if (activatedAction != 0)
    {
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
        resizeColumns();
    }
}

void KonqBookmarkView::resizeEvent(QResizeEvent* event)
{
    EntityTreeView::resizeEvent(event);
    if (d->mAutoResize) {
        resizeColumns();
    }
}

void KonqBookmarkView::slotHeaderSectionResized(int logicalIndex, int oldSize, int newSize)
{
    Q_UNUSED(logicalIndex);
    Q_UNUSED(oldSize);
    Q_UNUSED(newSize);
    
    // If the user changes the size of the headers, the autoresize feature should be
    // turned off. As there is no dedicated interface to find out whether the header
    // section has been resized by the user or by a resize event, another approach is used.
    if ((QApplication::mouseButtons() & Qt::LeftButton) && header()->underMouse()) {
        disableAutoResizing();
    }
}

void KonqBookmarkView::disableAutoResizing()
{
    d->mAutoResize = false;
}


void KonqBookmarkView::resizeColumns()
{
    // Using the resize mode QHeaderView::ResizeToContents is too slow (it takes
    // around 3 seconds for each (!) resize operation when having > 10000 items).
    // This gets a problem especially with a lot of bookmarks, where several
    // resize operations are received for showing the currently available items during
    // loading (the application hangs around 20 seconds when loading > 10000 items).

    QHeaderView* headerView = header();
    QFontMetrics fontMetrics(viewport()->font());

    int columnWidth[KonqBookmarkModel::ColumnCount];
    columnWidth[KonqBookmarkModel::UniqueUri] = fontMetrics.width("konqbookmark:/xxxxx..");
    columnWidth[KonqBookmarkModel::Tags] = fontMetrics.width("xxxx, xxx, xxxxxx");
    columnWidth[KonqBookmarkModel::Description] = fontMetrics.width("xxxx xxx xxxxxx xxx");
    columnWidth[KonqBookmarkModel::NumVisits] = fontMetrics.width("Visits Counter");
    columnWidth[KonqBookmarkModel::Created] = fontMetrics.width("00/00/00 00:00");
    columnWidth[KonqBookmarkModel::LastModified] = fontMetrics.width("00/00/00 00:00");
    columnWidth[KonqBookmarkModel::LastVisited] = fontMetrics.width("00/00/00 00:00");

    int requiredWidth = 0;
    for (int i = KonqBookmarkModel::UniqueUri; i < KonqBookmarkModel::ColumnCount; ++i) {
        if (!isColumnHidden(i)) {
            columnWidth[i] += 20; // provide a default gap
            requiredWidth += columnWidth[i];
            headerView->resizeSection(i, columnWidth[i]);
        }
    }

    // resize the title (and Url) columns in a way that the whole available width is used
    int available = viewport()->width() - requiredWidth;
    
    // If Url column is not shown, all the available with is used for the title
    if(isColumnHidden(KonqBookmarkModel::Url))
        columnWidth[KonqBookmarkModel::Title] = available;
    else {
        columnWidth[KonqBookmarkModel::Title] = available * 0.55;
        columnWidth[KonqBookmarkModel::Url] = available * 0.45;
    }

    int minTitleWidth = 300;
    int minUrlWidth = minTitleWidth*(0.45/0.55);
    
    // It might be possible that the title or url columns width can be
    // decreased without clipping any text. . For performance
    // reasons the exact necessary width for full visible titles/url is
    // only checked for up to 200 items
    if(model()) {
        const int rowCount = model()->rowCount();
        if (rowCount > 0 && rowCount < 200) {
            const int titleWidth = sizeHintForColumn(KonqBookmarkModel::Title);
            const int urlWidth = sizeHintForColumn(KonqBookmarkModel::Url);
            if (titleWidth < minTitleWidth)
                minTitleWidth = titleWidth;
            if (urlWidth < minUrlWidth)
                minUrlWidth = urlWidth;
        }
    }
    
    if (columnWidth[KonqBookmarkModel::Title] < minTitleWidth)
        columnWidth[KonqBookmarkModel::Title] = minTitleWidth;
    
    if(!isColumnHidden(KonqBookmarkModel::Url) && columnWidth[KonqBookmarkModel::Url] < minUrlWidth)
        columnWidth[KonqBookmarkModel::Url] = minUrlWidth;
    
    headerView->resizeSection(KonqBookmarkModel::Title, columnWidth[KonqBookmarkModel::Title]);
    
    if(!isColumnHidden(KonqBookmarkModel::Url))
        headerView->resizeSection(KonqBookmarkModel::Url, columnWidth[KonqBookmarkModel::Url]);
}

