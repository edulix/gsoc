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
#include "locationbarbutton.h"
#include "locationbarhighlighter.h"
#include "itemmodels/kcompletionmodel.h"
#include "itemmodels/konqbookmarkmodel.h"
#include "itemmodels/kaggregatedmodel.h"
#include "itemmodels/placesproxymodel.h"
#include "locationbarclearbutton.h"
#include "locationbarenterbutton.h"

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
#include <QtGui/QHBoxLayout>

#include <KStandardShortcut>
#include <KUrlCompletion>
#include <KUrl>
#include <KLocale>
#include <KIconLoader>
#include <KAcceleratorManager>

using namespace Konqueror;
using namespace Akonadi;

SideWidget::SideWidget(QWidget *parent)
    : QWidget(parent)
{
}

bool SideWidget::event(QEvent *event)
{
    if (event->type() == QEvent::LayoutRequest) {
        emit sizeHintChanged();
    }
    return QWidget::event(event);
}

void SideWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);
    QWidget::paintEvent(event);
}


class LocationBar::Private
{
public:
    Private(LocationBar *parent);
    ~Private();

    void slotCompletionActivated(const QModelIndex &index);
    void slotComplete();
    void slotCurrentCompletionChanged(const QModelIndex &index);

    void updateSideWidgetLocations();
    void updateTextMargins();

    LocationBar *q;
    LocationBarHighlighter *highlighter;
    QString currentCompletion;
    bool italicizePlaceholder;
    PlacesProxyModel *unsortedModel;
    LocationBarCompletionModel *model;
    QAbstractItemView *view;
    QStringList words;
    QString clickMessage;
	QCompleter *completer;
    QTimer completionTimer;

    SideWidget *m_leftWidget;
    SideWidget *m_rightWidget;
    QHBoxLayout *m_leftLayout;
    QHBoxLayout *m_rightLayout;
    LocationBarClearButton* clearButton;
};

LocationBar::Private::Private(LocationBar *parent)
    : q(parent)
{
}

LocationBar::Private::~Private()
{

}

QStringList LocationBar::words() const
{
    return d->words;
}

void LocationBar::Private::slotCurrentCompletionChanged(const QModelIndex &index)
{
    if(completer->popup()->selectionModel()->selectedIndexes().isEmpty()) {
        if (q->text() != completer->completionPrefix()) {
            q->setText(completer->completionPrefix());
            q->moveCursor(QTextCursor::End);
        }
        return;
    }
    q->setText(index.data().toString());
    q->moveCursor(QTextCursor::End);
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
    init();
}

