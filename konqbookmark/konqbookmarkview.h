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

#ifndef KONQBOOKMARK_VIEW_H
#define KONQBOOKMARK_VIEW_H

#include "konqbookmark_export.h"
#include <akonadi/itemview.h>

class KonqBookmark;

namespace Akonadi
{

    class KONQBOOKMARK_EXPORT KonqBookmarkView : public ItemView
    {
    Q_OBJECT
    public:
        KonqBookmarkView(QWidget *parent);
        
        ~KonqBookmarkView();
        
        virtual void setModel (QAbstractItemModel *model);
        
    public Q_SLOTS:
        /**
         * Opens a context menu at the position \a pos and allows to
         * configure the visibility of the header columns and whether
         * expandable folders should be shown.
         */
        void configureSettings(const QPoint& pos);
        
    protected:
        void dragEnterEvent(QDragEnterEvent *event) {}
        void dragLeaveEvent(QDragLeaveEvent *event) {}
        void dragMoveEvent(QDragMoveEvent *event) {}
        void dropEvent(QDropEvent *event) {}
        void mousePressEvent(QMouseEvent *event) {}
        void paintEvent(QPaintEvent *event) {}
        
    private:
        QRect highlightedRect;
    };

}

#endif // KONQ_BOOKMARK_VIEW_H

