#include <kdialog.h>
#include <klocale.h>

/********************************************************************************
** Form generated from reading ui file 'bookmarksview_base.ui'
**
** Created: Mon May 4 12:25:31 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_BOOKMARKSVIEW_BASE_H
#define UI_BOOKMARKSVIEW_BASE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QListView>
#include <QtGui/QSplitter>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <akonadi/collectionview.h>
#include <akonadi/itemview.h>

QT_BEGIN_NAMESPACE

class Ui_bookmarksview_base
{
public:
    QGridLayout *gridLayout;
    QSplitter *splitter;
    Akonadi::CollectionView *collectionsView;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QLineEdit *bookmarksFilterBox;
    Akonadi::ItemView *bookmarksView;
    QListView *bookmarkPropertiesView;

    void setupUi(QWidget *bookmarksview_base)
    {
        if (bookmarksview_base->objectName().isEmpty())
            bookmarksview_base->setObjectName(QString::fromUtf8("bookmarksview_base"));
        bookmarksview_base->resize(674, 437);
        bookmarksview_base->setAutoFillBackground(true);
        gridLayout = new QGridLayout(bookmarksview_base);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        splitter = new QSplitter(bookmarksview_base);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(splitter->sizePolicy().hasHeightForWidth());
        splitter->setSizePolicy(sizePolicy);
        splitter->setOrientation(Qt::Horizontal);
        collectionsView = new Akonadi::CollectionView(splitter);
        collectionsView->setObjectName(QString::fromUtf8("collectionsView"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(collectionsView->sizePolicy().hasHeightForWidth());
        collectionsView->setSizePolicy(sizePolicy1);
        splitter->addWidget(collectionsView);
        layoutWidget = new QWidget(splitter);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        bookmarksFilterBox = new QLineEdit(layoutWidget);
        bookmarksFilterBox->setObjectName(QString::fromUtf8("bookmarksFilterBox"));

        verticalLayout->addWidget(bookmarksFilterBox);

        bookmarksView = new Akonadi::ItemView(layoutWidget);
        bookmarksView->setObjectName(QString::fromUtf8("bookmarksView"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(4);
        sizePolicy2.setHeightForWidth(bookmarksView->sizePolicy().hasHeightForWidth());
        bookmarksView->setSizePolicy(sizePolicy2);

        verticalLayout->addWidget(bookmarksView);

        bookmarkPropertiesView = new QListView(layoutWidget);
        bookmarkPropertiesView->setObjectName(QString::fromUtf8("bookmarkPropertiesView"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(1);
        sizePolicy3.setHeightForWidth(bookmarkPropertiesView->sizePolicy().hasHeightForWidth());
        bookmarkPropertiesView->setSizePolicy(sizePolicy3);

        verticalLayout->addWidget(bookmarkPropertiesView);

        splitter->addWidget(layoutWidget);

        gridLayout->addWidget(splitter, 0, 0, 1, 1);


        retranslateUi(bookmarksview_base);

        QMetaObject::connectSlotsByName(bookmarksview_base);
    } // setupUi

    void retranslateUi(QWidget *bookmarksview_base)
    {
        bookmarksview_base->setWindowTitle(tr2i18n("kapp4_base", 0));
        Q_UNUSED(bookmarksview_base);
    } // retranslateUi

};

namespace Ui {
    class bookmarksview_base: public Ui_bookmarksview_base {};
} // namespace Ui

QT_END_NAMESPACE

#endif // BOOKMARKSVIEW_BASE_H

