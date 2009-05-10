# search packages used by KDE
find_package (KDE4 REQUIRED)
include(KDE4Defaults)
include(NepomukMacros)
include(MacroLibrary)
include(MacroOptionalAddSubdirectory)
include(CheckIncludeFiles)
find_package (KdepimLibs REQUIRED)
find_package (Nepomuk REQUIRED)

if(WIN32)
    set(LIB_INSTALL_DIR ${LIB_INSTALL_DIR}
                        RUNTIME DESTINATION ${BIN_INSTALL_DIR}
                        LIBRARY DESTINATION ${LIB_INSTALL_DIR}
                        ARCHIVE DESTINATION ${LIB_INSTALL_DIR})
endif(WIN32)

include_directories(
    ${KDE4_INCLUDES}
    ${KDEPIMLIBS_INCLUDE_DIRS}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")


include_directories(
    ${KDE4_INCLUDES}
    ${SOPRANO_INCLUDE_DIRS}
    ${NEPOMUK_INCLUDE_DIRS}
    ${KDEPIMLIBS_INCLUDE_DIRS}
)

# If any of the .rdfs files change or if Nepomuk changes extensively,
# you may need to add/remove source files ${nie_SRCS} variable below.
#

set(ontologies_SRCS
  ${CMAKE_CURRENT_BINARY_DIR}/affiliation.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/application.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/archive.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/archiveitem.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/attachment.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/audio.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/audioimaccount.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/bookmark.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/bookmarkfolder.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/bbsnumber.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/carphonenumber.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/cellphonenumber.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/compressiontype.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/contact.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/contactgroup.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/contactlist.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/contactlistdataobject.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/contactmedium.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/cursor.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/datacontainer.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/dataobject.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/datasource.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/deletedresource.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/document.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/domesticdeliveryaddress.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/emailaddress.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/email.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/embeddedfiledataobject.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/executable.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/faxnumber.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/filedataobject.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/filehash.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/filesystem.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/filesystemimage.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/folder.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/font.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/gender.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/harddiskpartition.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/htmldocument.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/icon.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/imaccount.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/image.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/immessage.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/informationelement.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/internationaldeliveryaddress.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/isdnnumber.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/mailbox.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/mailboxdataobject.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/media.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/mediafilelistentry.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/medialist.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/mediastream.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/message.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/messageheader.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/messagingnumber.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/mindmap.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/modemnumber.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/operatingsystem.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/organizationcontact.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/pagernumber.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/paginatedtextdocument.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/parceldeliveryaddress.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/pcsnumber.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/personcontact.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/phonenumber.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/plaintextdocument.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/postaladdress.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/presentation.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/rasterimage.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/remotedataobject.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/remoteportaddress.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/role.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/software.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/softwareitem.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/softwareservice.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/sourcecode.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/spreadsheet.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/textdocument.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/trash.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/vectorimage.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/video.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/videoimaccount.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/videotelephonenumber.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/visual.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/voicephonenumber.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/website.cpp
)

# finds nepomuk-rcgen in BIN_INSTALL_DIR, then in the others
find_program(_rcgen nepomuk-rcgen PATHS ${BIN_INSTALL_DIR} NO_DEFAULT_PATH)
find_program(_rcgen nepomuk-rcgen)
if(NOT _rcgen)
  message(FATAL_ERROR "Failed to find the Nepomuk source generator. Did you install Soprano?" )
endif(NOT _rcgen)

add_custom_command(
  OUTPUT 
    ${CMAKE_CURRENT_BINARY_DIR}/out_headers
    ${CMAKE_CURRENT_BINARY_DIR}/out_sources
    ${CMAKE_CURRENT_BINARY_DIR}/out_includes
    ${ontologies_SRCS}
  COMMAND ${CMAKE_COMMAND}
  ARGS 
    -D_rcgen=${_rcgen}
    -Dontofile1=${CMAKE_CURRENT_SOURCE_DIR}/nie.rdfs
    -Dontofile2=${CMAKE_CURRENT_SOURCE_DIR}/nco.rdfs
    -Dontofile3=${CMAKE_CURRENT_SOURCE_DIR}/nmo.rdfs
    -Dontofile4=${CMAKE_CURRENT_SOURCE_DIR}/nfo.rdfs
    -Dtargetdir=${CMAKE_CURRENT_BINARY_DIR}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/rcgen.cmake
  MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/nie.rdfs
  DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/nie.rdfs
    ${CMAKE_CURRENT_SOURCE_DIR}/nco.rdfs
    ${CMAKE_CURRENT_SOURCE_DIR}/nmo.rdfs 
    ${CMAKE_CURRENT_SOURCE_DIR}/nfo.rdfs 
)

add_custom_target(
  ontologies-gen
  ALL
  DEPENDS
    ${CMAKE_CURRENT_BINARY_DIR}/out_headers
    ${CMAKE_CURRENT_BINARY_DIR}/out_sources
    ${CMAKE_CURRENT_BINARY_DIR}/out_includes
)

kde4_add_library(ontologies SHARED ${ontologies_SRCS})
add_dependencies(ontologies ontologies-gen)

target_link_libraries(ontologies ${QT_QTCORE_LIBRARY} ${NEPOMUK_LIBRARIES})
set_target_properties(ontologies PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})

