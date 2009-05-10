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

#ifndef _GENDER_H_
#define _GENDER_H_

class QDateTime;
class QDate;
class QTime;

namespace Nepomuk {
        class PersonContact;
}

#include <nepomuk/resource.h>
#include <nepomuk/nepomuk_export.h>

namespace Nepomuk {

/**
 * Gender. Instances of this class may include male and female. 
 */
    class NEPOMUK_EXPORT Gender : public Resource
    {
    public:
        /**
         * Create a new empty and invalid Gender instance
         */
        Gender();
        Gender( ResourceManager* );
        /**
         * Default copy constructor
         */
        Gender( const Gender& );
        Gender( const Resource& );
        /**
         * Create a new Gender instance representing the resource
         * referenced by \a uriOrIdentifier.
         */
        Gender( const QString& uriOrIdentifier );
        Gender( const QString& uriOrIdentifier, ResourceManager* );
        /**
         * Create a new Gender instance representing the resource
         * referenced by \a uri.
         */
        Gender( const QUrl& uri );
        Gender( const QUrl& uri, ResourceManager* );
        ~Gender();

        Gender& operator=( const Gender& );

            /**
             * Get all resources that have this resource set as property 'gender'. 
             * Gender of the given contact. \sa ResourceManager::allResourcesWithProperty 
             */
            QList<PersonContact> genderOf() const;

            /**
             * Retrieve a list of all available Gender resources. This list 
             * consists of all resource of type Gender that are stored in the 
             * local Nepomuk meta data storage and any changes made locally. 
             * Be aware that in some cases this list can get very big. Then it 
             * might be better to use libKNep directly. 
             */
            static QList<Gender> allGenders();


        /**
         * \return The URI of the resource type that is used in Gender instances.
         */
        static QString resourceTypeUri();

    protected:
       Gender( const QString& uri, const QUrl& type );
       Gender( const QUrl& uri, const QUrl& type );
       Gender( const QString& uri, const QUrl& type, ResourceManager* );
       Gender( const QUrl& uri, const QUrl& type, ResourceManager* );
   };
}

#endif
