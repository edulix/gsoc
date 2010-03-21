/* This file is part of the KDE libraries

   Copyright (C) 1997 Sven Radej (sven.radej@iname.com)
   Copyright (c) 1999 Patrick Ward <PAT_WARD@HP-USA-om5.om.hp.com>
   Copyright (c) 1999 Preston Brown <pbrown@kde.org>

   Re-designed for KDE 2.x by
   Copyright (c) 2000, 2001 Dawit Alemayehu <adawit@kde.org>
   Copyright (c) 2000, 2001 Carsten Pfeiffer <pfeiffer@kde.org>

   Copyright (c) 2009 Benjamin C. Meyer  <ben@meyerhome.net>
   Copyright (c) 2009 Eduardo Robles Elvira <edulix@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License (LGPL) as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later
   version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "kcompletionview.h"
#include "klineeditview.h"
#include "klineeditview_p.h"
#include "klineeditviewbutton.h"
#include "kdeuiwidgetsproxystyle_p.h"

#include <kaction.h>
#include <kapplication.h>
#include <kauthorized.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kcursor.h>
#include <kdebug.h>
#include <kicontheme.h>
#include <kicon.h>
#include <klocale.h>
#include <kmenu.h>
#include <kstandardaction.h>
#include <kstandardshortcut.h>

#include <QtCore/QTimer>
#include <QtGui/QClipboard>
#include <QtGui/QStyleOption>
#include <QtGui/QToolTip>
#include <QtGui/QSpacerItem>
#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>

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

class KLineEditView::Private
{
public:
    Private(KLineEditView* qq)
        : q(qq)
    {
        completionView = 0L;
        handleURLDrops = true;
        grabReturnKeyEvents = false;

        userSelection = true;
        autoSuggest = false;
        disableRestoreSelection = false;
        enableSqueezedText = false;

        drawClickMsg = false;
        enableClickMsg = false;
        threeStars = false;
        completionRunning = false;
        if (!initialized) {
            KConfigGroup config(KGlobal::config(), "General");
            backspacePerformsCompletion = config.readEntry("Backspace performs completion", false);

            initialized = true;
        }

        clearButton = 0;
        clickInClear = false;

        // i18n: Placeholder text in line edit widgets is the text appearing
        // before any user input, briefly explaining to the user what to type
        // (e.g. "Enter search pattern").
        // By default the text is set in italic, which may not be appropriate
        // for some languages and scripts (e.g. for CJK ideographs).
        QString metaMsg = i18nc("Italic placeholder text in line edits: 0 no, 1 yes", "1");
        italicizePlaceholder = (metaMsg.trimmed() != QString('0'));


        // Initialize code related to the subwidgets lineedit support
        m_leftWidget = new SideWidget(q);
        m_leftWidget->resize(0, 0);
        m_leftLayout = new QHBoxLayout(m_leftWidget);
        m_leftLayout->setContentsMargins(0, 0, 0, 0);
        m_leftLayout->setSizeConstraint(QLayout::SetFixedSize);

        m_rightWidget = new SideWidget(q);
        m_rightWidget->resize(0, 0);
        m_rightLayout = new QHBoxLayout(m_rightWidget);
        m_rightLayout->setContentsMargins(0, 0, 0, 0);

        if (q->isRightToLeft()) {
            m_leftLayout->setDirection(QBoxLayout::RightToLeft);
            m_rightLayout->setDirection(QBoxLayout::RightToLeft);
        } else {
            m_leftLayout->setDirection(QBoxLayout::LeftToRight);
            m_rightLayout->setDirection(QBoxLayout::LeftToRight);
        }

        QSpacerItem *horizontalSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
        m_rightLayout->addItem(horizontalSpacer);

        connect(m_leftWidget, SIGNAL(sizeHintChanged()),
            q, SLOT(updateTextMargins()));
        connect(m_rightWidget, SIGNAL(sizeHintChanged()),
            q, SLOT(updateTextMargins()));

    }

    ~Private()
    {
    }

    void _k_slotSettingsChanged(int category)
    {
        Q_UNUSED(category);

        if (clearButton) {
            clearButton->setAnimationsEnabled(KGlobalSettings::graphicEffectsLevel() & KGlobalSettings::SimpleAnimationEffects);
        }
    }

    void _k_updateUserText(const QString &txt)
    {
        if ((!completionRunning) && (txt != userText)) {
            userText = txt;
            emit q->userTextChanged(txt);
        }
    }

    /**
     * Checks whether we should/should not consume a key used as a shortcut.
     * This makes it possible to handle shortcuts in the focused widget before any
     * window-global QAction is triggered.
     */
    bool overrideShortcut(const QKeyEvent* e);

    static bool initialized;
    static bool backspacePerformsCompletion; // Configuration option

    QColor previousHighlightColor;
    QColor previousHighlightedTextColor;

    bool userSelection: 1;
    bool autoSuggest : 1;
    bool disableRestoreSelection: 1;
    bool handleURLDrops:1;
    bool grabReturnKeyEvents:1;
    bool enableSqueezedText:1;
    bool completionRunning:1;

    int squeezedEnd;
    int squeezedStart;
    QPalette::ColorRole bgRole;
    QString squeezedText;
    QString userText;

    QString clickMessage;
    bool enableClickMsg:1;
    bool drawClickMsg:1;
    bool threeStars:1;

    bool possibleTripleClick :1;  // set in mousePressEvent, deleted in tripleClickTimeout

    bool clickInClear:1;
    KLineEditViewButton *clearButton;

    KCompletionView *completionView;

    bool italicizePlaceholder:1;

    QAction *noCompletionAction, *shellCompletionAction, *autoCompletionAction, *popupCompletionAction, *shortAutoCompletionAction, *popupAutoCompletionAction, *defaultAction;

    QMap<KGlobalSettings::Completion, bool> disableCompletionMap;
    KLineEditView* q;

    SideWidget *m_leftWidget;
    SideWidget *m_rightWidget;
    QHBoxLayout *m_leftLayout;
    QHBoxLayout *m_rightLayout;
};

bool KLineEditView::Private::backspacePerformsCompletion = false;
bool KLineEditView::Private::initialized = false;


KLineEditView::KLineEditView(const QString &string, QWidget *parent)
    : QLineEdit(string, parent), d(new KLineEditView::Private(this))
{
    init();
}

KLineEditView::KLineEditView(QWidget *parent)
    : QLineEdit(parent), d(new Private(this))
{
    init();
}


KLineEditView::~KLineEditView ()
{
    delete d;
}

