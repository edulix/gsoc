/*
 * bookmarksview.h
 *
 * Copyright (C) 2007 Eduardo Robles Elvira <edulix@gmail.com>
 */
#ifndef BOOKMARKSVIEW_H
#define BOOKMARKSVIEW_H

#include <QtGui/QWidget>

#include "ui_bookmarksview_base.h"

class QPainter;
class KUrl;

/**
 * This is the main view class for bookmarks.  Most of the non-menu,
 * non-toolbar, and non-statusbar (e.g., non frame) GUI code should go
 * here.
 *
 * @short Main view
 * @author Eduardo Robles Elvira <edulix@gmail.com>
 * @version 0.1
 */

class BookmarksView : public QWidget, public Ui::bookmarksview_base
{
    Q_OBJECT
public:
    /**
     * Default constructor
     */
    BookmarksView(QWidget *parent);

    /**
     * Destructor
     */
    virtual ~BookmarksView();

     
    void createModels();
private:
    Ui::bookmarksview_base ui_bookmarksview_base;

signals:
    /**
     * Use this signal to change the content of the statusbar
     */
    void signalChangeStatusbar(const QString& text);

    /**
     * Use this signal to change the content of the caption
     */
    void signalChangeCaption(const QString& text);

private slots:
    void switchColors();
    void settingsChanged();
};

#endif // BOOKMARSVIEW_H