void LocationBar::init()
{
    setFixedHeight(QFontMetrics(currentCharFormat().font()).height() + 8);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setTabChangesFocus(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setLineWrapMode(NoWrap);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    document()->setDocumentMargin(2);

    // i18n: Placeholder text in line edit widgets is the text appearing
    // before any user input, briefly explaining to the user what to type
    // (e.g. "Enter search pattern").
    // By default the text is set in italic, which may not be appropriate
    // for some languages and scripts (e.g. for CJK ideographs).
    QString metaMsg = i18nc("Italic placeholder text in line edits: 0 no, 1 yes", "1");
    d->italicizePlaceholder = (metaMsg.trimmed() != QString('0'));

//     setTrapReturnKey(true);

    // insert decoded URLs
//     setUrlDropsEnabled(true);

    setClickMessage(i18n("Search Bookmarks and History"));

    // Setting up models
    d->unsortedModel = new PlacesProxyModel(this);
    d->model = new LocationBarCompletionModel(d->unsortedModel, this);

    d->completer = new QCompleter(this);
    d->highlighter = new LocationBarHighlighter(document());
    d->completer->setWidget(this);
    d->completer->setModel(d->model);
    d->completer->setCompletionRole(Place::PlaceUrlRole);
    d->completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    d->completer->popup()->setItemDelegate(new LocationBarDelegate(this));
    d->completer->popup()->setFrameShape(Box);
    d->completer->popup()->setFrameShadow(Raised);
    connect(d->completer->popup()->selectionModel(),
        SIGNAL(currentChanged(QModelIndex,QModelIndex)), this,
        SLOT(slotCurrentCompletionChanged(QModelIndex)));
    connect(d->completer->popup(), SIGNAL(activated(QModelIndex)), this,
        SLOT(slotCompletionActivated(QModelIndex)));

    d->completionTimer.setInterval(200);
    d->completionTimer.setSingleShot(true);

    connect(this, SIGNAL(userTextChanged(QString)), &d->completionTimer, SLOT(start()));
    connect(&d->completionTimer, SIGNAL(timeout()), this, SLOT(slotComplete()));

    connect(this, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));

    // TODO: Why is this needed? who is setting the accels?
    KAcceleratorManager::setNoAccel(this);

    // Initialize code related to the subwidgets lineedit support
    d->m_leftWidget = new SideWidget(this);
    d->m_leftWidget->resize(0, 0);
    d->m_leftLayout = new QHBoxLayout(d->m_leftWidget);
    d->m_leftLayout->setContentsMargins(2, 2, 2, 2);
    d->m_leftLayout->setSizeConstraint(QLayout::SetFixedSize);

    d->m_rightWidget = new SideWidget(this);
    d->m_rightWidget->resize(0, 0);
    d->m_rightLayout = new QHBoxLayout(d->m_rightWidget);
    d->m_rightLayout->setContentsMargins(2, 2, 2, 2);
    d->m_rightLayout->setSizeConstraint(QLayout::SetFixedSize);

    if (isRightToLeft()) {
        d->m_leftLayout->setDirection(QBoxLayout::RightToLeft);
        d->m_rightLayout->setDirection(QBoxLayout::RightToLeft);
    } else {
        d->m_leftLayout->setDirection(QBoxLayout::LeftToRight);
        d->m_rightLayout->setDirection(QBoxLayout::LeftToRight);
    }

    connect(d->m_leftWidget, SIGNAL(sizeHintChanged()),
        this, SLOT(updateTextMargins()));
    connect(d->m_rightWidget, SIGNAL(sizeHintChanged()),
        this, SLOT(updateTextMargins()));

    addWidget(new LocationBarFaviconWidget(this), LeftSide);
    d->clearButton = 0;
    setClearButtonShown(true);
    addWidget(new LocationBarEnterButton(this), RightSide);
}

LocationBar::~LocationBar()
{

}

void LocationBar::slotTextChanged()
{
    kDebug() << text();
    emit textChanged(text());
}

void LocationBar::addWidget(QWidget *widget, WidgetPosition position)
{
    if (!widget) {
        return;
    }

    bool rtl = isRightToLeft();
    if (rtl) {
        position = (position == LeftSide) ? RightSide : LeftSide;
    }
    if (position == LeftSide) {
        d->m_leftLayout->addWidget(widget);
    } else {
        d->m_rightLayout->insertWidget(1, widget);
    }
}

void LocationBar::removeWidget(QWidget *widget)
{
    if (!widget) {
        return;
    }

    d->m_leftLayout->removeWidget(widget);
    d->m_rightLayout->removeWidget(widget);
    widget->hide();
}

int LocationBar::textMargin(WidgetPosition position) const
{
    int spacing = d->m_rightLayout->spacing();
    int w = 0;

    if (position == LeftSide) {
        w = d->m_leftWidget->sizeHint().width();
    } else {
        w = d->m_rightWidget->sizeHint().width();
    }

    if (w == 0) {
        return 0;
    }
    return w + spacing * 2;
}

void LocationBar::Private::updateTextMargins()
{
    int left = q->textMargin(LeftSide);
    int right = q->textMargin(RightSide);
    int top = 0;
    int bottom = 0;
    q->setViewportMargins(left, top, right, bottom);
    updateSideWidgetLocations();
}