void KLineEditView::init()
{
    d->possibleTripleClick = false;
    d->bgRole = backgroundRole();
    d->m_leftLayout->setSpacing(3);
    d->m_rightLayout->setSpacing(3);
    updateTextMargins();

    // Enable the context menu by default.
    QLineEdit::setContextMenuPolicy(Qt::DefaultContextMenu);
    KCursor::setAutoHideCursor(this, true, true);
    KCompletionBase::setHandleSignals(true);
    KCompletionBase::setEnableSignals(true);

    KGlobalSettings::Completion mode = completionMode();
    d->autoSuggest = (mode == KGlobalSettings::CompletionMan ||
                      mode == KGlobalSettings::CompletionPopupAuto ||
                      mode == KGlobalSettings::CompletionAuto);
    connect(this, SIGNAL(selectionChanged()), this, SLOT(slotRestoreSelectionColors()));

    connect(KGlobalSettings::self(), SIGNAL(settingsChanged(int)), this, SLOT(_k_slotSettingsChanged(int)));

    const QPalette p = palette();
    if (!d->previousHighlightedTextColor.isValid()) {
        d->previousHighlightedTextColor=p.color(QPalette::Normal,QPalette::HighlightedText);
    }
    if (!d->previousHighlightColor.isValid()) {
        d->previousHighlightColor=p.color(QPalette::Normal,QPalette::Highlight);
    }

    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(_k_updateUserText(const QString&)));

}

void KLineEditView::addWidget(QWidget *widget, WidgetPosition position)
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

void KLineEditView::removeWidget(QWidget *widget)
{
    if (!widget) {
        return;
    }

    d->m_leftLayout->removeWidget(widget);
    d->m_rightLayout->removeWidget(widget);
    widget->hide();
}

int KLineEditView::textMargin(WidgetPosition position) const
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

void KLineEditView::updateTextMargins()
{
    int left = textMargin(LeftSide);
    int right = textMargin(RightSide);
    int top = 0;
    int bottom = 0;
    setTextMargins(left, top, right, bottom);
    updateSideWidgetLocations();
}

void KLineEditView::updateSideWidgetLocations()
{
    QStyleOptionFrameV2 opt;
    initStyleOption(&opt);
    QRect textRect = style()->subElementRect(QStyle::SE_LineEditContents, &opt, this);
    int spacing = d->m_rightLayout->spacing();
    textRect.adjust(spacing, 0, -spacing, 0);

    int left = textMargin(LeftSide);
    int midHeight = textRect.center().y() + 1;

    if (d->m_leftLayout->count() > 0) {
        int leftHeight = midHeight - d->m_leftWidget->height() / 2;
        int leftWidth = d->m_leftWidget->width();
        if (leftWidth == 0) {
            leftHeight = midHeight - d->m_leftWidget->sizeHint().height() / 2;
        }
        d->m_leftWidget->move(textRect.x(), leftHeight);
    }
    textRect.setX(left);
    textRect.setY(midHeight - d->m_rightWidget->sizeHint().height() / 2);
    textRect.setHeight(d->m_rightWidget->sizeHint().height());
    d->m_rightWidget->setGeometry(textRect);
}

QString KLineEditView::clickMessage() const
{
    return d->clickMessage;
}

void KLineEditView::setClearButtonShown(bool show)
{
    if (show) {
        if (d->clearButton) {
            return;
        }

        d->clearButton = new KLineEditViewButton(this);
        d->clearButton->setCursor(Qt::ArrowCursor);
        d->clearButton->setToolTip(i18nc("@action:button Clear current text in the line edit", "Clear text"));
        addWidget(d->clearButton, RightSide);

        updateClearButtonIcon(text());
        connect(this, SIGNAL(textChanged(QString)), this, SLOT(updateClearButtonIcon(QString)));
    } else {
        disconnect(this, SIGNAL(textChanged(QString)), this, SLOT(updateClearButtonIcon(QString)));
        removeWidget(d->clearButton);
        delete d->clearButton;
        d->clearButton = 0;
        d->clickInClear = false;
    }
}

bool KLineEditView::isClearButtonShown() const
{
    return d->clearButton != 0;
}

QSize KLineEditView::clearButtonUsedSize() const
{
    QSize s;
    if (d->clearButton) {
        const int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, this);
        s = d->clearButton->sizeHint();
        s.rwidth() += frameWidth;
    }
    return s;
}

void KLineEditView::updateClearButtonIcon(const QString& text)
{
    kDebug();
    if (!d->clearButton || isReadOnly()) {
        return;
    }

    int clearButtonState = KIconLoader::DefaultState;

    if (text.length() > 0) {
        d->clearButton->animateVisible(true);
    } else {
        d->clearButton->animateVisible(false);
    }

    if (!d->clearButton->pixmap().isNull()) {
        return;
    }

    if (layoutDirection() == Qt::LeftToRight) {
        d->clearButton->setPixmap(SmallIcon("edit-clear-locationbar-rtl", 0, clearButtonState));
    } else {
        d->clearButton->setPixmap(SmallIcon("edit-clear-locationbar-ltr", 0, clearButtonState));
    }

    d->clearButton->setVisible(text.length());
}

void KLineEditView::setCompletionMode(KGlobalSettings::Completion mode)
{
    KGlobalSettings::Completion oldMode = completionMode();

    if (oldMode != mode && (oldMode == KGlobalSettings::CompletionPopup ||
        oldMode == KGlobalSettings::CompletionPopupAuto) &&
        d->completionView && d->completionView->isVisible()) {
        d->completionView->hide();
    }

    // If the widgets echo mode is not Normal, no completion
    // feature will be enabled even if one is requested.
    if (echoMode() != QLineEdit::Normal) {
        mode = KGlobalSettings::CompletionNone; // Override the request.
    }

    if (kapp && !KAuthorized::authorize("lineedit_text_completion")) {
        mode = KGlobalSettings::CompletionNone;
    }

    if (mode == KGlobalSettings::CompletionPopupAuto ||
        mode == KGlobalSettings::CompletionAuto ||
        mode == KGlobalSettings::CompletionMan) {
        d->autoSuggest = true;
    } else {
        d->autoSuggest = false;
    }

    KCompletionBase::setCompletionMode(mode);
}

void KLineEditView::setCompletionModeDisabled(KGlobalSettings::Completion mode, bool disable)
{
    d->disableCompletionMap[ mode ] = disable;
}

void KLineEditView::setCompletedText(const QString& t, bool marked)
{
    if (!d->autoSuggest) {
      return;
    }

    const QString txt = text();

    if (t != txt) {

        const bool blocked = blockSignals(true);
        setText(t);
        blockSignals(blocked);

        if (marked) {
            setSelection(t.length(), txt.length()-t.length());
        }
        setUserSelection(false);
    } else {
        setUserSelection(true);
    }

}

