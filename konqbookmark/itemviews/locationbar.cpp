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
#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QKeyEvent>
#include <QtGui/QTextBlock>
#include <QtGui/QScrollBar>

#include <KStandardShortcut>
#include <KUrlCompletion>
#include <KUrl>
#include <KLocale>
#include <KIconLoader>
#include <kacceleratormanager.h>

using namespace Konqueror;
using namespace Akonadi;



class LocationBar::Private
{
public:
    Private(LocationBar *parent);
    ~Private();

    void slotCompletionActivated(const QModelIndex &index);
    void slotTextChanged();
    void slotCurrentCompletionChanged(const QModelIndex &index);

    LocationBar *q;
    QString currentCompletion;
    bool italicizePlaceholder;
    PlacesProxyModel *unsortedModel;
    LocationBarCompletionModel *model;
    QAbstractItemView *view;
    QStringList words;
    QString clickMessage;
	QCompleter *completer;
};

LocationBar::Private::Private(LocationBar *parent)
    : q(parent)
{
}

LocationBar::Private::~Private()
{

}

void LocationBar::Private::slotTextChanged()
{
    QString text = q->text();
    words = text.split(" ", QString::SkipEmptyParts);
    unsortedModel->setQuery(text);
}

QStringList LocationBar::words() const
{
    return d->words;
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
    : QPlainTextEdit(parent), d(new Private(this))
{
    setMaximumHeight(25);
    QTimer::singleShot(0, this, SLOT(init()));
}

void LocationBar::init()
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setTabChangesFocus(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setLineWrapMode(NoWrap);
    setViewportMargins(30, 0, 0, 0);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    document()->setDocumentMargin(1);
 
    // i18n: Placeholder text in line edit widgets is the text appearing
    // before any user input, briefly explaining to the user what to type
    // (e.g. "Enter search pattern").
    // By default the text is set in italic, which may not be appropriate
    // for some languages and scripts (e.g. for CJK ideographs).
    QString metaMsg = i18nc("Italic placeholder text in line edits: 0 no, 1 yes", "1");
    d->italicizePlaceholder = (metaMsg.trimmed() != QString('0'));

//     setCompletionMode(KGlobalSettings::CompletionPopup);
//     setClearButtonShown(true);
//     setTrapReturnKey(true);

    // insert decoded URLs
//     setUrlDropsEnabled(true);

    setClickMessage(i18n("Search Bookmarks and History"));

    // Setting up models
    d->unsortedModel = new PlacesProxyModel(this);
    d->model = new LocationBarCompletionModel(d->unsortedModel, this);
    connect(this, SIGNAL(textChanged()),
        this, SLOT(slotTextChanged()));

    d->completer = new QCompleter(this);
    d->completer->setWidget(this);
    d->completer->setModel(d->model);
    d->completer->setCompletionRole(Place::PlaceUrlRole);
    d->completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    d->completer->popup()->setItemDelegate(new LocationBarDelegate(this));
    connect(d->completer->popup()->selectionModel(),
        SIGNAL(currentChanged(QModelIndex,QModelIndex)), this,
        SLOT(slotCurrentCompletionChanged(QModelIndex)));

    connect(d->completer->popup(), SIGNAL(activated(QModelIndex)), this,
        SLOT(slotCompletionActivated(QModelIndex)));

    // TODO: Why is this needed? who is setting the accels?
    KAcceleratorManager::setNoAccel(this);
    setFixedHeight(QFontMetrics(currentCharFormat().font()).height() + 8);
//     addWidget(new LocationBarFaviconWidget(this), LeftSide);
}

LocationBar::~LocationBar()
{

}

static void fillBackground(QPainter *p, const QRectF &rect, QBrush brush, QRectF gradientRect = QRectF())
{
    p->save();
    if (brush.style() >= Qt::LinearGradientPattern && brush.style() <= Qt::ConicalGradientPattern) {
        if (!gradientRect.isNull()) {
            QTransform m = QTransform::fromTranslate(gradientRect.left(), gradientRect.top());
            m.scale(gradientRect.width(), gradientRect.height());
            brush.setTransform(m);
            const_cast<QGradient *>(brush.gradient())->setCoordinateMode(QGradient::LogicalMode);
        }
    } else {
        p->setBrushOrigin(rect.topLeft());
    }
    p->fillRect(rect, brush);
    p->restore();
}

void LocationBar::paintEvent(QPaintEvent *e)
{
    QPainter painter(viewport());
    Q_ASSERT(qobject_cast<QPlainTextDocumentLayout*>(document()->documentLayout()));

    QPointF offset(contentOffset());

    QRect er = e->rect();
    QRect viewportRect = viewport()->rect();

    bool editable = !isReadOnly();

    QTextBlock block = firstVisibleBlock();
    qreal maximumWidth = document()->documentLayout()->documentSize().width();

    // Set a brush origin so that the WaveUnderline knows where the wave started
    painter.setBrushOrigin(offset);

    // keep right margin clean from full-width selection
    int maxX = offset.x() + qMax((qreal)viewportRect.width(), maximumWidth)
               - document()->documentMargin();
    er.setRight(qMin(er.right(), maxX));
    painter.setClipRect(er);

    QAbstractTextDocumentLayout::PaintContext context = getPaintContext();

    while (block.isValid()) {
        QRectF r = blockBoundingRect(block).translated(offset);
        QTextLayout *layout = block.layout();

        if (r.bottom() >= er.top() && r.top() <= er.bottom()) {

            QTextBlockFormat blockFormat = block.blockFormat();

            QBrush bg = blockFormat.background();

            if (bg != Qt::NoBrush) {
                QRectF contentsRect = r;
                contentsRect.setWidth(qMax(r.width(), maximumWidth));
                fillBackground(&painter, contentsRect, bg);
            }


            QVector<QTextLayout::FormatRange> selections;
            int blpos = block.position();
            int bllen = block.length();

            for (int i = 0; i < context.selections.size(); ++i) {
                const QAbstractTextDocumentLayout::Selection &range = context.selections.at(i);
                const int selStart = range.cursor.selectionStart() - blpos;
                const int selEnd = range.cursor.selectionEnd() - blpos;
                if (selStart < bllen && selEnd > 0
                    && selEnd > selStart) {
                    QTextLayout::FormatRange o;
                    o.start = selStart;
                    o.length = selEnd - selStart;
                    o.format = range.format;
                    selections.append(o);
                } else if (!range.cursor.hasSelection() && range.format.hasProperty(QTextFormat::FullWidthSelection)
                           && block.contains(range.cursor.position())) {
                    // for full width selections we don't require an actual selection, just
                    // a position to specify the line. that's more convenience in usage.
                    QTextLayout::FormatRange o;
                    QTextLine l = layout->lineForTextPosition(range.cursor.position() - blpos);
                    o.start = l.textStart();
                    o.length = l.textLength();
                    if (o.start + o.length == bllen - 1)
                        ++o.length; // include newline
                    o.format = range.format;
                    selections.append(o);
                }
            }

            bool drawCursor = (editable
                               && context.cursorPosition >= blpos
                               && context.cursorPosition < blpos + bllen);

            bool drawCursorAsBlock = drawCursor && overwriteMode() ;

            if (drawCursorAsBlock) {
                if (context.cursorPosition == blpos + bllen - 1) {
                    drawCursorAsBlock = false;
                } else {
                    QTextLayout::FormatRange o;
                    o.start = context.cursorPosition - blpos;
                    o.length = 1;
                    o.format.setForeground(palette().base());
                    o.format.setBackground(palette().text());
                    selections.append(o);
                }
            }


            layout->draw(&painter, offset, selections, er);
            if ((drawCursor && !drawCursorAsBlock)
                || (editable && context.cursorPosition < -1
                    && !layout->preeditAreaText().isEmpty())) {
                int cpos = context.cursorPosition;
                if (cpos < -1)
                    cpos = layout->preeditAreaPosition() - (cpos + 2);
                else
                    cpos -= blpos;
                layout->drawCursor(&painter, offset, cpos, cursorWidth());
            }
        }

        offset.ry() += r.height();
        if (offset.y() > viewportRect.height())
            break;
        block = block.next();
    }

    QFontMetrics fm = fontMetrics();
    QString clickMessage = this->clickMessage();
    QFont f = font();
    f.setItalic(d->italicizePlaceholder);
    painter.setFont(f);
    // Show click message only if it's not empty and space is available
    if (!clickMessage.isEmpty()
        && maximumWidth + fm.width(clickMessage) + 10 < viewportRect.width()) {
        QColor color(palette().color(foregroundRole()));
        color.setAlphaF(0.5);
        painter.setPen(color);
        viewportRect.setRight(viewportRect.right() - 4);

        painter.drawText(viewportRect, Qt::AlignRight|Qt::AlignVCenter, clickMessage);
    }

    if (backgroundVisible() && !block.isValid() && offset.y() <= er.bottom()
        && (centerOnScroll() || verticalScrollBar()->maximum() == verticalScrollBar()->minimum())) {
        painter.fillRect(QRect(QPoint((int)er.left(), (int)offset.y()), er.bottomRight()), palette().background());
    }
}

void LocationBar::keyPressEvent(QKeyEvent * e)
{   
    switch(e->key()){
        case Qt::Key_Escape:
            if(d->completer->popup()->isVisible()) {
                d->completer->popup()->hide();
            } else {
                selectAll();
            }
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if(d->completer->popup()->isVisible() && !static_cast<QListView*>(d->completer->popup())->selectionModel()->selectedRows().isEmpty()) { 
                setText(d->completer->currentCompletion());
                emit returnPressed(text(), qApp->keyboardModifiers());
            }
            d->completer->popup()->hide();
            break;
        default:
            QPlainTextEdit::keyPressEvent(e);
            if (e->modifiers() == Qt::NoModifier) {
                d->completer->setCompletionPrefix(text());
                d->completer->complete();
            }
            break;
    }
}

void LocationBar::setURL(const QString &url)
{
    //TODO: see konqcombo.cpp KonqCombo::setURL() for ideas
    setPlainText(url);
    // important security consideration: always display the beginning
    // of the url rather than its end to prevent spoofing attempts.
    moveCursor(QTextCursor::Start);
}

QString LocationBar::clickMessage() const
{
    return d->clickMessage;
}

void LocationBar::setClickMessage(const QString& clickMessage)
{
    d->clickMessage = clickMessage;
}

void LocationBar::setText(const QString& text)
{
    kDebug() << text;
    if (text.contains("&")) {
        kDebug() << "trap";
    }
    setPlainText(text);
}
QString LocationBar::text() const
{
    return toPlainText();
}

#include "locationbar.moc"
