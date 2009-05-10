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

#ifndef _SOFTWARE_H_
#define _SOFTWARE_H_

class QDateTime;
class QDate;
class QTime;

namespace Nepomuk {
        class Software;
}

#include "informationelement.h"
#include <nepomuk/nepomuk_export.h>

namespace Nepomuk {

/**
 * A piece of software. Examples may include applications and 
 * the operating system. This interpretation most commonly applies 
 * to SoftwareItems. 
 */
    class NEPOMUK_EXPORT Software : public InformationElement
    {
    public:
        /**
         * Create a new empty and invalid Software instance
         */
        Software();
        Software( ResourceManager* );
        /**
         * Default copy constructor
         */
        Software( const Software& );
        Software( const Resource& );
        /**
         * Create a new Software instance representing the resource
         * referenced by \a uriOrIdentifier.
         */
        Software( const QString& uriOrIdentifier );
        Software( const QString& uriOrIdentifier, ResourceManager* );
        /**
         * Create a new Software instance representing the resource
         * referenced by \a uri.
         */
        Software( const QUrl& uri );
        Software( const QUrl& uri, ResourceManager* );
        ~Software();

        Software& operator=( const Software& );

            /**
             * Get property 'conflicts'. States that a piece of software is 
             * in conflict with another piece of software. 
             */
            QList<Software> conflictses() const;

            /**
             * Set property 'conflicts'. States that a piece of software is 
             * in conflict with another piece of software. 
             */
            void setConflictses( const QList<Software>& value );

            /**
             * Add a value to property 'conflicts'. States that a piece of software 
             * is in conflict with another piece of software. 
             */
            void addConflicts( const Software& value );

            /**
             * \return The URI of the property 'conflicts'. 
             */
            static QUrl conflictsUri();

            /**
             * Get property 'supercedes'. States that a piece of software 
             * supercedes another piece of software. 
             */
            QList<Software> supercedeses() const;

            /**
             * Set property 'supercedes'. States that a piece of software 
             * supercedes another piece of software. 
             */
            void setSupercedeses( const QList<Software>& value );

            /**
             * Add a value to property 'supercedes'. States that a piece of 
             * software supercedes another piece of software. 
             */
            void addSupercedes( const Software& value );

            /**
             * \return The URI of the property 'supercedes'. 
             */
            static QUrl supercedesUri();

            /**
             * Get all resources that have this resource set as property 'conflicts'. 
             * States that a piece of software is in conflict with another piece 
             * of software. \sa ResourceManager::allResourcesWithProperty 
             */
            QList<Software> conflictsOf() const;

            /**
             * Get all resources that have this resource set as property 'supercedes'. 
             * States that a piece of software supercedes another piece of 
             * software. \sa ResourceManager::allResourcesWithProperty 
             */
            QList<Software> supercedesOf() const;

            /**
             * Retrieve a list of all available Software resources. This list 
             * consists of all resource of type Software that are stored in 
             * the local Nepomuk meta data storage and any changes made locally. 
             * Be aware that in some cases this list can get very big. Then it 
             * might be better to use libKNep directly. 
             */
            static QList<Software> allSoftwares();


        /**
         * \return The URI of the resource type that is used in Software instances.
         */
        static QString resourceTypeUri();

    protected:
       Software( const QString& uri, const QUrl& type );
       Software( const QUrl& uri, const QUrl& type );
       Software( const QString& uri, const QUrl& type, ResourceManager* );
       Software( const QUrl& uri, const QUrl& type, ResourceManager* );
   };
}

#endif