void KLineEditView::setCompletedText(const QString& text)
{
    KGlobalSettings::Completion mode = completionMode();
    const bool marked = (mode == KGlobalSettings::CompletionAuto ||
                    mode == KGlobalSettings::CompletionMan ||
                    mode == KGlobalSettings::CompletionPopup ||
                    mode == KGlobalSettings::CompletionPopupAuto);
    setCompletedText(text, marked);
}

void KLineEditView::rotateText(KCompletionBase::KeyBindingType type)
{
    if (d->completionView && d->completionView->model() &&
        d->completionView->model()->hasChildren() &&
        (type == KCompletionBase::PrevCompletionMatch ||
        type == KCompletionBase::NextCompletionMatch)) {
        QString input;
        QItemSelectionModel* selModel = d->completionView->selectionModel();

        input = selModel->selectedIndexes().first().data().toString();
        kDebug() << input;

        if (d->completionView->currentIndex() == selModel->selectedIndexes().first() ||
            input.isEmpty() || input == displayText()) {
            return;
        }

        d->autoSuggest = true;
        setCompletedText(input, hasSelectedText());
    }
}

void KLineEditView::makeCompletion(const QString& text)
{
    KGlobalSettings::Completion mode = completionMode();

    if (!d->completionView || !d->completionView->model() || mode == KGlobalSettings::CompletionNone) {
        return;  // No completion object...
    }

    QAbstractItemModel* model = d->completionView->model();
    const QString match = model->index(0,0).data().toString();

    if (mode == KGlobalSettings::CompletionPopup ||
         mode == KGlobalSettings::CompletionPopupAuto) {
        if (match.isEmpty()) {
            if (d->completionView) {
                d->completionView->hide();
            }
        } else {
            updateCompletedItems();
        }
    } else {
        // Auto,  ShortAuto (Man) and Shell
        // all other completion modes
        // If no match or the same match, simply return without completing.
        if (match.isEmpty() || match == text) {
            return;
        }

        if (mode != KGlobalSettings::CompletionShell) {
            setUserSelection(false);
        }

        if (d->autoSuggest) {
            setCompletedText(match);
        }
    }
}

void KLineEditView::setReadOnly(bool readOnly)
{
    // Do not do anything if nothing changed...
    if (readOnly == isReadOnly()) {
      return;
    }

    QLineEdit::setReadOnly(readOnly);

    if (readOnly) {
        d->bgRole = backgroundRole();
        setBackgroundRole(QPalette::Window);
        if (d->enableSqueezedText && d->squeezedText.isEmpty()) {
            d->squeezedText = text();
            setSqueezedText();
        }

        if (d->clearButton) {
            d->clearButton->animateVisible(false);
        }
    } else {
        if (!d->squeezedText.isEmpty()) {
           setText(d->squeezedText);
           d->squeezedText.clear();
        }

        setBackgroundRole(d->bgRole);
    }
}

void KLineEditView::setSqueezedText(const QString &text)
{
    setSqueezedTextEnabled(true);
    setText(text);
}

void KLineEditView::setSqueezedTextEnabled(bool enable)
{
    d->enableSqueezedText = enable;
}

bool KLineEditView::isSqueezedTextEnabled() const
{
    return d->enableSqueezedText;
}

void KLineEditView::setText(const QString& text)
{
    if (d->enableClickMsg) {
          d->drawClickMsg = text.isEmpty();
          update();
    }
    if (d->enableSqueezedText && isReadOnly()) {
        d->squeezedText = text;
        setSqueezedText();
        return;
    }

    QLineEdit::setText(text);
}

void KLineEditView::setSqueezedText()
{
    d->squeezedStart = 0;
    d->squeezedEnd = 0;
    const QString fullText = d->squeezedText;
    const QFontMetrics fm(fontMetrics());
    const int labelWidth = size().width() - 2*style()->pixelMetric(QStyle::PM_DefaultFrameWidth) - 2;
    const int textWidth = fm.width(fullText);

    if (textWidth > labelWidth) {
        // start with the dots only
        QString squeezedText = "...";
        int squeezedWidth = fm.width(squeezedText);

        // estimate how many letters we can add to the dots on both sides
        int letters = fullText.length() * (labelWidth - squeezedWidth) / textWidth / 2;
        squeezedText = fullText.left(letters) + "..." + fullText.right(letters);
        squeezedWidth = fm.width(squeezedText);

        if (squeezedWidth < labelWidth) {
            // we estimated too short
            // add letters while text < label
            do {
                letters++;
                squeezedText = fullText.left(letters) + "..." + fullText.right(letters);
                squeezedWidth = fm.width(squeezedText);
            } while (squeezedWidth < labelWidth);

            letters--;
            squeezedText = fullText.left(letters) + "..." + fullText.right(letters);
        } else if (squeezedWidth > labelWidth) {
            // we estimated too long
            // remove letters while text > label
            do {
                letters--;
                squeezedText = fullText.left(letters) + "..." + fullText.right(letters);
                squeezedWidth = fm.width(squeezedText);
            } while (squeezedWidth > labelWidth);
        }

        if (letters < 5) {
            // too few letters added -> we give up squeezing
            QLineEdit::setText(fullText);
        } else {
            QLineEdit::setText(squeezedText);
            d->squeezedStart = letters;
            d->squeezedEnd = fullText.length() - letters;
        }
        setToolTip(fullText);

    } else {
      QLineEdit::setText(fullText);

      this->setToolTip("");
      QToolTip::showText(pos(), QString()); // hide
    }

    setCursorPosition(0);
}

void KLineEditView::copy() const
{
    if (!copySqueezedText(true)) {
        QLineEdit::copy();
    }
}

bool KLineEditView::copySqueezedText(bool clipboard) const
{
    if (!d->squeezedText.isEmpty() && d->squeezedStart) {
        KLineEditView *that = const_cast<KLineEditView *>(this);
        if (!that->hasSelectedText()) {
            return false;
        }

        int start = selectionStart(), end = start + selectedText().length();
        if (start >= d->squeezedStart+3) {
            start = start - 3 - d->squeezedStart + d->squeezedEnd;
        } else if (start > d->squeezedStart) {
            start = d->squeezedStart;
        }

        if (end >= d->squeezedStart+3) {
            end = end - 3 - d->squeezedStart + d->squeezedEnd;
        } else if (end > d->squeezedStart) {
            end = d->squeezedEnd;
        }

        if (start == end) {
            return false;
        }
        QString t = d->squeezedText;
        t = t.mid(start, end - start);
        disconnect(QApplication::clipboard(), SIGNAL(selectionChanged()), this, 0);
        QApplication::clipboard()->setText(t, clipboard ? QClipboard::Clipboard : QClipboard::Selection);
        connect(QApplication::clipboard(), SIGNAL(selectionChanged()), this,
            SLOT(_q_clipboardChanged()));
        return true;
    }
    return false;
}

