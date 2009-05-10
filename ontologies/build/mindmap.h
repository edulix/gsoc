/*
 * This file is part of the Nepomuk KDE project.
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

/*
 * This file has been generated by the Nepomuk Resource class generator.
 * DO NOT EDIT THIS FILE.
 * ANY CHANGES WILL BE LOST.
 */

#ifndef _MINDMAP_H_
#define _MINDMAP_H_

class QDateTime;
class QDate;
class QTime;

namespace Nepomuk {
}

#include "document.h"
#include <nepomuk/nepomuk_export.h>

namespace Nepomuk {

/**
 * A MindMap, created by a mind-mapping utility. Examples might 
 * include FreeMind or mind mapper. 
 */
    class NEPOMUK_EXPORT MindMap : public Document
    {
    public:
        /**
         * Create a new empty and invalid MindMap instance
         */
        MindMap();
        MindMap( ResourceManager* );
        /**
         * Default copy constructor
         */
        MindMap( const MindMap& );
        MindMap( const Resource& );
        /**
         * Create a new MindMap instance representing the resource
         * referenced by \a uriOrIdentifier.
         */
        MindMap( const QString& uriOrIdentifier );
        MindMap( const QString& uriOrIdentifier, ResourceManager* );
        /**
         * Create a new MindMap instance representing the resource
         * referenced by \a uri.
         */
        MindMap( const QUrl& uri );
        MindMap( const QUrl& uri, ResourceManager* );
        ~MindMap();

        MindMap& operator=( const MindMap& );

            /**
             * Retrieve a list of all available MindMap resources. This list 
             * consists of all resource of type MindMap that are stored in the 
             * local Nepomuk meta data storage and any changes made locally. 
             * Be aware that in some cases this list can get very big. Then it 
             * might be better to use libKNep directly. 
             */
            static QList<MindMap> allMindMaps();


        /**
         * \return The URI of the resource type that is used in MindMap instances.
         */
        static QString resourceTypeUri();

    protected:
       MindMap( const QString& uri, const QUrl& type );
       MindMap( const QUrl& uri, const QUrl& type );
       MindMap( const QString& uri, const QUrl& type, ResourceManager* );
       MindMap( const QUrl& uri, const QUrl& type, ResourceManager* );
   };
}

#endif
