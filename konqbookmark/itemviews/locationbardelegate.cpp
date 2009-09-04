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
#include "locationbar.h"
#include "place.h"
#include "placesmanager.h"

#include <QObject>
#include <QFontMetrics>
#include <QColor>
#include <QPainter>

#include <kdebug.h>
#include <kiconloader.h>

using namespace Konqueror;

LocationBarDelegate::LocationBarDelegate(LocationBar *parent)
    : QStyledItemDelegate(parent), m_parent(parent)
{

}

void LocationBarDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    // Try to get the place related to this index, which contains the
    // information we need to paint
    QVariant variant = index.data(Place::PlaceUrlRole);
    if (variant == QVariant()) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }
    
    QUrl url = variant.toString();
    Place* place = PlacesManager::self()->place(url);

    // Start painting
    painter->save();
    painter->translate(option.rect.x(), option.rect.y());
    
    // Paint background and stablish main text colors depending on if we are
    // painting the currently selected index or not
    // TODO: why doesn't it work fine? background for selected items is only
    // painted in the first item, and normal items' background is only painted
    // for even indexex.
    
    QColor titleColor;
    QColor urlColor;
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
        urlColor = titleColor = option.palette.color(QPalette::HighlightedText);
    } else {
        painter->fillRect(option.rect, option.palette.window());
        titleColor = option.palette.color(QPalette::Text);
        urlColor = option.palette.color(QPalette::Dark);
    }
    
    // Paint url
    painter->setPen(urlColor);
    paintText(painter, 27, 40, place->url().toString());
    
    // Paint tags
    int maxTitleWidth = option.rect.width();
    QString tags;
    if (!place->tags().isEmpty() && !(tags = place->tags().join(", ")).isEmpty()) {
        const QString tags = place->tags().join(", ");
        const QPixmap tagIcon = SmallIcon("mail-tagged"); // TODO: ask for a proper icon
        int rightX = option.rect.width() - tagIcon.width() - 6;
        const int bottomY = 17;
        const int maxWidth = option.rect.width() / 3.0;
        painter->setPen(titleColor);
        QFont font = painter->font();
        QFont fontPrev = painter->font();
        font.setPointSize(font.pointSize()-1);
        painter->setFont(font);
        const int usedWidth = paintText(painter, rightX - 3, bottomY, tags, AlignRight, maxWidth);
        painter->setFont(fontPrev);
        
        rightX -= usedWidth + tagIcon.width() + 3;
        painter->drawPixmap(rightX, 4, tagIcon);
        maxTitleWidth = rightX - 3;
    }
    
    // Paint title
    painter->setPen(titleColor);
    QFont font = painter->font();
    font.setPointSize(font.pointSize()+2);
    painter->setFont(font);
    paintText(painter, 27, 19, place->title(), AlignLeft, maxTitleWidth);
    
    if (place->bookmark()) {
        const QPixmap bookmark = SmallIcon("bookmarks");
        const int x = option.rect.width() - bookmark.width() - 3;
        painter->drawPixmap(x, 3, bookmark);
    }
    
    QIcon uriIcon;
    QPixmap uriPixmap;
    uriIcon = PlacesManager::self()->icon(place);
    if (uriIcon.isNull()) {
        uriPixmap = SmallIcon("folder-remote");
    } else {
        uriPixmap = uriIcon.pixmap(16, 16);
    }
    
    painter->drawPixmap(3, 3, uriPixmap);
    
    painter->restore();
}


QSize LocationBarDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize s = QStyledItemDelegate::sizeHint(option, index);
    s.setHeight(50);
    return s;
}

int LocationBarDelegate::paintText(QPainter *painter, int x, int bottomY, QString text, TextAlignment textAlignment, int maxWidth) const
{
    if (maxWidth == -1) {
        maxWidth = 5000; // TODO: set to max int
    }
    
    // In this function we iterate the text for each match of the location bar
    // query string the user entered and elaborate a list of substrings which
    // might or might not be painted underlined depeding if they match with the
    // query.
    // 
    // For example: if the query is "dot" and the text is "Slashdot.org", we
    // will end up with the list of substrings "Slash", "dot", ".org".
    // 
    // After that we iterate the substrings and paint them. If the text does not
    // fit the given width, it's elided (dots are appended at the end), so we
    // migtht end up painting "Slashdot....".
    // 
    // We don't directly paint the text while iterating the matches because in
    // case the text alignment is to the left we don't really know the x 
    // position from which we should start painting until we have finished
    // finding the matches.
    
    QString query = m_parent->userText();
    QList<int>      positions;
    QList<QString>  substrings;
    QList<bool>     underline;
    
    int width = 0;
    int oldPos = 0;
    int currentPos;
    QFont fontNormal(painter->font());
    QFontMetrics fontMetrics(painter->fontMetrics());
    
    QFont fontUnderline(painter->font());
    fontUnderline.setBold(true);
    fontUnderline.setUnderline(true);
    QFontMetrics fontMetricsUnderline(fontUnderline);
    bool endByEliding = false;
    
    // Find matches and non matches, populate list of substrings
    while ((currentPos = text.indexOf(query, oldPos)) != -1 && width < maxWidth) {
        if (currentPos - oldPos > 0) {
            underline.append(false);
            positions.append(width);
            
            //elide text
            QString tempString = text.mid(oldPos, (currentPos - oldPos));
            int delta = maxWidth - width;
            QString substring = painter->fontMetrics().elidedText(tempString, Qt::ElideRight, delta);
            substrings.append(substring);
            width += fontMetrics.width(substring);
            // If text elided finish
            if (tempString != tempString) {
                endByEliding = true;
                break;
            }
        }
        
        underline.append(true);
        positions.append(width);
        
        //elide text
        QString tempString = text.mid(currentPos, query.size());
        int delta = maxWidth - width;
        QString substring = painter->fontMetrics().elidedText(tempString, Qt::ElideRight, delta);
        substrings.append(substring);
        width += fontMetricsUnderline.width(substring);
        // If text elided finish
        if (tempString != substring) {
            endByEliding = true;
            break;
        }
        oldPos = currentPos + substring.size();
    }
    
    if (!endByEliding && oldPos < text.size()) {
        underline.append(false);
        positions.append(width);
        
        //elide text
        QString tempString = text.mid(oldPos, (text.size() - oldPos));
        int delta = maxWidth - width;
        QString substring = painter->fontMetrics().elidedText(tempString, Qt::ElideRight, delta);
        substrings.append(substring);
        width += fontMetrics.width(substring);
    }
    
    int startX;
    
    if (textAlignment == AlignLeft) {
        startX = x;
    } else {
        startX = x - width;
    }
    
    for (int i = 0; i < substrings.size(); i++) {
        QString substring = substrings.at(i);
        if (underline.at(i)) {
            painter->setFont(fontUnderline);
        } else {
            painter->setFont(fontNormal);
        }
                
        int position = startX + positions.at(i);
        painter->drawText(position, bottomY, substring);
    }
    
    return width;
}
