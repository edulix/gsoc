/*
    This file is part of the KDE project
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

#include "kdatawidgetselectionmapper.h"

#include <QAbstractItemDelegate>
#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QDataWidgetMapper>
#include <QModelIndex>
#include <QObject>

class KDataWidgetSelectionMapper::Private
{
public:
    Private(KDataWidgetSelectionMapper *parent);
    
    void slotUpdateCurrentIndex();
    
    KDataWidgetSelectionMapper* m_parent;
    QDataWidgetMapper* m_mapper;
    QItemSelectionModel* m_selectionModel;
};

KDataWidgetSelectionMapper::Private::Private(KDataWidgetSelectionMapper *parent)
    : m_parent(parent),
    m_mapper(new QDataWidgetMapper(parent)),
    m_selectionModel(0)
{
    
}

void KDataWidgetSelectionMapper::Private::slotUpdateCurrentIndex()
{
    m_mapper->setRootIndex(m_selectionModel->currentIndex().parent());
    m_mapper->setCurrentModelIndex(m_selectionModel->currentIndex());
}

KDataWidgetSelectionMapper::KDataWidgetSelectionMapper(QObject *parent)
    : QObject(parent), d(new Private(this))
{

}

KDataWidgetSelectionMapper::~KDataWidgetSelectionMapper()
{
    delete d;
}

void KDataWidgetSelectionMapper::setModel(QAbstractItemModel *model)
{
    if (d->m_mapper->model() == model)
        return;
    
    QItemSelectionModel *tempSelectionModel = d->m_selectionModel;
    if (d->m_selectionModel && d->m_mapper->model())
    {
        d->m_selectionModel = 0;
        disconnect(d->m_selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(slotUpdateCurrentIndex()));
    }
    d->m_mapper->setModel(model);
    
    if (tempSelectionModel)
        setSelectionModel(tempSelectionModel);
}

QAbstractItemModel* KDataWidgetSelectionMapper::model() const
{
    return d->m_mapper->model();
}

void KDataWidgetSelectionMapper::setSelectionModel(QItemSelectionModel *selectionModel)
{
    if (d->m_selectionModel == selectionModel)
        return;

    if (d->m_selectionModel && d->m_mapper->model())
    {
        disconnect(d->m_selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(slotUpdateCurrentIndex()));
    }
    
    d->m_selectionModel = selectionModel;
    
    connect(d->m_selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
        this, SLOT(slotUpdateCurrentIndex()));
        
    d->slotUpdateCurrentIndex();
}

QItemSelectionModel* KDataWidgetSelectionMapper::selectionModel()
{
    return d->m_selectionModel;
}

void KDataWidgetSelectionMapper::setItemDelegate(QAbstractItemDelegate *delegate)
{
    d->m_mapper->setItemDelegate(delegate);
}

QAbstractItemDelegate* KDataWidgetSelectionMapper::itemDelegate() const
{
    return d->m_mapper->itemDelegate();
}


void KDataWidgetSelectionMapper::setOrientation(Qt::Orientation aOrientation)
{
    d->m_mapper->setOrientation(aOrientation);
}

Qt::Orientation KDataWidgetSelectionMapper::orientation() const
{
    return d->m_mapper->orientation();
}

void KDataWidgetSelectionMapper::setSubmitPolicy(QDataWidgetMapper::SubmitPolicy policy)
{
    d->m_mapper->setSubmitPolicy(policy);
}

QDataWidgetMapper::SubmitPolicy KDataWidgetSelectionMapper::submitPolicy() const
{
    return d->m_mapper->submitPolicy();
}


void KDataWidgetSelectionMapper::addMapping(QWidget *widget, int section)
{
    d->m_mapper->addMapping(widget, section);
}

void KDataWidgetSelectionMapper::addMapping(QWidget *widget, int section, const QByteArray &propertyName)
{
    d->m_mapper->addMapping(widget, section, propertyName);
}

void KDataWidgetSelectionMapper::removeMapping(QWidget *widget)
{
    d->m_mapper->removeMapping(widget);
}

int KDataWidgetSelectionMapper::mappedSection(QWidget *widget) const
{
    return d->m_mapper->mappedSection(widget);
}

QByteArray KDataWidgetSelectionMapper::mappedPropertyName(QWidget *widget) const
{
    return d->m_mapper->mappedPropertyName(widget);
}

QWidget* KDataWidgetSelectionMapper::mappedWidgetAt(int section) const
{
    return d->m_mapper->mappedWidgetAt(section);
}

void KDataWidgetSelectionMapper::clearMapping()
{
    d->m_mapper->clearMapping();
}


void KDataWidgetSelectionMapper::revert()
{
    d->m_mapper->revert();
}

bool KDataWidgetSelectionMapper::submit()
{
    return d->m_mapper->submit();
}

#include "kdatawidgetselectionmapper.moc"