void KLineEditView::resizeEvent(QResizeEvent * ev)
{
    updateSideWidgetLocations();
    if (!d->squeezedText.isEmpty()) {
        setSqueezedText();
    }

    QLineEdit::resizeEvent(ev);
}


void KLineEditView::keyPressEvent(QKeyEvent *e)
{
    kDebug() << "key" << e->text() << KStandardShortcut::shortcut(KStandardShortcut::PrevCompletion).primary().toString();
    const int key = e->key() | e->modifiers();

    if (KStandardShortcut::copy().contains(key)) {
        copy();
        return;
    } else if (KStandardShortcut::paste().contains(key)) {
        // TODO:
        // we should restore the original text (not autocompleted), otherwise the paste
        // will get into troubles Bug: 134691
        if (!isReadOnly()) {
            paste();
        }
        return;
    } else if (KStandardShortcut::pasteSelection().contains(key)) {
        QString text = QApplication::clipboard()->text(QClipboard::Selection);
        insert(text);
        deselect();
        return;
    } else if (KStandardShortcut::cut().contains(key)) {
        if (!isReadOnly()) {
            cut();
        }
        return;
    } else if (KStandardShortcut::undo().contains(key)) {
        if (!isReadOnly()) {
            undo();
        }
        return;
    } else if (KStandardShortcut::redo().contains(key)) {
        if (!isReadOnly()) {
            redo();
        }
        return;
    } else if (KStandardShortcut::deleteWordBack().contains(key)) {
        cursorWordBackward(true);
        if (hasSelectedText()) {
            del();
        }

        e->accept();
        return;
    } else if (KStandardShortcut::deleteWordForward().contains(key)) {
        // Workaround for QT bug where
        cursorWordForward(true);
        if (hasSelectedText())
            del();

        e->accept();
        return;
    } else if (KStandardShortcut::backwardWord().contains(key)) {
      cursorWordBackward(false);
      e->accept();
      return;
    } else if (KStandardShortcut::forwardWord().contains(key)) {
      cursorWordForward(false);
      e->accept();
      return;
    } else if (KStandardShortcut::beginningOfLine().contains(key)) {
      home(false);
      e->accept();
      return;
    } else if (KStandardShortcut::endOfLine().contains(key)) {
      end(false);
      e->accept();
      return;
    }

    // Filter key-events if EchoMode is normal and
    // completion mode is not set to CompletionNone
    if (echoMode() == QLineEdit::Normal &&
        completionMode() != KGlobalSettings::CompletionNone) {
        const KeyBindingMap keys = getKeyBindings();
        const KGlobalSettings::Completion mode = completionMode();
        const bool noModifier = (e->modifiers() == Qt::NoButton ||
            e->modifiers() == Qt::ShiftModifier ||
            e->modifiers() == Qt::KeypadModifier);

        if ((mode == KGlobalSettings::CompletionAuto ||
            mode == KGlobalSettings::CompletionPopupAuto ||
            mode == KGlobalSettings::CompletionMan) && noModifier) {
            if (!d->userSelection && hasSelectedText() &&
                (e->key() == Qt::Key_Right || e->key() == Qt::Key_Left) &&
                e->modifiers() == Qt::NoButton) {
                const QString old_txt = text();
                d->disableRestoreSelection = true;
                const int start = selectionStart();

                deselect();
                QLineEdit::keyPressEvent(e);
                const int cPosition = cursorPosition();
                setText(old_txt);

                // keep cursor at cPosition
                setSelection(old_txt.length(), cPosition - old_txt.length());
                if (e->key() == Qt::Key_Right && cPosition > start) {
                    //the user explicitly accepted the autocompletion
                    d->_k_updateUserText(text());
                }

                d->disableRestoreSelection = false;
                return;
            }

            if (e->key() == Qt::Key_Escape) {
                if (hasSelectedText() && !d->userSelection) {
                    del();
                    setUserSelection(true);
                }

                // Don't swallow the Escape press event for the case
                // of dialogs, which have Escape associated to Cancel
                e->ignore();
                return;
            }
        }

        if ((mode == KGlobalSettings::CompletionAuto ||
            mode == KGlobalSettings::CompletionMan) && noModifier) {
            const QString keycode = e->text();
            if (!keycode.isEmpty() && (keycode.unicode()->isPrint() ||
                e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Delete)) {
                const bool hasUserSelection=d->userSelection;
                const bool hadSelection=hasSelectedText();

                bool cursorNotAtEnd=false;

                const int start = selectionStart();
                const int cPos = cursorPosition();

                // When moving the cursor, we want to keep the autocompletion as an
                // autocompletion, so we want to process events at the cursor position
                // as if there was no selection. After processing the key event, we
                // can set the new autocompletion again.
                if (hadSelection && !hasUserSelection && start>cPos) {
                    del();
                    setCursorPosition(cPos);
                    cursorNotAtEnd=true;
                }

                d->disableRestoreSelection = true;
                QLineEdit::keyPressEvent (e);
                d->disableRestoreSelection = false;

                QString txt = text();
                int len = txt.length();
                if (!hasSelectedText() && len /*&& cursorPosition() == len */) {
                    if (e->key() == Qt::Key_Backspace) {
                        if (hadSelection && !hasUserSelection && !cursorNotAtEnd) {
                            backspace();
                            txt = text();
                            len = txt.length();
                        }

                        if (!d->backspacePerformsCompletion || !len) {
                            d->autoSuggest = false;
                        }
                    }

                    if (e->key() == Qt::Key_Delete) {
                        d->autoSuggest=false;
                    }

                    doCompletion(txt);

                    if ((e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Delete)) {
                        d->autoSuggest=true;
                    }

                    e->accept();
                }
                return;
            }
        } else if ((mode == KGlobalSettings::CompletionPopup ||
            mode == KGlobalSettings::CompletionPopupAuto) &&
            noModifier && !e->text().isEmpty()) {
            const QString old_txt = text();
            const bool hasUserSelection=d->userSelection;
            const bool hadSelection=hasSelectedText();
            bool cursorNotAtEnd=false;

            const int start = selectionStart();
            const int cPos = cursorPosition();
            const QString keycode = e->text();

            // When moving the cursor, we want to keep the autocompletion as an
            // autocompletion, so we want to process events at the cursor position
            // as if there was no selection. After processing the key event, we
            // can set the new autocompletion again.
            if (hadSelection && !hasUserSelection && start>cPos &&
                ((!keycode.isEmpty() && keycode.unicode()->isPrint()) ||
                e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Delete)) {
                del();
                setCursorPosition(cPos);
                cursorNotAtEnd=true;
            }

            const int selectedLength=selectedText().length();

            d->disableRestoreSelection = true;
            QLineEdit::keyPressEvent(e);
            d->disableRestoreSelection = false;

            if ((selectedLength != selectedText().length()) && !hasUserSelection) {
                slotRestoreSelectionColors(); // and set userSelection to true
            }

            QString txt = text();
            int len = txt.length();
            if ((txt != old_txt || txt != e->text()) && len/* && (cursorPosition() == len || force)*/ &&
                ((!keycode.isEmpty() && keycode.unicode()->isPrint()) ||
                e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Delete)) {
                if (e->key() == Qt::Key_Backspace) {
                    if (hadSelection && !hasUserSelection && !cursorNotAtEnd) {
                        backspace();
                        txt = text();
                        len = txt.length();
                    }

                    if (!d->backspacePerformsCompletion) {
                        d->autoSuggest = false;
                    }
                }

                if (e->key() == Qt::Key_Delete) {
                    d->autoSuggest=false;
                }

                if (d->completionView) {
                    kDebug() << "setCancelledText" << txt;
                    d->completionView->setCancelledText(txt);
                }

                doCompletion(txt);

                if ((e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Delete) &&
                    mode == KGlobalSettings::CompletionPopupAuto) {
                    d->autoSuggest=true;
                }

                e->accept();
            } else if (!len && d->completionView && d->completionView->isVisible()) {
                d->completionView->hide();
            }

            return;
        } else if (mode == KGlobalSettings::CompletionShell) {
            // Handles completion.
            KShortcut cut;
            if (keys[TextCompletion].isEmpty()) {
                cut = KStandardShortcut::shortcut(KStandardShortcut::TextCompletion);
            } else {
                cut = keys[TextCompletion];
            }

            if (cut.contains(key)) {
                // Emit completion if the completion mode is CompletionShell
                // and the cursor is at the end of the string.
                const QString txt = text();
                const int len = txt.length();
                if (cursorPosition() == len && len != 0) {
                    doCompletion(txt);
                    return;
                }
            } else if (d->completionView) {
                d->completionView->hide();
            }
        }
    }
    const int selectedLength = selectedText().length();

    // Let QLineEdit handle any other keys events.
    QLineEdit::keyPressEvent (e);

    if (selectedLength != selectedText().length()) {
        slotRestoreSelectionColors(); // and set userSelection to true
    }
}

