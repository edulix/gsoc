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

#ifndef KDATAWIDGETSELECTIONMAPPER_H
#define KDATAWIDGETSELECTIONMAPPER_H

#include "konqbookmark_export.h"

#include <QDataWidgetMapper>

class QAbstractItemDelegate;
class QAbstractItemModel;
class QItemSelectionModel;
class QModelIndex;

/**
 * @short Provides mapping between a selection of a data model to widgets
 *  
 * Works exactly like QDataWidgetMapper but setting current index
 * by retrieving it from a QItemSelectionModel. It's useful for editing the
 * current index in a view.
 * 
 * The typical usecase scenario  would be:
 * 
 * @code
    MyModel *model = new MyModel(this);
    QTreeView *mainView = new QTreeView(this);
    mainView->setModel(model);

    QLineEdit *titleBox = new QLineEdit(this);
    
    KDataWidgetSelectionMapper *selectionMapper = new KDataWidgetSelectionMapper(this);
    selectionMapper->setSelectionModel(mainView->selectionModel());
    selectionMapper->addMapping(titleBox, MyModel::Title);
 * @endcode
 *
 * The class itself is implemented using a delegated QDataWidgetMapper because
 * using inheritance would allow the user to access to currentIndex property or
 * methods like toNext() toPrevious() which don't make sense in this widget
 * because current index is managed directly in the selection model.
 *
 * @see QDataWidgetMapper for documentation of the inherited methods
 *
 * @author Eduardo Robles Elvira <edulix@gmail.com>
 */
class KONQBOOKMARK_EXPORT KDataWidgetSelectionMapper : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
    Q_PROPERTY(QDataWidgetMapper::SubmitPolicy submitPolicy READ submitPolicy WRITE setSubmitPolicy)

public:
    /**
     * Default constructor.
     */
    KDataWidgetSelectionMapper(QObject *parent = 0);
    
    virtual ~KDataWidgetSelectionMapper();
    
    void setSelectionModel(QItemSelectionModel *selectionModel);
    QItemSelectionModel *selectionModel();
    
    void setModel(QAbstractItemModel *model);
    QAbstractItemModel *model() const;
    
    void setItemDelegate(QAbstractItemDelegate *delegate);
    QAbstractItemDelegate *itemDelegate() const;

    void setOrientation(Qt::Orientation aOrientation);
    Qt::Orientation orientation() const;

    void setSubmitPolicy(QDataWidgetMapper::SubmitPolicy policy);
    QDataWidgetMapper::SubmitPolicy submitPolicy() const;

    void addMapping(QWidget *widget, int section);
    void addMapping(QWidget *widget, int section, const QByteArray &propertyName);
    void removeMapping(QWidget *widget);
    int mappedSection(QWidget *widget) const;
    QByteArray mappedPropertyName(QWidget *widget) const;
    QWidget *mappedWidgetAt(int section) const;
    void clearMapping();

public Q_SLOTS:
    void revert();
    bool submit();
    
private:
    class Private;
    Private* const d;
    
    Q_PRIVATE_SLOT(d, void slotUpdateCurrentIndex())
    Q_DISABLE_COPY(KDataWidgetSelectionMapper)
};

#endif // KDATAWIDGETSELECTIONMAPPER_H
