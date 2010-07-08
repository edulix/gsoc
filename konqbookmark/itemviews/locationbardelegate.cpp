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

#include <limits.h>

#include <QtCore/QObject>
#include <QtGui/QFontMetrics>
#include <QtGui/QColor>
#include <QtGui/QPainter>
#include <QtGui/QApplication>

#include <KIconLoader>

using namespace Konqueror;

LocationBarDelegate::LocationBarDelegate(LocationBar *parent)
    : QStyledItemDelegate(parent), m_parent(parent)
{

}

void LocationBarDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const
    QModelIndex &index) const
{
    // Try to get the place related to this index, which contains the
    // information we need to paint
    QVariant variant = index.data(Place::PlaceUrlRole);
    if (variant == QVariant()) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    QUrl url = variant.toString();
    Place *place = PlacesManager::self()->place(url);

    // Start painting
    painter->save();
    QStyleOptionViewItemV4 option2(option);
    option2.state = option2.state | QStyle::State_Active;
    QStyledItemDelegate::paint(painter, option2, index);
    painter->translate(option.rect.x(), option.rect.y());
    // Paint background and stablish main text colors depending on if we are
    // painting the currently selected index or not
    // TODO: why doesn't it work fine? background for selected items is only
    // painted in the first item, and normal items' background is only painted
    // for even indexex.

    QColor titleColor;
    QColor urlColor;

    if (option.state & QStyle::State_Selected) {
        urlColor = titleColor = option.palette.color(QPalette::HighlightedText);
    } else {
        titleColor = option.palette.color(QPalette::Text);
        urlColor = option.palette.color(QPalette::Link);
    }

    // Paint url
    painter->setPen(urlColor);
    paintText(painter, 27, 40, place->url().toString(), AlignLeft, option.rect.width() - 30);

    // Paint relevance (for debugging only!)
    painter->setPen(titleColor);
    QFont font = painter->font();
    QFont fontPrev = painter->font();
    font.setPointSize(font.pointSize()-1);
    painter->setFont(font);
    qreal relevance = qVariantValue<qreal>(index.data(Place::PlaceRelevanceRole));
    paintText(painter, option.rect.width() - 3, 40, QString::number(relevance), AlignRight);

    // Paint tags if any (use the same font as in relevance)
    int maxTitleWidth = option.rect.width();
    QString tags;
    if (!place->tags().isEmpty() && !(tags = place->tags().join(", ")).isEmpty()) {
        const QString tags = place->tags().join(", ");
        const QPixmap tagIcon = SmallIcon("item-tagged");
        int rightX = option.rect.width() - tagIcon.width() - 6;
        const int bottomY = 17;
        const int maxWidth = option.rect.width() / 3.0;
        const int usedWidth = paintText(painter, rightX - 3, bottomY, tags, AlignRight, maxWidth);
        rightX -= usedWidth + tagIcon.width() + 6;
        painter->drawPixmap(rightX, 4, tagIcon);
        maxTitleWidth = rightX - 3;
    }
    painter->setFont(fontPrev);

    // Paint title
    painter->setPen(titleColor);
    font = painter->font();
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


/**
 * Returns the next position of any of the needles in the haystack starting from startPos,
 * and also takes startPos and endPos as references which get updated.
 */
bool nextPosition(const QString &haystack, const QStringList &needles, int &startPos,
    const int oldPos, int &endPos)
{
    bool foundMatch = false;
    Q_FOREACH (QString needle, needles) {
        int pos = haystack.indexOf(needle, oldPos, Qt::CaseInsensitive);

        // First appearance
        if (!foundMatch && pos >= startPos) {
            startPos = pos;
            foundMatch = true;
            endPos = pos + needle.size();
        // Find a joined word. This happens if for example:
        //  - haystack = "Qt QScrollArea: help system"
        //  - needles is = ["qscroll", "scrollarea"]
        //  - startPos = 0
        // Then in first iteration of this loop, startPos = 3, endPos = 3 + 7 = 10
        // Then here we check that the second match starts between 3 and 10 in order to be 
        // a "joined word".
        } else if (pos >= startPos && pos <= endPos) {
            int posPlusSize = pos + needle.size();
            if (posPlusSize > endPos) {
                endPos = posPlusSize;
            }
        // Find a previous word. This is a completely different case. We might have:
        // - haystack = "sabes que nepomuk kmola que si que nepomukmola"
        // - needles is = ["kmola", "nepomuk"]
        // - startPos = 0
        // Then at first, we get  startPos = 19, endPos = 22 for the first match of "kmola",
        // but at next iteration of the needles we need to find out that, actually nepomuk
        // word at [11, 18] is first.
        // Same happens with startPos > 18. First, we get match for kmola in [42, 47], but
        // then we need to get the match for "nepomuk" in [26, 43], but also noting that
        // there's a "joined word" because the correct match is the complete word
        // "nepomukmola", at [36, 47]
        } else if (foundMatch && pos != -1 && pos < startPos) {
            int needleSize = needle.size();
            if (pos + needleSize < startPos) {
                startPos = pos;
                endPos = startPos + needleSize;
            // endPos continues to be valid because we got a "joined word", like
            // in the example with "nepomukmola"!:
            } else {
                startPos = pos;
            }
        }
    }
    return foundMatch;
}

int LocationBarDelegate::paintText(QPainter *painter, int x, int bottomY, QString text, TextAlignment textAlignment, int maxWidth) const
{
    // maxWidth == -1 means there's no maxWidth, so we need to set a really high maxwidth.
    // We don't directly use INT_MAX because QFontMetrics thinks it's too much, apparently
    if (maxWidth == -1) {
        maxWidth = INT_MAX / 100;
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

    QStringList query = m_parent->words();
    QFont fontNormal(painter->font());
    QFontMetrics fontMetrics(painter->fontMetrics());

    if (query.isEmpty()) {
        painter->setFont(fontNormal);
        painter->drawText(x, bottomY, text);
        return fontMetrics.width(text);
    }

    QList<int>      positions;
    QList<QString>  substrings;
    QList<bool>     underline;

    int width = 0;
    int oldPos = 0;
    int currentPos = 0;
    int endPos = 0;
    int textSize = text.size();

    QFont fontUnderline(painter->font());
    fontUnderline.setBold(true);
    fontUnderline.setUnderline(true);
    QFontMetrics fontMetricsUnderline(fontUnderline);
    bool endByEliding = false;

    // Find matches and non matches, populate list of substrings
    while (nextPosition(text, query, currentPos, oldPos, endPos) && width < maxWidth) {
        // Add the text before the match, if any
        if (currentPos > oldPos) {
            underline.append(false);
            positions.append(width);

            //elide text
            QString tempString = text.mid(oldPos, currentPos - oldPos);
            int delta = maxWidth - width;
            QString substring = painter->fontMetrics().elidedText(tempString, Qt::ElideRight, delta);
            substrings.append(substring);
            width += fontMetrics.width(substring);
            // If text elided finish
            if (substring.size() != tempString.size()) {
                endByEliding = true;
                break;
            }
        }

        // Now add the matched text
        underline.append(true);
        positions.append(width);

        // elide text
        QString tempString = text.mid(currentPos, endPos - currentPos);
        int delta = maxWidth - width;
        QString substring = painter->fontMetrics().elidedText(tempString, Qt::ElideRight, delta);
        substrings.append(substring);
        width += fontMetricsUnderline.width(substring);
        // If text elided finish
        if (tempString.size() != substring.size()) {
            endByEliding = true;
            break;
        }
        oldPos = currentPos + substring.size();
    }

    // "Special" case: adding also the unmatched text of the end of the string
    if (!endByEliding && oldPos < textSize) {
        underline.append(false);
        positions.append(width);

        //elide text
        QString tempString = text.mid(oldPos, textSize - oldPos);
        int delta = maxWidth - width;
        tempString = painter->fontMetrics().elidedText(tempString, Qt::ElideRight, maxWidth);
        substrings.append(tempString);
        width += fontMetrics.width(tempString);
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
    painter->setFont(fontNormal);

    return width;
}
