/*
   This file is part of the KDE libraries
   Copyright (c) 2009 Eduardo Robles Elvira <edulix@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.   If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "kcompletionmodel.h"
#include <QHash>
#include <QPair>
#include <QVariant>
#include <QHashIterator>
#include <QStringList>
#include <QStringListIterator>
#include <QModelIndexList>
#include <QPersistentModelIndex>

/**
 * Private class that helps to provide binary compatibility between releases.
 * @internal
 */
//@cond PRIVATE
class KCompletionModel::Private
{
public:
    Private(KCompletionModel* parent);
    
    /**
     * Connected to @p m_completion matches() signal. Instead or reseting the
     * model, it adds new matches and remove old ones.
     */
    void slotMatches(const QStringList& matches);
    void slotMatches();
    
    /**
     * @returns an invalid index (QModelIndex()) if match is not found or the
     * corresponding QModelIndex for that match if found.
     */
    QModelIndex indexForMatch(const QString& match) const;
    
    /**
     * This method inserts a list of indexes in the model. It will call to
     * beginInsertRows() and endInsertRows() appropiately. Called by
     * @p slotMatches .
     * 
     * @note This method expects a list of contiguous new model indexes.
     */
    void insertRows(const QList<QPair<QModelIndex, QVariant> >& indexes);
    
    /**
     * This method removes a list of indexes in the model. It will call to
     * beginRemoveRows() and endRemoveRows() appropiately. Called by
     * @p slotMatches .
     * 
     * @arg indexes needs to be a list of valid indexes which are currently
     * inserted in the model.
     */
    void removeRows(const QModelIndexList& indexes);
    
    KCompletionModel* q;
    
    KCompletion* m_completion;
    QList<QString> m_strings;
    QHash<QPersistentModelIndex, QString> m_indexes;
    QHash<QString, QPersistentModelIndex> m_reverseIndexes; 
};

KCompletionModel::Private::Private(KCompletionModel* parent)
    : q(parent), m_completion(0)
{

}

void KCompletionModel::Private::slotMatches()
{
    const QStringList matches = m_completion->allMatches();
    slotMatches(matches);
}

void KCompletionModel::Private::slotMatches(const QStringList &matches)
{   
    QModelIndexList indexesToRemove;
    QHashIterator<QString, QPersistentModelIndex> it(m_reverseIndexes);
    
    while (it.hasNext()) {
        it.next();
        if (!matches.contains(it.key())) {
            indexesToRemove.append(it.value());
        }
    }
    
    if (!indexesToRemove.empty()) {
        removeRows(indexesToRemove);
    }
    
    QList<QPair<QModelIndex, QVariant> > indexesToAppend;
    QList<QString>::const_iterator strIt;
    int count = m_strings.size();
    for (strIt = matches.constBegin(); strIt != matches.constEnd(); ++strIt) {
        if (!m_reverseIndexes.contains(*strIt)) {
            QPair<QModelIndex, QVariant> indexPair;
            indexPair.first = q->createIndex(count++, 0);
            indexPair.second = *strIt;
            
            indexesToAppend.append(indexPair);
        }
    }

    if (!indexesToAppend.empty()) {
        insertRows(indexesToAppend);
    }
}

void KCompletionModel::Private::insertRows(const QList<QPair<QModelIndex, QVariant> >& indexes)
{
    if (indexes.empty()) {
        return;
    }
    
    QList<QPair<QModelIndex, QVariant> >::ConstIterator it;
    
    int start = indexes.first().first.row();
    int end = indexes.last().first.row();
    int pos = start;
    
    q->beginInsertRows(QModelIndex(), start, end);
    for (it = indexes.constBegin(); it != indexes.constEnd(); ++it) {
        const QModelIndex &index = (*it).first;
        const QString &match = (*it).second.toString();
        
        m_strings.insert(pos++, match);
        m_indexes[index] = match;
        m_reverseIndexes[match] = index;
    }
    
    q->endInsertRows();
    
    for (it = indexes.constBegin(); it != indexes.constEnd(); ++it) {
        const QModelIndex &index = (*it).first;
    }
}

void KCompletionModel::Private::removeRows(const QModelIndexList& list)
{
    // First convert the indexes to persistent because we'll remove them one by one
    QList<QPersistentModelIndex> indexes;
    QList<QModelIndex>::const_iterator listIt;
    for (listIt = list.constBegin(); listIt != list.constEnd(); ++listIt) {
        indexes.append(*listIt);
    }
    
    QList<QPersistentModelIndex>::ConstIterator it;
    
    for (it = indexes.constBegin(); it != indexes.constEnd(); ++it) {
        const QPersistentModelIndex &index = *it;
        int pos = index.row();
        
        q->beginRemoveRows(QModelIndex(), pos, pos);
        
        QString match = m_indexes[index];
        
        m_strings.removeAt(pos);
        m_indexes.remove(index);
        m_reverseIndexes.remove(match);
        
        q->endRemoveRows();
    }
}

QModelIndex KCompletionModel::Private::indexForMatch(const QString& match) const
{
    if (m_reverseIndexes.contains(match)) {
        return m_reverseIndexes.value(match);
    }
    
    return QModelIndex();
}

//@endcond

KCompletionModel::KCompletionModel(QObject *parent)
    : QAbstractItemModel(parent), d(new Private(this))
{
    
}

KCompletionModel::~KCompletionModel()
{
    delete d;
}

int KCompletionModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    
    return d->m_strings.size();
}

QVariant KCompletionModel::data(const QModelIndex &index, int role) const
{
    
    if (index.row() < 0 || index.row() >= d->m_strings.size()) {
        return QVariant();
    }
    
    switch(role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case CompletionMatchRole:
        return d->m_strings.at(index.row());
    }
    return QVariant();
}

KCompletion* KCompletionModel::completion() const
{
    return d->m_completion;
}
void KCompletionModel::setCompletion(KCompletion* completion)
{
    if (!completion) {
        return;
    }
    
    if (d->m_completion) {
        disconnect(d->m_completion, SIGNAL(matches(const QStringList &)),
            this, SLOT(slotMatches(const QStringList &)));
        
        d->m_completion = 0;
        d->m_strings.clear();
        d->m_indexes.clear();
        d->m_reverseIndexes.clear();
    }
    
    d->m_completion = completion;
    
    connect(d->m_completion, SIGNAL(matches(const QStringList &)),
        this, SLOT(slotMatches(const QStringList &)));
    connect(d->m_completion, SIGNAL(match(const QString&)),
        this, SLOT(slotMatches()));
    connect(d->m_completion, SIGNAL(multipleMatches()),
        this, SLOT(slotMatches()));
    reset();
    
    // refill
    d->slotMatches();
}

QModelIndex KCompletionModel::index(int row, int column, const QModelIndex& parent) const
{   
    if (parent.isValid()) {
        return QModelIndex();
    }
    if (row < 0 || row >= d->m_strings.size() || column != 0) {
        return QModelIndex();
    }
    
    return createIndex(row, column);
}

QModelIndex KCompletionModel::parent(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }
    
    if (index.column() != 0) {
        QModelIndex index = createIndex(index.row(), 0);
        return (d->m_indexes.contains(index)) ? index : QModelIndex();
    }
    
    return QModelIndex();
}

int KCompletionModel::columnCount(const QModelIndex& index) const
{
    Q_UNUSED(index);
    return ColumnCount;
}

#include "kcompletionmodel.moc"
