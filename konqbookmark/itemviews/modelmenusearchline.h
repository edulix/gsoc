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

#ifndef MODELMENUSEARCHLINE_H
#define MODELMENUSEARCHLINE_H

#include "konqbookmark_export.h"

#include <QWidget>

class KLineEdit;
class QSortFilterProxyModel;
class ModelMenu;

/**
 * @class ModelMenuSearchLine
 *
 * Responsible for the quick search in ModelMenu. It sets the filter string of
 * a QSortFilterProxyModel which can be specified with setProxy() method.
 *
 * @author Tom Albers <tomalbers@kde.nl>
 * @author Eduardo Robles Elvira <edulix@gmail.com>
 */

class KONQBOOKMARK_EXPORT ModelMenuSearchLine
    : public QWidget
{
    Q_OBJECT

public:
    /**
     * Contructor. Takes as an argument the parent model menu.
     */
    explicit ModelMenuSearchLine(ModelMenu* parent = 0);

    /**
     * Destructor
     */
    ~ModelMenuSearchLine();

    /**
     * Associate a proxy
     * @param proxy The proxy to operate with.
     */
    void setProxy(QSortFilterProxyModel* proxy);

    /**
     * Returns the pointer of the lineedit.
     */
    KLineEdit* lineEdit() const;

private:
    class Private;
    Private* const d;
    
    Q_DISABLE_COPY(ModelMenuSearchLine)
    Q_PRIVATE_SLOT(d, void slotSearchLineChange(const QString& newText))
    Q_PRIVATE_SLOT(d, void slotSearchLineActivate())
    Q_PRIVATE_SLOT(d, void slotRootChanged(const QModelIndex &rootIndex))
};

#endif //MODELMENUSEARCHLINE_H
