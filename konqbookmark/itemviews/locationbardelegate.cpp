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

#include "locationbardelegate.h"
#include "place.h"
#include "placesmanager.h"

#include <QObject>
#include <QColor>
#include <QPainter>

#include <kdebug.h>
#include <kiconloader.h>

using namespace Konqueror;

LocationBarDelegate::LocationBarDelegate(QObject* parent): QStyledItemDelegate(parent)
{

}

void LocationBarDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QVariant variant = index.data(Place::PlaceUrlRole);
    if (variant == QVariant()) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }
    
    QUrl url = variant.toString();
    Place* place = PlacesManager::self()->place(url);

    painter->save();
    painter->translate(option.rect.x(), option.rect.y());
    
    QColor titleColor;
    QColor urlColor;
    if (option.state & QStyle::State_Selected) {
        kDebug() << "selected";
        painter->fillRect(option.rect, option.palette.highlight());
        titleColor = option.palette.color(QPalette::HighlightedText);
        urlColor = option.palette.color(QPalette::HighlightedText);
    } else {
        painter->fillRect(option.rect, option.palette.window());
        titleColor = option.palette.color(QPalette::Text);
        urlColor = option.palette.color(QPalette::Dark);
    }

    painter->setPen(titleColor);
    painter->drawText(27, 16, place->title());
    painter->setPen(urlColor);
    painter->drawText(27, 32, place->url().toString());
    
    if(place->bookmark()) {
        const QPixmap bookmark = SmallIcon("bookmarks");
        const int x = option.rect.width() - bookmark.width() - 3;
        painter->drawPixmap(x, 3, bookmark);
    }
    painter->restore();
}


QSize LocationBarDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize s = QStyledItemDelegate::sizeHint(option, index);
    s.setHeight(s.height() * 2);
    return s;
}

