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

#ifndef _SOFTWARESERVICE_H_
#define _SOFTWARESERVICE_H_

class QDateTime;
class QDate;
class QTime;

namespace Nepomuk {
}

#include "dataobject.h"
#include <nepomuk/nepomuk_export.h>

namespace Nepomuk {

/**
 * A service published by a piece of software, either by an operating 
 * system or an application. Examples of such services may include 
 * calendar, addresbook and mailbox managed by a PIM application. 
 * This category is introduced to distinguish between data available 
 * directly from the applications (Via some Interprocess Communication 
 * Mechanisms) and data available from files on a disk. In either 
 * case both DataObjects would receive a similar interpretation 
 * (e.g. a Mailbox) and wouldn't differ on the content level. 
 */
    class NEPOMUK_EXPORT SoftwareService : public DataObject
    {
    public:
        /**
         * Create a new empty and invalid SoftwareService instance
         */
        SoftwareService();
        SoftwareService( ResourceManager* );
        /**
         * Default copy constructor
         */
        SoftwareService( const SoftwareService& );
        SoftwareService( const Resource& );
        /**
         * Create a new SoftwareService instance representing the resource
         * referenced by \a uriOrIdentifier.
         */
        SoftwareService( const QString& uriOrIdentifier );
        SoftwareService( const QString& uriOrIdentifier, ResourceManager* );
        /**
         * Create a new SoftwareService instance representing the resource
         * referenced by \a uri.
         */
        SoftwareService( const QUrl& uri );
        SoftwareService( const QUrl& uri, ResourceManager* );
        ~SoftwareService();

        SoftwareService& operator=( const SoftwareService& );

            /**
             * Retrieve a list of all available SoftwareService resources. 
             * This list consists of all resource of type SoftwareService 
             * that are stored in the local Nepomuk meta data storage and any 
             * changes made locally. Be aware that in some cases this list can 
             * get very big. Then it might be better to use libKNep directly. 
             */
            static QList<SoftwareService> allSoftwareServices();


        /**
         * \return The URI of the resource type that is used in SoftwareService instances.
         */
        static QString resourceTypeUri();

    protected:
       SoftwareService( const QString& uri, const QUrl& type );
       SoftwareService( const QUrl& uri, const QUrl& type );
       SoftwareService( const QString& uri, const QUrl& type, ResourceManager* );
       SoftwareService( const QUrl& uri, const QUrl& type, ResourceManager* );
   };
}

#endif
