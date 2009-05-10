# Install script for directory: /home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/home/edulix/proyectos/kde4")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "debugfull")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg][Ff][Uu][Ll][Ll])$")
    IF(EXISTS "$ENV{DESTDIR}/home/edulix/proyectos/kde4/lib/libontologies.so.4.3.0")
      FILE(RPATH_CHECK
           FILE "$ENV{DESTDIR}/home/edulix/proyectos/kde4/lib/libontologies.so.4.3.0"
           RPATH "/home/edulix/proyectos/kde4/lib:/home/edulix/proyectos/kde4/qt4/lib:/home/edulix/proyectos/kde4/lib")
    ENDIF(EXISTS "$ENV{DESTDIR}/home/edulix/proyectos/kde4/lib/libontologies.so.4.3.0")
    FILE(INSTALL DESTINATION "/home/edulix/proyectos/kde4/lib" TYPE SHARED_LIBRARY FILES
      "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/lib/libontologies.so.4.3.0"
      "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/lib/libontologies.so.4"
      "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/lib/libontologies.so"
      )
    IF(EXISTS "$ENV{DESTDIR}/home/edulix/proyectos/kde4/lib/libontologies.so.4.3.0")
      FILE(RPATH_CHANGE
           FILE "$ENV{DESTDIR}/home/edulix/proyectos/kde4/lib/libontologies.so.4.3.0"
           OLD_RPATH "/home/edulix/proyectos/kde4/qt4/lib:/home/edulix/proyectos/kde4/lib::::::::::::::::::::::::::::::::"
           NEW_RPATH "/home/edulix/proyectos/kde4/lib:/home/edulix/proyectos/kde4/qt4/lib:/home/edulix/proyectos/kde4/lib")
      IF(CMAKE_INSTALL_DO_STRIP)
        EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/edulix/proyectos/kde4/lib/libontologies.so.4.3.0")
      ENDIF(CMAKE_INSTALL_DO_STRIP)
    ENDIF(EXISTS "$ENV{DESTDIR}/home/edulix/proyectos/kde4/lib/libontologies.so.4.3.0")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg][Ff][Uu][Ll][Ll])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Devel)$")
  FILE(INSTALL DESTINATION "/home/edulix/proyectos/kde4/include/nepomuk/ontologies" TYPE FILE FILES
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/affiliation.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/application.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/archive.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/archiveitem.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/attachment.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/audio.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/audioimaccount.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/bookmark.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/bookmarkfolder.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/bbsnumber.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/carphonenumber.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/cellphonenumber.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/compressiontype.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/contact.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/contactgroup.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/contactlist.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/contactlistdataobject.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/contactmedium.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/cursor.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/datacontainer.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/dataobject.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/datasource.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/deletedresource.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/document.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/domesticdeliveryaddress.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/emailaddress.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/email.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/embeddedfiledataobject.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/executable.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/faxnumber.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/filedataobject.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/filehash.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/filesystem.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/filesystemimage.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/folder.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/font.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/gender.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/harddiskpartition.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/htmldocument.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/icon.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/imaccount.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/image.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/immessage.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/informationelement.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/internationaldeliveryaddress.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/isdnnumber.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/mailbox.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/mailboxdataobject.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/media.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/mediafilelistentry.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/medialist.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/mediastream.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/message.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/messageheader.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/messagingnumber.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/mindmap.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/modemnumber.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/operatingsystem.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/organizationcontact.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/pagernumber.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/paginatedtextdocument.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/parceldeliveryaddress.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/pcsnumber.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/personcontact.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/phonenumber.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/plaintextdocument.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/postaladdress.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/presentation.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/rasterimage.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/remotedataobject.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/remoteportaddress.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/role.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/software.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/softwareitem.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/softwareservice.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/sourcecode.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/spreadsheet.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/textdocument.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/trash.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/vectorimage.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/video.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/videoimaccount.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/videotelephonenumber.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/visual.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/voicephonenumber.h"
    "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/website.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Devel)$")

IF(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
ELSE(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
ENDIF(CMAKE_INSTALL_COMPONENT)

FILE(WRITE "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/${CMAKE_INSTALL_MANIFEST}" "")
FOREACH(file ${CMAKE_INSTALL_MANIFEST_FILES})
  FILE(APPEND "/home/edulix/proyectos/kde4/kdesvn/gsoc/ontologies/build/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
ENDFOREACH(file)
