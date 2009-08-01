/*
 * Copyright (C) 2007-2008 Omat Holding B.V. <info@omat.nl>
 * Copyright (c) 2009 Eduardo Robles Elvira <edulix@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "modelmenusearchline.h"
#include "modelmenu.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QSortFilterProxyModel>

#include <klineedit.h>
#include <klocale.h>
#include <kdebug.h>

/**
 * Private class that helps to provide binary compatibility between releases.
 * @internal
 */
//@cond PRIVATE
class ModelMenuSearchLine::Private {
public:
    Private( ModelMenuSearchLine* parent) :
                                q(parent), proxy(0), searchLine(0)
    {
    timer = new QTimer( q );
    timer->setSingleShot( true );
    connect( timer, SIGNAL( timeout() ), q, SLOT( slotSearchLineActivate() ) );
    }
    QTimer* timer;
    ModelMenuSearchLine* q;
    QSortFilterProxyModel* proxy;
    KLineEdit* searchLine;
    ModelMenu* m_parent;

    void slotSearchLineChange( const QString& newText );
    void slotSearchLineActivate();
    void slotRootChanged(const QModelIndex &rootIndex);
};

void ModelMenuSearchLine::Private::slotSearchLineChange( const QString& )
{
    timer->start( 300 );
}


void ModelMenuSearchLine::Private::slotSearchLineActivate()
{
    if ( !proxy )
        return;
    
    kDebug() << "starts";
    proxy->setFilterFixedString( searchLine->text() );
    kDebug() << "text = " << searchLine->text();
    
    bool activate = !searchLine->text().isEmpty();
    if(m_parent->searchActive() != activate)
        m_parent->setSearchActive(activate);
}

void ModelMenuSearchLine::Private::slotRootChanged(const QModelIndex &rootIndex)
{
    QString rootTitle = rootIndex.data().toString();
    if(rootTitle.isEmpty())
        searchLine->setClickMessage(i18n("Search..."));
    else
        searchLine->setClickMessage(i18n("Search in %1...", rootTitle));
}

//@endcond

ModelMenuSearchLine::ModelMenuSearchLine( ModelMenu* parent )
        : QWidget( parent ), d( new Private( this ) )
{
    d->m_parent = parent;

    d->searchLine = new KLineEdit( this );
    d->searchLine->setClearButtonShown( true );
    d->searchLine->setMinimumWidth(300);
    
    d->slotRootChanged(parent->rootIndex());
    
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setMargin( 0 );
    layout->addWidget( d->searchLine );
    setProxy(parent->searchModel());

    connect( d->searchLine, SIGNAL( textChanged( const QString& ) ),
             SLOT( slotSearchLineChange( const QString& ) ) );
    connect( parent, SIGNAL( rootChanged(const QModelIndex &) ),
             SLOT( slotRootChanged(const QModelIndex & ) ) );
}

ModelMenuSearchLine::~ModelMenuSearchLine()
{
    delete d;
}

void ModelMenuSearchLine::setProxy( QSortFilterProxyModel* proxy ) 
{
    d->proxy = proxy;
    proxy->setDynamicSortFilter(true);
    proxy->setFilterKeyColumn( -1 );
    proxy->setFilterCaseSensitivity( Qt::CaseInsensitive );
}

KLineEdit* ModelMenuSearchLine::lineEdit() const
{
    return d->searchLine;
}

#include "modelmenusearchline.moc"