void KLineEditView::mouseDoubleClickEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) {
        d->possibleTripleClick=true;
        QTimer::singleShot(QApplication::doubleClickInterval(),this,
            SLOT(tripleClickTimeout()));
    }
    QLineEdit::mouseDoubleClickEvent(e);
}

void KLineEditView::mousePressEvent(QMouseEvent* e)
{
    if  ((e->button() == Qt::LeftButton ||
        e->button() == Qt::MidButton) &&
        d->clearButton) {
        d->clickInClear = d->clearButton->underMouse();

        if (d->clickInClear) {
            d->possibleTripleClick = false;
        }
    }

    if (e->button() == Qt::LeftButton && d->possibleTripleClick) {
        selectAll();
        e->accept();
        return;
    }

    QLineEdit::mousePressEvent(e);
}

void KLineEditView::mouseReleaseEvent(QMouseEvent* e)
{
    if (d->clickInClear) {
        if (d->clearButton->underMouse()) {
            QString newText;
            if (e->button() == Qt::MidButton) {
                newText = QApplication::clipboard()->text(QClipboard::Selection);
                setText(newText);
            } else {
                setSelection(0, text().size());
                del();
                emit clearButtonClicked();
            }
            emit textChanged(newText);
        }

        d->clickInClear = false;
        e->accept();
        return;
    }

    QLineEdit::mouseReleaseEvent(e);

   if (QApplication::clipboard()->supportsSelection()) {
       if (e->button() == Qt::LeftButton) {
            // Fix copying of squeezed text if needed
            copySqueezedText(false);
       }
   }
}

void KLineEditView::tripleClickTimeout()
{
    d->possibleTripleClick=false;
}

QMenu* KLineEditView::createStandardContextMenu()
{
    QMenu *popup = QLineEdit::createStandardContextMenu();

    if (!isReadOnly()) {
        // FIXME: This code depends on Qt's action ordering.
        const QList<QAction *> actionList = popup->actions();
        enum { UndoAct, RedoAct, Separator1, CutAct, CopyAct, PasteAct, DeleteAct, ClearAct,
               Separator2, SelectAllAct, NCountActs };
        QAction *separatorAction = 0L;
        // separator we want is right after Delete right now.
        const int idx = actionList.indexOf(actionList[DeleteAct]) + 1;
        if (idx < actionList.count()) {
            separatorAction = actionList.at(idx);
        }
        if (separatorAction) {
            KAction *clearAllAction = KStandardAction::clear(this, SLOT(clear()), this) ;
            if (text().isEmpty()) {
                clearAllAction->setEnabled(false);
            }
            popup->insertAction(separatorAction, clearAllAction);
        }
    }

    KIconTheme::assignIconsToContextMenu(KIconTheme::TextEditor, popup->actions ());

    // If a completion object is present and the input
    // widget is not read-only, show the Text Completion
    // menu item.
    if (d->completionView && !isReadOnly() && KAuthorized::authorize("lineedit_text_completion")) {
        QMenu *subMenu = popup->addMenu(KIcon("text-completion"), i18nc("@title:menu", "Text Completion"));
        connect(subMenu, SIGNAL(triggered (QAction*)),
                 this, SLOT(completionMenuActivated(QAction*)));

        popup->addSeparator();

        QActionGroup* ag = new QActionGroup(this);
        d->noCompletionAction = ag->addAction(i18nc("@item:inmenu Text Completion", "None"));
        d->shellCompletionAction = ag->addAction(i18nc("@item:inmenu Text Completion", "Manual"));
        d->autoCompletionAction = ag->addAction(i18nc("@item:inmenu Text Completion", "Automatic"));
        d->popupCompletionAction = ag->addAction(i18nc("@item:inmenu Text Completion", "Dropdown List"));
        d->shortAutoCompletionAction = ag->addAction(i18nc("@item:inmenu Text Completion", "Short Automatic"));
        d->popupAutoCompletionAction = ag->addAction(i18nc("@item:inmenu Text Completion", "Dropdown List && Automatic"));
        subMenu->addActions(ag->actions());

        //subMenu->setAccel(KStandardShortcut::completion(), ShellCompletion);

        d->shellCompletionAction->setCheckable(true);
        d->noCompletionAction->setCheckable(true);
        d->popupCompletionAction->setCheckable(true);
        d->autoCompletionAction->setCheckable(true);
        d->shortAutoCompletionAction->setCheckable(true);
        d->popupAutoCompletionAction->setCheckable(true);

        d->shellCompletionAction->setEnabled(!d->disableCompletionMap[ KGlobalSettings::CompletionShell ]);
        d->noCompletionAction->setEnabled(!d->disableCompletionMap[ KGlobalSettings::CompletionNone ]);
        d->popupCompletionAction->setEnabled(!d->disableCompletionMap[ KGlobalSettings::CompletionPopup ]);
        d->autoCompletionAction->setEnabled(!d->disableCompletionMap[ KGlobalSettings::CompletionAuto ]);
        d->shortAutoCompletionAction->setEnabled(!d->disableCompletionMap[ KGlobalSettings::CompletionMan ]);
        d->popupAutoCompletionAction->setEnabled(!d->disableCompletionMap[ KGlobalSettings::CompletionPopupAuto ]);

        const KGlobalSettings::Completion mode = completionMode();
        d->noCompletionAction->setChecked(mode == KGlobalSettings::CompletionNone);
        d->shellCompletionAction->setChecked(mode == KGlobalSettings::CompletionShell);
        d->popupCompletionAction->setChecked(mode == KGlobalSettings::CompletionPopup);
        d->autoCompletionAction->setChecked( mode == KGlobalSettings::CompletionAuto);
        d->shortAutoCompletionAction->setChecked(mode == KGlobalSettings::CompletionMan);
        d->popupAutoCompletionAction->setChecked(mode == KGlobalSettings::CompletionPopupAuto);

        const KGlobalSettings::Completion defaultMode = KGlobalSettings::completionMode();
        if (mode != defaultMode && !d->disableCompletionMap[ defaultMode ]) {
            subMenu->addSeparator();
            d->defaultAction = subMenu->addAction(i18nc("@item:inmenu Text Completion", "Default"));
        }
    }

    return popup;
}

