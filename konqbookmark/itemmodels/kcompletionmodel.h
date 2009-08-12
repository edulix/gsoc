/*
    This file is part of the KDE libraries
    Copyright (c) 2009 Eduardo Robles Elvira <edulix@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KCOMPLETION_MODEL_H
#define KCOMPLETION_MODEL_H

#include "konqbookmark_export.h"
#include <kcompletion.h>
#include <QString>
#include <QObject>
#include <QAbstractItemModel>

class QStringList;
class KUrl;

/**
 * @brief A wrapper model for KCompletion objects.
 *
 * This is a convenience readonly model useful when you want to represent under
 * the well-known interface of a QAbstractItemModel the data of a KCompletion
 * object.
 * 
 * Works by listening to the KCompletion signals and applying the modifications
 * to the model. So when new matches are found, those are reflected in the
 * completion model, and similarly when matches are removed from the completion
 * object the same happens in the model.
 * 
 * The only column this model has shows the text for the match. Inherit from 
 * this model and reimplement data() and columnCount() if you want to change 
 * that behaviour.
 */
class KONQBOOKMARK_EXPORT KCompletionModel : public QAbstractItemModel
{
    Q_OBJECT
    
public:
    enum Column { Default = 0 };
    enum {ColumnCount = 1 };
    KCompletionModel(QObject * parent);
    virtual ~KCompletionModel();
    
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex &index, int role) const;

    KCompletion* completion() const;
    void setCompletion(KCompletion* completion);
    
    virtual QModelIndex index(int, int, const QModelIndex&) const;
    virtual QModelIndex parent(const QModelIndex&) const;
    virtual int columnCount(const QModelIndex&) const;

private:
    class Private;
    friend class Private;
    Private* const d;
    
    Q_PRIVATE_SLOT( d, void slotMatches(const QStringList &) )
    Q_PRIVATE_SLOT( d, void slotMatches() )
};

#endif // KCOMPLETION_MODEL_H