void LocationBar::Private::updateSideWidgetLocations()
{
    QRect cr = q->contentsRect();

    if (m_leftLayout->count() > 0) {
        m_leftWidget->setGeometry(QRect(cr.left(), cr.top(), q->textMargin(LeftSide),
            cr.height()));
    }

    int right = q->textMargin(RightSide);
    m_rightWidget->setGeometry(QRect(cr.right() - right, cr.top(), right, cr.height()));
}

void LocationBar::resizeEvent(QResizeEvent * ev)
{
    d->clearButton->updateNeeded();
    d->updateSideWidgetLocations();

    QPlainTextEdit::resizeEvent(ev);
}

void LocationBar::setWidgetSpacing(int spacing)
{
    d->m_leftLayout->setSpacing(spacing);
    d->m_rightLayout->setSpacing(spacing);
    d->updateTextMargins();
}

int LocationBar::widgetSpacing() const
{
    return d->m_leftLayout->spacing();
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
    // Most of this code comes directly from QPlainTextEdit::paintEvent, but it's been modified
    // to display the click Message
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
}

void LocationBar::keyPressEvent(QKeyEvent * e)
{
    int key = e->key();
    switch(key){
        case Qt::Key_Escape:
            if(d->completer->popup()->isVisible()) {
                setText(d->completer->completionPrefix());
                moveCursor(QTextCursor::End);
                d->completer->popup()->hide();
            } else {
                selectAll();
            }
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            emit returnPressed(text(), qApp->keyboardModifiers());
            d->completer->popup()->hide();
            break;
        case Qt::Key_Up:
        case Qt::Key_Down:
            if(!d->completer->popup()->isVisible()) {
                d->completer->popup()->show();
            }
        default:
            QString s1 = text();
            QPlainTextEdit::keyPressEvent(e);
            if (text().isEmpty()) {
                d->completer->popup()->hide();
                break;
            }
            if (s1 != text()) {
                emit userTextChanged(text());
            }
            break;
    }
}

void LocationBar::Private::slotComplete()
{
    completer->setCompletionPrefix(q->text());
    QString text = q->text();
    words = text.split(" ", QString::SkipEmptyParts);
    unsortedModel->setQuery(text);
    completer->complete();
}


void LocationBar::setURL(const QString &url)
{
    //TODO: see konqcombo.cpp KonqCombo::setURL() for ideas
    setPlainText(url);
    // important security consideration: always display the beginning
    // of the url rather than its end to prevent spoofing attempts.
    moveCursor(QTextCursor::Start);
    emit urlChanged(url);
}

int LocationBar::completionDelay() const
{
    return d->completionTimer.interval();
}

void LocationBar::setCompletionDelay(int miliseconds)
{
    d->completionTimer.setInterval(miliseconds);
}


QString LocationBar::clickMessage() const
{
    return d->clickMessage;
}

void LocationBar::setClickMessage(const QString& clickMessage)
{
    d->clickMessage = clickMessage;
}

void LocationBar::setClearButtonShown(bool show)
{
    if (show) {
        if (d->clearButton) {
            return;
        }

        d->clearButton = new LocationBarClearButton(this);
        addWidget(d->clearButton, RightSide);
    } else {
        delete d->clearButton;
        d->clearButton = 0;
    }
}

bool LocationBar::isClearButtonShown() const
{
    return d->clearButton != 0;
}

void LocationBar::setReadOnly(bool ro)
{
    QPlainTextEdit::setReadOnly(ro);
    emit readOnlyChanged(ro);
}

void LocationBar::setText(const QString& text)
{
    setPlainText(text);
}

QString LocationBar::text() const
{
    return toPlainText();
}

void LocationBar::focusInEvent(QFocusEvent* e)
{
    QPlainTextEdit::focusInEvent(e);
    if (e->reason() & (Qt::TabFocusReason | Qt::BacktabFocusReason)) {
        selectAll();
    }
}

void LocationBar::focusOutEvent(QFocusEvent* e)
{
    setTextCursor(QTextCursor(document()));
    QPlainTextEdit::focusOutEvent(e);
}

#include "locationbar.moc"