install(TARGETS ontologies ${INSTALL_TARGETS_DEFAULT_ARGS})

set(ontologies_LIB_HDRS
  ${CMAKE_CURRENT_BINARY_DIR}/affiliation.h
  ${CMAKE_CURRENT_BINARY_DIR}/application.h
  ${CMAKE_CURRENT_BINARY_DIR}/archive.h
  ${CMAKE_CURRENT_BINARY_DIR}/archiveitem.h
  ${CMAKE_CURRENT_BINARY_DIR}/attachment.h
  ${CMAKE_CURRENT_BINARY_DIR}/audio.h
  ${CMAKE_CURRENT_BINARY_DIR}/audioimaccount.h
  ${CMAKE_CURRENT_BINARY_DIR}/bookmark.h
  ${CMAKE_CURRENT_BINARY_DIR}/bookmarkfolder.h
  ${CMAKE_CURRENT_BINARY_DIR}/bbsnumber.h
  ${CMAKE_CURRENT_BINARY_DIR}/carphonenumber.h
  ${CMAKE_CURRENT_BINARY_DIR}/cellphonenumber.h
  ${CMAKE_CURRENT_BINARY_DIR}/compressiontype.h
  ${CMAKE_CURRENT_BINARY_DIR}/contact.h
  ${CMAKE_CURRENT_BINARY_DIR}/contactgroup.h
  ${CMAKE_CURRENT_BINARY_DIR}/contactlist.h
  ${CMAKE_CURRENT_BINARY_DIR}/contactlistdataobject.h
  ${CMAKE_CURRENT_BINARY_DIR}/contactmedium.h
  ${CMAKE_CURRENT_BINARY_DIR}/cursor.h
  ${CMAKE_CURRENT_BINARY_DIR}/datacontainer.h
  ${CMAKE_CURRENT_BINARY_DIR}/dataobject.h
  ${CMAKE_CURRENT_BINARY_DIR}/datasource.h
  ${CMAKE_CURRENT_BINARY_DIR}/deletedresource.h
  ${CMAKE_CURRENT_BINARY_DIR}/document.h
  ${CMAKE_CURRENT_BINARY_DIR}/domesticdeliveryaddress.h
  ${CMAKE_CURRENT_BINARY_DIR}/emailaddress.h
  ${CMAKE_CURRENT_BINARY_DIR}/email.h
  ${CMAKE_CURRENT_BINARY_DIR}/embeddedfiledataobject.h
  ${CMAKE_CURRENT_BINARY_DIR}/executable.h
  ${CMAKE_CURRENT_BINARY_DIR}/faxnumber.h
  ${CMAKE_CURRENT_BINARY_DIR}/filedataobject.h
  ${CMAKE_CURRENT_BINARY_DIR}/filehash.h
  ${CMAKE_CURRENT_BINARY_DIR}/filesystem.h
  ${CMAKE_CURRENT_BINARY_DIR}/filesystemimage.h
  ${CMAKE_CURRENT_BINARY_DIR}/folder.h
  ${CMAKE_CURRENT_BINARY_DIR}/font.h
  ${CMAKE_CURRENT_BINARY_DIR}/gender.h
  ${CMAKE_CURRENT_BINARY_DIR}/harddiskpartition.h
  ${CMAKE_CURRENT_BINARY_DIR}/htmldocument.h
  ${CMAKE_CURRENT_BINARY_DIR}/icon.h
  ${CMAKE_CURRENT_BINARY_DIR}/imaccount.h
  ${CMAKE_CURRENT_BINARY_DIR}/image.h
  ${CMAKE_CURRENT_BINARY_DIR}/immessage.h
  ${CMAKE_CURRENT_BINARY_DIR}/informationelement.h
  ${CMAKE_CURRENT_BINARY_DIR}/internationaldeliveryaddress.h
  ${CMAKE_CURRENT_BINARY_DIR}/isdnnumber.h
  ${CMAKE_CURRENT_BINARY_DIR}/mailbox.h
  ${CMAKE_CURRENT_BINARY_DIR}/mailboxdataobject.h
  ${CMAKE_CURRENT_BINARY_DIR}/media.h
  ${CMAKE_CURRENT_BINARY_DIR}/mediafilelistentry.h
  ${CMAKE_CURRENT_BINARY_DIR}/medialist.h
  ${CMAKE_CURRENT_BINARY_DIR}/mediastream.h
  ${CMAKE_CURRENT_BINARY_DIR}/message.h
  ${CMAKE_CURRENT_BINARY_DIR}/messageheader.h
  ${CMAKE_CURRENT_BINARY_DIR}/messagingnumber.h
  ${CMAKE_CURRENT_BINARY_DIR}/mindmap.h
  ${CMAKE_CURRENT_BINARY_DIR}/modemnumber.h
  ${CMAKE_CURRENT_BINARY_DIR}/operatingsystem.h
  ${CMAKE_CURRENT_BINARY_DIR}/organizationcontact.h
  ${CMAKE_CURRENT_BINARY_DIR}/pagernumber.h
  ${CMAKE_CURRENT_BINARY_DIR}/paginatedtextdocument.h
  ${CMAKE_CURRENT_BINARY_DIR}/parceldeliveryaddress.h
  ${CMAKE_CURRENT_BINARY_DIR}/pcsnumber.h
  ${CMAKE_CURRENT_BINARY_DIR}/personcontact.h
  ${CMAKE_CURRENT_BINARY_DIR}/phonenumber.h
  ${CMAKE_CURRENT_BINARY_DIR}/plaintextdocument.h
  ${CMAKE_CURRENT_BINARY_DIR}/postaladdress.h
  ${CMAKE_CURRENT_BINARY_DIR}/presentation.h
  ${CMAKE_CURRENT_BINARY_DIR}/rasterimage.h
  ${CMAKE_CURRENT_BINARY_DIR}/remotedataobject.h
  ${CMAKE_CURRENT_BINARY_DIR}/remoteportaddress.h
  ${CMAKE_CURRENT_BINARY_DIR}/role.h
  ${CMAKE_CURRENT_BINARY_DIR}/software.h
  ${CMAKE_CURRENT_BINARY_DIR}/softwareitem.h
  ${CMAKE_CURRENT_BINARY_DIR}/softwareservice.h
  ${CMAKE_CURRENT_BINARY_DIR}/sourcecode.h
  ${CMAKE_CURRENT_BINARY_DIR}/spreadsheet.h
  ${CMAKE_CURRENT_BINARY_DIR}/textdocument.h
  ${CMAKE_CURRENT_BINARY_DIR}/trash.h
  ${CMAKE_CURRENT_BINARY_DIR}/vectorimage.h
  ${CMAKE_CURRENT_BINARY_DIR}/video.h
  ${CMAKE_CURRENT_BINARY_DIR}/videoimaccount.h
  ${CMAKE_CURRENT_BINARY_DIR}/videotelephonenumber.h
  ${CMAKE_CURRENT_BINARY_DIR}/visual.h
  ${CMAKE_CURRENT_BINARY_DIR}/voicephonenumber.h
  ${CMAKE_CURRENT_BINARY_DIR}/website.h
)

install( FILES ${ontologies_LIB_HDRS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/nepomuk/ontologies
  COMPONENT Devel
)
