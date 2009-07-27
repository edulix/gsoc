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
#include <konqbookmark/konqbookmark.h>

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
int addResource(int argc, char **argv);
int showResource(int argc, char **argv);
int clearResources(int argc, char **argv);
int showResources();
void processCommand(QString command);

int main(int argc, char **argv)
{    
    QApplication app(argc, argv);
    
    Nepomuk::ResourceManager::instance()->init();
    
    processArgs(argc, argv);
    string command;
    
    while(1)
    {
        cout << "terminal > ";
        cin >> command;
        processCommand(QString(command.c_str()));
    }
}

void processCommand(QString command)
{
    if(command == "set")
    {
        string desc;
        cin >> desc;
        Nepomuk::Bookmark bookmark("example:/");
        bookmark.setDescription(QString(desc.c_str()));
        kDebug() << "set description: " << bookmark.description();
    } else if(command == "get")
    {
    
        Nepomuk::Bookmark bookmark("example:/");
        kDebug() << "description is: " << bookmark.description();
    } else if(command == "query")
    {
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
    }
}

int processArgs(int argc, char **argv)
{
    if(argc < 2)
    {
        printf("Usage: %s <command>. Use command 'list' to show available commands\n", argv[0]);
        return 1;
    }
    
    if(strcmp(argv[1], "list") == 0)
    {
        printf("Available commands: add, show, clear\n");
        return 0;
    } else if(strcmp(argv[1], "add") == 0)
    {
        return addResource(argc, argv);
    } else if(strcmp(argv[1], "show") == 0)
    {
        return showResource(argc, argv);
    } else if(strcmp(argv[1], "clear") == 0)
    {
        return clearResources(argc, argv);
    }
    
    return 1;
}

int addResource(int argc, char **argv)
{
    if(argc < 4)
    {
        printf("Usage: %s add <uri> <tagName>\n", argv[0]);
        return 1;
    }
    Nepomuk::Resource res(argv[2], QUrl("http://www.konqueror.com/#Bookmark"));
    Nepomuk::Tag newTag( argv[3] );
    newTag.setLabel( argv[3] );
    newTag.addIdentifier( argv[3] );
    
    if(!res.tags().contains(newTag))
    {
        res.addTag(newTag);
        kDebug() << res.resourceUri().toString() << ": added tag " << newTag.label();
    } else
        kDebug() << res.resourceUri().toString() << ": resource has already tag " << newTag.label();
    
    return 0;
}

int showResource(int argc, char **argv)
{
    if(argc < 3)
    {
        showResources();
        return 1;
    }
    
    Nepomuk::Resource res(argv[2]/*, QUrl("http://www.konqueror.com/#Bookmark")*/);
    kDebug() << "resource " << res.resourceUri() << ":";
    
    QHash<QUrl, Nepomuk::Variant> properties = res.properties();
    QHash<QUrl, Nepomuk::Variant>::const_iterator it = properties.constBegin();
    
    for(; it != properties.constEnd(); ++it)
    {
        QUrl propertyUri = it.key();
        Nepomuk::Variant value = it.value();
        Nepomuk::Types::Class propertyType( propertyUri );
        
        kDebug() << "label: " << propertyType.name() << ": " << value.toString();
    }
        
    return 0;
}

int showResources()
{
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

int clearResources(int argc, char **argv)
{
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
