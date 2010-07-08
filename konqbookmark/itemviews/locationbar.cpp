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

#include "locationbar.h"
#include "locationbarfaviconwidget.h"
#include "locationbardelegate.h"
#include "kcompletionview.h"
#include "placesmanager.h"
#include "klineeditviewbutton.h"
#include "itemmodels/kcompletionmodel.h"
#include "itemmodels/konqbookmarkmodel.h"
#include "itemmodels/kaggregatedmodel.h"
#include "itemmodels/placesproxymodel.h"

#include <QtGui/QAbstractItemView>
#include <QtCore/QAbstractItemModel>
#include <QtGui/QTreeView>
#include <QtGui/QStringListModel>
#include <QtCore/QStringList>
#include <QtGui/QWidget>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QCompleter>
#include <QtGui/QApplication>
#include <QtGui/QPainter>

#include <KUrlCompletion>
#include <KUrl>
#include <KLocale>
#include <KIconLoader>

using namespace Konqueror;
using namespace Akonadi;

class LocationBar::Private
{
public:
    Private(LocationBar *parent);
    ~Private();

    void sort();

    void slotReturnPressed(const QString &text);
    void slotCompletionActivated(const QModelIndex &index);
    void updateWords(const QString &text);
    void slotCurrentCompletionChanged(const QModelIndex &index);

    LocationBar *q;
    QString m_currentCompletion;
    bool italicizePlaceholder;
    PlacesProxyModel *m_unsortedModel;
    LocationBarCompletionModel *m_model;
    QAbstractItemView *m_view;
    QStringList m_words;
};

LocationBar::Private::Private(LocationBar *parent)
    : q(parent)
{
}

LocationBar::Private::~Private()
{

}

void LocationBar::Private::updateWords(const QString& text)
{
    m_words = text.split(" ", QString::SkipEmptyParts);
}

QStringList LocationBar::words() const
{
    return d->m_words;
}


void LocationBar::Private::slotReturnPressed(const QString &text)
{
    kDebug() << text;
    q->setText(text);
    emit q->returnPressed(text, qApp->keyboardModifiers());
}


void LocationBar::Private::slotCurrentCompletionChanged(const QModelIndex &index)
{
    kDebug() << index.data().toString();
}

void LocationBar::Private::slotCompletionActivated(const QModelIndex& index)
{
    kDebug() << index.data().toString();
    q->setText(index.data().toString());
    emit q->returnPressed(index.data().toString(), qApp->keyboardModifiers());
}

LocationBar::LocationBar(QWidget *parent)
    : KLineEdit(parent), d(new Private(this))
{
    QTimer::singleShot(0, this, SLOT(init()));
}

void LocationBar::init()
{
    // i18n: Placeholder text in line edit widgets is the text appearing
    // before any user input, briefly explaining to the user what to type
    // (e.g. "Enter search pattern").
    // By default the text is set in italic, which may not be appropriate
    // for some languages and scripts (e.g. for CJK ideographs).
    QString metaMsg = i18nc("Italic placeholder text in line edits: 0 no, 1 yes", "1");
    d->italicizePlaceholder = (metaMsg.trimmed() != QString('0'));

    setCompletionMode(KGlobalSettings::CompletionPopup);
    setClearButtonShown(true);
    setTrapReturnKey(true);

    // insert decoded URLs
    setUrlDropsEnabled(true);

    setClickMessage(i18n("Search Bookmarks and History"));

    // Setting up models
    d->m_unsortedModel = new PlacesProxyModel(this);
    d->m_model = new LocationBarCompletionModel(d->m_unsortedModel, this);
    connect(this, SIGNAL(textChanged(const QString &)),
        this, SLOT(updateWords(QString)));
    connect(this, SIGNAL(textChanged(const QString &)),
        d->m_unsortedModel, SLOT(setQuery(const QString &)));

    QCompleter *completer = new QCompleter(this);
    setCompleter(completer);
    completer->setModel(d->m_model);
    completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    completer->popup()->setItemDelegate(new LocationBarDelegate(this));
    connect(completer->popup()->selectionModel(),
        SIGNAL(currentChanged(QModelIndex,QModelIndex)), this,
        SLOT(slotCurrentCompletionChanged(QModelIndex)));

    connect(completer->popup(), SIGNAL(activated(QModelIndex)), this,
        SLOT(slotCompletionActivated(QModelIndex)));

    connect(this, SIGNAL(returnPressed(const QString &)),
        this, SLOT(slotReturnPressed(const QString &)));

//     addWidget(new LocationBarFaviconWidget(this), LeftSide);
}

LocationBar::~LocationBar()
{

}

void LocationBar::paintEvent(QPaintEvent* ev)
{
    QLineEdit::paintEvent(ev);
    QString text = this->text();

    // this is copied/adapted from KLineEdit::paintEvent
    QFontMetrics fm = fontMetrics();
    QString clickMessage = this->clickMessage();

    QPainter p(this);
    QFont f = font();
    f.setItalic(d->italicizePlaceholder);
    p.setFont(f);

    QStyleOptionFrame opt;
    initStyleOption(&opt);
    QRect cr = style()->subElementRect(QStyle::SE_LineEditContents, &opt, this);

    // this is copied/adapted from QLineEdit::paintEvent
    const int verticalMargin(1);
    const int horizontalMargin(2);

    int left, top, right, bottom;
    getTextMargins( &left, &top, &right, &bottom );
    cr.adjust( left, top, -right, -bottom );

    p.setClipRect(cr);
    Qt::Alignment va = alignment() & Qt::AlignVertical_Mask;
    int vscroll;
    switch (va & Qt::AlignVertical_Mask)
    {
        case Qt::AlignBottom:
        vscroll = cr.y() + cr.height() - fm.height() - verticalMargin;
        break;

        case Qt::AlignTop:
        vscroll = cr.y() + verticalMargin;
        break;

        default:
        vscroll = cr.y() + (cr.height() - fm.height() + 1) / 2;
        break;

    }

    QRect lineRect(cr.x() + horizontalMargin, vscroll, cr.width() - 2*horizontalMargin, fm.height());

    // Show click message only if it's not empty and space is available
    if (!clickMessage.isEmpty() &&
        fm.width(text) + fm.width(clickMessage) <= lineRect.width()) {
        QColor color(palette().color(foregroundRole()));
        color.setAlphaF(0.5);
        p.setPen(color);

        p.drawText(lineRect, Qt::AlignRight|Qt::AlignVCenter, clickMessage);
    }
}

void LocationBar::setURL(const QString &url)
{
    //TODO: see konqcombo.cpp KonqCombo::setURL() for ideas
    setText(url);
}

#include "locationbar.moc"