void KLineEditView::contextMenuEvent(QContextMenuEvent *e)
{
    if (QLineEdit::contextMenuPolicy() != Qt::DefaultContextMenu) {
      return;
    }
    QMenu *popup = createStandardContextMenu();

    // ### do we really need this?  Yes, Please do not remove!  This
    // allows applications to extend the popup menu without having to
    // inherit from this class! (DA)
    emit aboutToShowContextMenu(popup);

    popup->exec(e->globalPos());
    delete popup;
}

void KLineEditView::completionMenuActivated(QAction  *act)
{
    KGlobalSettings::Completion oldMode = completionMode();

    if (act == d->noCompletionAction) {
        setCompletionMode(KGlobalSettings::CompletionNone);
    } else if (act ==  d->shellCompletionAction) {
        setCompletionMode(KGlobalSettings::CompletionShell);
    } else if (act == d->autoCompletionAction) {
        setCompletionMode(KGlobalSettings::CompletionAuto);
    } else if (act == d->popupCompletionAction) {
        setCompletionMode(KGlobalSettings::CompletionPopup);
    } else if (act == d->shortAutoCompletionAction) {
        setCompletionMode(KGlobalSettings::CompletionMan);
    } else if (act == d->popupAutoCompletionAction) {
        setCompletionMode(KGlobalSettings::CompletionPopupAuto);
    } else if (act == d->defaultAction) {
        setCompletionMode(KGlobalSettings::completionMode());
    } else {
        return;
    }

    if (oldMode != completionMode()) {
        if ((oldMode == KGlobalSettings::CompletionPopup ||
            oldMode == KGlobalSettings::CompletionPopupAuto) &&
            d->completionView && d->completionView->isVisible()) {
            d->completionView->hide();
        }
        emit completionModeChanged(completionMode());
    }
}

void KLineEditView::dropEvent(QDropEvent *e)
{
    if (d->handleURLDrops) {
        const KUrl::List urlList = KUrl::List::fromMimeData(e->mimeData());
        if (!urlList.isEmpty()) {
            // Let's replace the current text with the dropped URL(s), rather than appending.
            // Makes more sense in general (#188129), e.g. konq location bar and kurlrequester
            // can only hold one url anyway. OK this code supports multiple urls being dropped,
            // but that's not the common case [and it breaks if they contain spaces... this is why
            // kfiledialog uses double quotes around filenames in multiple-selection mode]...
            //
            // Anyway, if some apps prefer "append" then we should have a
            // setUrlDropsSupport({NoUrlDrops, SingleUrlDrops, MultipleUrlDrops})
            // where Single replaces and Multiple appends.
            QString dropText;
            //QString dropText = text();
            KUrl::List::ConstIterator it;
            for(it = urlList.begin() ; it != urlList.end() ; ++it) {
                if (!dropText.isEmpty()) {
                    dropText+=' ';
                }

                dropText += (*it).prettyUrl();
            }

            setText(dropText);
            setCursorPosition(dropText.length());

            e->accept();
            return;
        }
    }
    QLineEdit::dropEvent(e);
}

bool KLineEditView::event(QEvent* ev)
{
//     KCursor::autoHideEventFilter(this, ev);
    if (ev->type() == QEvent::ShortcutOverride) {
        QKeyEvent *e = static_cast<QKeyEvent *>(ev);
        if (d->overrideShortcut(e)) {
            ev->accept();
        }
    } else if (ev->type() == QEvent::KeyPress) {
        // Hmm -- all this could be done in keyPressEvent too...

        QKeyEvent *e = static_cast<QKeyEvent *>(ev);
        if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
            kDebug() << "enter";
            const bool trap = d->completionView && d->completionView->isVisible();

            const bool stopEvent = trap || (d->grabReturnKeyEvents &&
                                      (e->modifiers() == Qt::NoButton ||
                                       e->modifiers() == Qt::KeypadModifier));

            // Qt will emit returnPressed() itself if we return false
            if (stopEvent) {
                emit QLineEdit::returnPressed();
                e->accept();
            }

            emit returnPressed(displayText());

            if (trap) {
                d->completionView->hide();
                deselect();
                setCursorPosition(text().length());
            }

            // Eat the event if the user asked for it, or if a completionview was visible
            if (stopEvent) {
                return true;
            }
        }
    } else if (completionMode() != KGlobalSettings::CompletionNone &&
        ev->type() == QEvent::KeyRelease) {
        QKeyEvent *e = static_cast<QKeyEvent *>(ev);
        // handle rotation
        // Handles previous match
        if (e->key() == Qt::Key_Up) {
            kDebug() << "PrevCompletionMatch";
            if (emitSignals()) {
                emit textRotation(KCompletionBase::PrevCompletionMatch);
            }
            if (handleSignals()) {
                kDebug() << "rotateText(KCompletionBase::PrevCompletionMatch);";
                rotateText(KCompletionBase::PrevCompletionMatch);
            }
            return true;
        }

        // Handles next match
        if (e->key() == Qt::Key_Down) {
            kDebug() << "NextCompletionMatch";
            if (emitSignals()) {
                emit textRotation(KCompletionBase::NextCompletionMatch);
            }
            if (handleSignals()) {
                kDebug() << "rotateText(KCompletionBase::NextCompletionMatch);";
                rotateText(KCompletionBase::NextCompletionMatch);
            }
            return true;
        }
    } else if (ev->type() == QEvent::LayoutDirectionChange) {
        if (isRightToLeft()) {
            d->m_leftLayout->setDirection(QBoxLayout::RightToLeft);
            d->m_rightLayout->setDirection(QBoxLayout::RightToLeft);
        } else {
            d->m_leftLayout->setDirection(QBoxLayout::LeftToRight);
            d->m_rightLayout->setDirection(QBoxLayout::LeftToRight);
        }
    }
    return QLineEdit::event(ev);
}


