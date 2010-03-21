/***************************************************************************
 *   Copyright (C) 2009 by Eduardo Robles Elvira <edulix@gmail.com>        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA            *
 ***************************************************************************/
#include <QApplication>
#include <QString>
#include <QHash>
#include <QList>
#include <QUrl>

#include <kdebug.h>

#include <nepomuk/tag.h>
#include <nepomuk/resource.h>
#include <nepomuk/resourcemanager.h>
#include <nepomuk/variant.h>
#include <nepomuk/tools.h>
#include <nepomuk/class.h>
#include <nepomuk/ontologies/bookmark.h>
#include <nepomuk/ontologies/bookmarkfolder.h>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>
#include <Soprano/Vocabulary/NAO>
#include <Soprano/Global>
#include <Soprano/LiteralValue>

#include <iostream>
#include <cstring>
#include <cstdio>

using namespace std;

int processArgs(int argc, char **argv);
int addResource(const QString& uri, const QString& url, const QString& tagName);
int showResource(const QString& resUri);
int clearResources();
int showResources();
void processCommand(QString command, QList< QString > args);

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    Nepomuk::ResourceManager::instance()->init();
    string command;

    while(1)
    {
        cout << "terminal > ";
        cin >> command;
        QString qcommand(command.c_str());
        QStringList args = qcommand.split(",", QString::SkipEmptyParts);
        kDebug() << args;

        if (!args.isEmpty()) {
            processCommand(args.first(), args);
        }
    }
}

void processCommand(QString command, QList<QString> args)
{
    int numArgs = args.length();
    if (command == "query") {
        QString query = QString(
            "PREFIX nao: <%1> "
            "PREFIX nie: <http://www.semanticdesktop.org/ontologies/2007/01/19/nie> "
            "prefix rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> "
            "prefix nfo: <http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#> "
            "select distinct ?r where { "
            "?r rdf:type nfo:Bookmark . "
            "?r nao:created ?time . } "
            "ORDER BY DESC(?time) "
            "LIMIT 20")
            .arg( Soprano::Vocabulary::NAO::naoNamespace().toString() );

        Soprano::QueryResultIterator it = Nepomuk::ResourceManager::instance()->mainModel()->executeQuery( query,
            Soprano::Query::QueryLanguageSparql );
        kDebug() << "query done:";
        while( it.next() ) {
            QUrl resource = it.binding( "r" ).uri();
            Nepomuk::Bookmark bookmark(resource);
            kDebug() << "resource: " << resource.toString();
            kDebug() << "resource.titles: " << bookmark.titles();
            kDebug() << "resource.created: " << bookmark.property( Soprano::Vocabulary::NAO::naoNamespace().toString() + "created" ).toDateTime();
        }
    } else if (command == "clear") {
        clearResources();
    } else if (command == "list") {
        showResources();
    } else if (command == "add" && numArgs >= 4) {
        addResource(args[1], args[2], args[3]);
    } else {
        cout << "command not found: " << command.toStdString() << endl;
    }
}


int addResource(const QString& uri, const QString& url, const QString& tagName)
{
    kDebug();
    Nepomuk::Resource res(uri, url);
    Nepomuk::Tag newTag(tagName);
    newTag.setLabel(tagName);
    newTag.addIdentifier(tagName);

    if(!res.tags().contains(newTag)) {
        res.addTag(newTag);
        kDebug() << res.resourceUri().toString() << ": added tag " << newTag.label();
    } else {
        kDebug() << res.resourceUri().toString() << ": resource has already tag " << newTag.label();
    }

    return 0;
}

int showResource(const QString& resUri)
{
    kDebug();
    Nepomuk::Resource res(resUri);
    kDebug() << "resource " << res.resourceUri() << ":";

    QHash<QUrl, Nepomuk::Variant> properties = res.properties();
    QHash<QUrl, Nepomuk::Variant>::const_iterator it = properties.constBegin();

    for (; it != properties.constEnd(); ++it) {
        QUrl propertyUri = it.key();
        Nepomuk::Variant value = it.value();
        Nepomuk::Types::Class propertyType( propertyUri );

        kDebug() << "label: " << propertyType.name() << ": " << value.toString();
    }

    return 0;
}

int showResources()
{
    kDebug();
    QList<Nepomuk::Bookmark> bookmarks = Nepomuk::Bookmark::allBookmarks();
    kDebug()  << "There are" << bookmarks.count() << " bookmarks";

    foreach( const Nepomuk::Bookmark& bookmark, bookmarks )
    {
        kDebug() << "\nresource " << bookmark.resourceUri() << ":";
        QHash<QUrl, Nepomuk::Variant> properties = bookmark.properties();
        QHash<QUrl, Nepomuk::Variant>::const_iterator it = properties.constBegin();

        for(; it != properties.constEnd(); ++it)
        {
            QUrl propertyUri = it.key();
            Nepomuk::Variant value = it.value();
            Nepomuk::Types::Class propertyType( propertyUri );

            kDebug() << propertyType.name() << ": " << value.toString();
        }
    }
    return 0;
}

int clearResources()
{
    kDebug();
    kDebug() << "Removing all existing resources";
    QList<Nepomuk::Bookmark> bookmarks = Nepomuk::Bookmark::allBookmarks();
    kDebug()  << "There are " << bookmarks.count() << " bookmarks";

    foreach( const Nepomuk::Bookmark& bookmark, bookmarks )
    {
        kDebug() << "Removing: " << bookmark.resourceUri();
        Nepomuk::Bookmark copy(bookmark);
        copy.remove();
    }
    return 0;
}
