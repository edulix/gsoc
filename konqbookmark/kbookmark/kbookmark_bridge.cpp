/*
    This file is part of the KDE project
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

#include "kbookmark_bridge.h"
#include "konqbookmark.h"
#include "placesmanager.h"

#include <KFileDialog>
#include <KStringHandler>
#include <KLocale>
#include <KDebug>

#include <nepomuk/ontologies/bookmark.h>
#include <nepomuk/ontologies/bookmarkfolder.h>
#include <nepomuk/ontologies/dataobject.h>

#include <QtGui/QApplication>

/**
 * A class for importing bookmarks
 */
namespace Konqueror
{
class  KBookmarkImporter : public QObject
{
    Q_OBJECT
public:
    KBookmarkImporter() {}
    ~KBookmarkImporter() {}

    void parseBookmarks();
protected:
    void parseBookmarks(const Nepomuk::BookmarkFolder &parent);

Q_SIGNALS:
    void newBookmark( const QString & text, const QString & url, const QString & additionalInfo );
    void newFolder( const QString & text, bool open, const QString & additionalInfo );
    void newSeparator();
    void endFolder();
};
}

void Konqueror::KBookmarkImporter::parseBookmarks()
{
    Nepomuk::BookmarkFolder bookmarkFolder("konqbookmark:/");
    parseBookmarks(bookmarkFolder);
}

void Konqueror::KBookmarkImporter::parseBookmarks(const Nepomuk::BookmarkFolder &parent)
{
    if (!parent.hasProperty(Nepomuk::BookmarkFolder::containsBookmarkFolderUri())) {
        return;
    }

    QString title = parent.titles().empty() ? QString() : parent.titles().first();
    emit newFolder(title, false, "");

    QList<Nepomuk::BookmarkFolder> bookmarkFolders = parent.containsBookmarkFolders();

    foreach (const Nepomuk::BookmarkFolder& bookmarkFolder, bookmarkFolders) {
        parseBookmarks(bookmarkFolder);
    }

    QList<Nepomuk::Bookmark> bookmarks = parent.containsBookmarks();
    foreach (const Nepomuk::Bookmark& bookmark, bookmarks) {
        KonqBookmark konqBookmark(bookmark.resourceUri());
        emit newBookmark(konqBookmark.title(), konqBookmark.url().toString(), "");

    }
    emit endFolder();
}

void Konqueror::KBookmarkImporterImpl::parse()
{
   KBookmarkImporter importer;
   setupSignalForwards(&importer, this);
   importer.parseBookmarks();
}

QString Konqueror::KBookmarkImporterImpl::findDefaultLocation(bool) const
{
    return QString();
}

///////////////////////////////////////////////////////////////////////////////

namespace Konqueror
{

class KBookmarkExporter : private KBookmarkGroupTraverser
{
public:
    KBookmarkExporter();

    void write( const KBookmarkGroup &grp ) { traverse(grp); }

private:
    virtual void visit( const KBookmark & );

    virtual void visitEnter( const KBookmarkGroup & );

    virtual void visitLeave( const KBookmarkGroup & );

private:
    Nepomuk::BookmarkFolder m_currentBookmarkFolder;
};
}

Konqueror::KBookmarkExporter::KBookmarkExporter()
    : m_currentBookmarkFolder("konqbookmark:/")
{
}

void Konqueror::KBookmarkExporter::visit(const KBookmark &bk)
{
    KonqBookmark bookmark;
    bookmark.setTitle(bk.fullText());
    bookmark.setUrl(bk.url());
    m_currentBookmarkFolder.addContainsBookmark(bookmark.bookmark());
}

void Konqueror::KBookmarkExporter::visitEnter(const KBookmarkGroup &grp)
{
    Nepomuk::BookmarkFolder newFolder;
    newFolder.setTitles(QStringList()); // so that the resource unique uri is actually created
    m_currentBookmarkFolder.addContainsBookmarkFolder( newFolder );
    newFolder.setTitles(QStringList(grp.fullText()));
    m_currentBookmarkFolder = newFolder;
}

void Konqueror::KBookmarkExporter::visitLeave(const KBookmarkGroup &)
{
    QList<Nepomuk::BookmarkFolder> parents = m_currentBookmarkFolder.containsBookmarkFolderOf();
    if (!parents.empty()) {
        m_currentBookmarkFolder = parents.first();
    }
}

void Konqueror::KBookmarkExporterImpl::write(const KBookmarkGroup& parent)
{
    KBookmarkExporter exporter;
    exporter.write( parent );
}

#include "kbookmark_bridge.moc"