void KLineEditView::setUrlDropsEnabled(bool enable)
{
    d->handleURLDrops=enable;
}

bool KLineEditView::urlDropsEnabled() const
{
    return d->handleURLDrops;
}

void KLineEditView::setTrapReturnKey(bool grab)
{
    d->grabReturnKeyEvents = grab;
}

bool KLineEditView::trapReturnKey() const
{
    return d->grabReturnKeyEvents;
}

void KLineEditView::setUrl(const KUrl& url)
{
    setText(url.prettyUrl());
}

void KLineEditView::slotActivated(const QModelIndex &index)
{
    userCancelled(index.data().toString());
}

void KLineEditView::userCancelled(const QString & cancelText)
{
    if (completionMode() != KGlobalSettings::CompletionPopupAuto) {
        // TODO: this sets modified==false. But maybe it was true before...
        setText(cancelText);
    } else if (hasSelectedText()) {
        if (d->userSelection) {
            deselect();
        } else {
            d->autoSuggest=false;
            const int start = selectionStart() ;
            const QString s=text().remove(selectionStart(), selectedText().length());
            setText(s);
            setCursorPosition(start);
            d->autoSuggest=true;
        }
    }
}

bool KLineEditView::Private::overrideShortcut(const QKeyEvent* e)
{
    KShortcut scKey;

    const int key = e->key() | e->modifiers();
    const KLineEditView::KeyBindingMap keys = q->getKeyBindings();

    if (keys[KLineEditView::TextCompletion].isEmpty()) {
        scKey = KStandardShortcut::shortcut(KStandardShortcut::TextCompletion);
    } else {
        scKey = keys[KLineEditView::TextCompletion];
    }

    if (scKey.contains(key)) {
        return true;
    }

    if (keys[KLineEditView::NextCompletionMatch].isEmpty()) {
        scKey = KStandardShortcut::shortcut(KStandardShortcut::NextCompletion);
    } else {
        scKey = keys[KLineEditView::NextCompletionMatch];
    }

    if (scKey.contains(key)) {
        return true;
    }

    if (keys[KLineEditView::PrevCompletionMatch].isEmpty()) {
        scKey = KStandardShortcut::shortcut(KStandardShortcut::PrevCompletion);
    } else {
        scKey = keys[KLineEditView::PrevCompletionMatch];
    }

    if (scKey.contains(key)) {
        return true;
    }

    // Override all the text manupilation accelerators...
    if (KStandardShortcut::copy().contains(key)) {
        return true;
    } else if (KStandardShortcut::paste().contains(key)) {
        return true;
    } else if (KStandardShortcut::cut().contains(key)) {
        return true;
    } else if (KStandardShortcut::undo().contains(key)) {
        return true;
    } else if (KStandardShortcut::redo().contains(key)) {
        return true;
    } else if (KStandardShortcut::deleteWordBack().contains(key)) {
        return true;
    } else if (KStandardShortcut::deleteWordForward().contains(key)) {
        return true;
    } else if (KStandardShortcut::forwardWord().contains(key)) {
        return true;
    } else if (KStandardShortcut::backwardWord().contains(key)) {
        return true;
    } else if (KStandardShortcut::beginningOfLine().contains(key)) {
        return true;
    } else if (KStandardShortcut::endOfLine().contains(key)) {
        return true;
    }
    // Shortcut overrides for shortcuts that QLineEdit handles
    // but doesn't dare force as "stronger than kaction shortcuts"...
    else if (e->matches(QKeySequence::SelectAll)) {
        return true;
    }
#ifdef Q_WS_X11
    else if (key == Qt::CTRL + Qt::Key_E || key == Qt::CTRL + Qt::Key_U) {
        return true;
    }
#endif

    if (completionView && completionView->model() && completionView->isVisible()) {
        const int key = e->key();
        const Qt::KeyboardModifiers modifiers = e->modifiers();
        if ((key == Qt::Key_Backtab || key == Qt::Key_Tab) &&
            (modifiers == Qt::NoModifier || (modifiers & Qt::ShiftModifier))) {
            return true;
        }
    }

    return false;
}

void KLineEditView::updateCompletedItems(bool autoSuggest)
{
    QString txt;
    if (d->completionView && d->completionView->isVisible()) {
        // The popup is visible already - do the matching on the initial string,
        // not on the currently selected one.
        txt = d->completionView->cancelledText();
    } else {
        txt = text();
    }

    if (d->completionView && d->completionView->model() && d->completionView->model()->hasChildren()) {
        if (d->completionView->isVisible()) {
            QModelIndex currentIndex = d->completionView->currentIndex();
            QModelIndex matchedIndex;

            bool wasSelected = false;
            QString currentSelection;

            d->completionView->sizeAndPosition();
            if (currentIndex.isValid()) {
                QItemSelectionModel* selModel = d->completionView->selectionModel();
                wasSelected = selModel->hasSelection() &&
                    selModel->selectedIndexes().first() == currentIndex;
                currentSelection = selModel->selectedIndexes().first().data().toString();
            }

            // If no item is selected, that means the listbox hasn't been manipulated by the user yet,
            // because it's not possible otherwise to have no selected item. In such case make
            // always the first item current and unselected, so that the current item doesn't jump.
            QModelIndex matchedItem;
            if (!wasSelected) {
                matchedItem = d->completionView->model()->index(0,0);
            }

            if (matchedItem.isValid()) {
                const bool blocked = d->completionView->blockSignals(true);
                d->completionView->selectionModel()->select(matchedIndex, QItemSelectionModel::SelectCurrent);
                d->completionView->blockSignals(blocked);
            }
        } else { // completion box not visible yet -> show it
            if (!txt.isEmpty()) {
                kDebug() << "setCancelledText" << txt;
                d->completionView->setCancelledText(txt);
            }
            d->completionView->popup();
        }

        if (d->autoSuggest && autoSuggest) {
            const QString tempString = d->completionView->model()->index(0,0).data().toString();
            const int index = tempString.indexOf(txt);
            const QString newText = tempString.mid(index);
            setUserSelection(false);
            setCompletedText(newText, true);
        }
    } else if (d->completionView && d->completionView->isVisible()) {
        d->completionView->hide();
    }
}

KCompletionView * KLineEditView::completionView(bool create)
{
    if (create && !d->completionView) {
        d->completionView = new KCompletionView(this);
        d->completionView->setObjectName("completion box");
        d->completionView->setFont(font());
        connect(d->completionView, SIGNAL(userCancelled(const QString&)),
                this, SLOT(userCancelled(const QString&)));
        connect(d->completionView, SIGNAL(activated(const QModelIndex &)),
            SLOT(slotActivated(const QModelIndex &)));
    }

    return d->completionView;
}

// QWidget::create() turns off mouse-Tracking which would break auto-hiding
void KLineEditView::create(WId id, bool initializeWindow, bool destroyOldWindow)
{
    QLineEdit::create(id, initializeWindow, destroyOldWindow);
    KCursor::setAutoHideCursor(this, true, true);
}

void KLineEditView::setUserSelection(bool userSelection)
{
    //if !d->userSelection && userSelection we are accepting a completion,
    //so trigger an update

    if (!d->userSelection && userSelection) {
        d->_k_updateUserText(text());
    }

    QPalette p = palette();

    if (userSelection) {
        p.setColor(QPalette::Highlight, d->previousHighlightColor);
        p.setColor(QPalette::HighlightedText, d->previousHighlightedTextColor);
    } else {
        QColor color=p.color(QPalette::Disabled, QPalette::Text);
        p.setColor(QPalette::HighlightedText, color);
        color=p.color(QPalette::Active, QPalette::Base);
        p.setColor(QPalette::Highlight, color);
    }

    d->userSelection=userSelection;
    setPalette(p);
}

void KLineEditView::slotRestoreSelectionColors()
{
    if (d->disableRestoreSelection) {
        return;
    }

    setUserSelection(true);
}

void KLineEditView::clear()
{
    setText(QString());
}

void KLineEditView::setTextWorkaround(const QString& text)
{
    if (!text.isEmpty()) {
        setText(text);
        end(false); // force cursor at end
    }
}

QString KLineEditView::originalText() const
{
    if (d->enableSqueezedText && isReadOnly()) {
        return d->squeezedText;
    }

    return text();
}

QString KLineEditView::userText() const
{
    return d->userText;
}

bool KLineEditView::autoSuggest() const
{
    return d->autoSuggest;
}

void KLineEditView::paintEvent(QPaintEvent *ev)
{
    if (echoMode() == Password && d->threeStars) {
        blockSignals(true);
        const QString oldText = text();
        const bool isModifiedState = isModified(); // save modified state because setText resets it
        setText(oldText + oldText + oldText);
        QLineEdit::paintEvent(ev);
        setText(oldText);
        setModified(isModifiedState);
        blockSignals(false);
    } else {
        QLineEdit::paintEvent(ev);
    }

//     kDebug() << d->enableClickMsg << d->drawClickMsg << !hasFocus() << text().isEmpty();
    if (d->enableClickMsg && d->drawClickMsg && !hasFocus() && text().isEmpty()) {
        QPainter p(this);
        QFont f = font();
        f.setItalic(d->italicizePlaceholder);
        p.setFont(f);

        QColor color(palette().color(foregroundRole()));
        color.setAlphaF(0.5);
        p.setPen(color);

        //FIXME: fugly alert!
        // qlineedit uses an internal qstyleoption set to figure this out
        // and then adds a hardcoded 2 pixel interior to that.
        // probably requires fixes to Qt itself to do this cleanly
        // see define horizontalMargin 2 in qlineedit.cpp
        QStyleOptionFrameV2 opt;
        initStyleOption(&opt);
        QRect textRect = style()->subElementRect(QStyle::SE_LineEditContents, &opt, this);
        int horizontalMargin = 2;
        textRect.adjust(horizontalMargin, 0, 0, 0);
        int left = textMargin(LeftSide);
        int right = textMargin(RightSide);
        textRect.adjust(left, 0, -right, 0);

        p.drawText(textRect, Qt::AlignLeft|Qt::AlignVCenter, d->clickMessage);
    }
}

void KLineEditView::focusInEvent(QFocusEvent *ev)
{
    kDebug();
    if (d->enableClickMsg && d->drawClickMsg) {
        d->drawClickMsg = false;
        update();
    }
    QLineEdit::focusInEvent(ev);
}

void KLineEditView::focusOutEvent(QFocusEvent *ev)
{
    if (d->enableClickMsg && text().isEmpty()) {
        d->drawClickMsg = true;
        update();
    }

    QLineEdit::focusOutEvent(ev);
}

void KLineEditView::setClickMessage(const QString &msg)
{
    d->enableClickMsg = !msg.isEmpty();
    d->clickMessage = msg;
    d->drawClickMsg = text().isEmpty();
    update();
}

void KLineEditView::setContextMenuEnabled(bool showMenu)
{
    QLineEdit::setContextMenuPolicy(showMenu ? Qt::DefaultContextMenu : Qt::NoContextMenu);
}

bool KLineEditView::isContextMenuEnabled() const
{
    return  (contextMenuPolicy() == Qt::DefaultContextMenu);
}

void KLineEditView::setPasswordMode(bool b)
{
    if (b) {
        KConfigGroup cg(KGlobal::config(), "Passwords");
        const QString val = cg.readEntry("EchoMode", "OneStar");
        if (val == "NoEcho") {
            setEchoMode(NoEcho);
        } else {
            d->threeStars = (val == "ThreeStars");
            setEchoMode(Password);
        }
    } else {
        setEchoMode(Normal);
    }
}

bool KLineEditView::passwordMode() const
{
    return echoMode() == NoEcho || echoMode() == Password;
}

void KLineEditView::doCompletion(const QString& txt)
{
    kDebug();
    if (emitSignals()) {
        emit completion(txt); // emit when requested...
    }
    d->completionRunning = true;
    if (handleSignals()) {
        makeCompletion(txt);  // handle when requested...
    }
    d->completionRunning = false;
}

#include "klineeditview.moc"
#include "klineeditview_p.moc"

