set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
project(konqbookmark)

# search packages used by KDE
set(KDE_MIN_VERSION "4.2.68")  # for the < 4.2 macro
find_package (KDE4 REQUIRED)
find_package(ZLIB REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)
include(MacroOptionalAddSubdirectory)
include(CheckIncludeFiles)
find_package (KdepimLibs REQUIRED)
find_package(Nepomuk REQUIRED)

add_subdirectory(icons)

if(WIN32)
    set(LIB_INSTALL_DIR ${LIB_INSTALL_DIR}
        RUNTIME DESTINATION ${BIN_INSTALL_DIR}
        LIBRARY DESTINATION ${LIB_INSTALL_DIR}
        ARCHIVE DESTINATION ${LIB_INSTALL_DIR})
endif(WIN32)

include_directories(
    ${KDE4_INCLUDES}
    ${QT_INCLUDES}
    ${KDEPIMLIBS_INCLUDE_DIRS}
    ${SOPRANO_INCLUDE_DIRS}
    ${NEPOMUK_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")

############# library ################

set(konqbookmark_LIB_SRCS
    konqbookmark.cpp
    itemmodels/konqbookmarkproxymodel.cpp
    itemmodels/collectionsproxymodel.cpp
    itemmodels/konqbookmarkmodel.cpp
    itemmodels/kcompletionmodel.cpp
    itemmodels/kaggregatedmodel.cpp
    itemmodels/placesproxymodel.cpp
    place.cpp
    placesmanager.cpp
    watchitemcreatejob.cpp
    watchcollectioncreatejob.cpp
    kdatawidgetselectionmapper.cpp
    modelwatcher.cpp
    itemviews/konqbookmarkview.cpp
    itemviews/konqbookmarkdelegate.cpp
    itemviews/kbreadcrumbbutton.cpp
    itemviews/kbreadcrumbnavigator.cpp
    itemviews/kbreadcrumbnavigatorbutton.cpp
    itemviews/kbreadcrumbnavigatormenu.cpp
    itemviews/konqbookmarkmodelmenu.cpp
    itemviews/modelmenu.cpp
    itemviews/ktreeviewsearchline.cpp
    itemviews/modelmenusearchline.cpp
    itemviews/locationbar.cpp
    itemviews/locationbardelegate.cpp
    itemviews/kcompletionview.cpp
    itemviews/klineeditview.cpp
    kbookmark/kbookmark_bridge.cpp
)

kde4_add_kcfg_files(konqbookmark_LIB_SRCS konqbookmarksettings.kcfgc )
kde4_add_library(konqbookmark SHARED ${konqbookmark_LIB_SRCS})

target_link_libraries(konqbookmark
    akonadi_next
    ontologies
    kfile
    konq # for konq_historyentry. Anyway konqbookmark will have to move to libkonq in the future
    ${KDE4_AKONADI_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KIO_LIBS} # for KUrlCompletion
    ${NEPOMUK_LIBRARIES}
    ${SOPRANO_LIBRARIES}
)

set_target_properties(konqbookmark PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})

install(TARGETS konqbookmark ${INSTALL_TARGETS_DEFAULT_ARGS})


set(konqbookmark_LIB_HDRS
    konqbookmark.h
    itemmodels/collectionsproxymodel.h
    itemmodels/konqbookmarkproxymodel.h
    itemmodels/konqbookmarkmodel.h
    itemmodels/kcompletionmodel.h
    itemmodels/placesproxymodel.h
    itemmodels/kaggregatedmodel.h
    place.h
    placesmanager.h
    watchitemcreatejob.h
    watchcollectioncreatejob.h
    kdatawidgetselectionmapper.h
    modelwatcher.h
    konqbookmark_export.h
    itemviews/konqbookmarkview.h
    itemviews/konqbookmarkdelegate.h
    itemviews/kbreadcrumbnavigator.h
    itemviews/ktreeviewsearchline.h
    itemviews/modelmenu.h
    itemviews/konqbookmarkmodelmenu.h
    itemviews/modelmenusearchline.h
    itemviews/locationbar.h
    itemviews/locationbardelegate.h
    itemviews/kcompletionview.h
    itemviews/klineeditview.h
    kbookmark/kbookmark_bridge.h
)

install( FILES ${konqbookmark_LIB_HDRS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/konqbookmark
  COMPONENT Devel
)
install( FILES konqbookmarksettings.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )

########### serializer ###############

set(akonadi_serializer_konqbookmark_SRCS
  akonadi_serializer_konqbookmark.cpp
  konqbookmarkxmlio.cpp
)

kde4_add_plugin(akonadi_serializer_konqbookmark ${akonadi_serializer_konqbookmark_SRCS})

target_link_libraries(akonadi_serializer_konqbookmark
    konqbookmark
    akonadi_next
    ${KDE4_AKONADI_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${KDE4_KDECORE_LIBS}
)

install(TARGETS akonadi_serializer_konqbookmark DESTINATION ${PLUGIN_INSTALL_DIR})

############### Macros ###############

macro(MAKE_INSTALL_PATH_ABSOLUTE out in)
   if (IS_ABSOLUTE "${in}")
      set(${out} "${in}")
   else (IS_ABSOLUTE "${in}")
      set(${out} "\${KONQBOOKMARK_INSTALL_DIR}/${in}")
   endif (IS_ABSOLUTE "${in}")
endmacro(MAKE_INSTALL_PATH_ABSOLUTE out in)

############### Prepare KonqBookmarkConfig.cmake ###############

# all the following variables are put into KonqBookmarkConfig.cmake, so
# they are usable by projects using Akonadi.
make_install_path_absolute(KONQBOOMARK_BIN_DIR             ${BIN_INSTALL_DIR})
make_install_path_absolute(KONQBOOMARK_INCLUDE_DIR         ${INCLUDE_INSTALL_DIR})
make_install_path_absolute(KONQBOOMARK_LIB_DIR             ${LIB_INSTALL_DIR})
make_install_path_absolute(KONQBOOMARK_CONFIG_DIR          ${CONFIG_INSTALL_DIR})
make_install_path_absolute(KONQBOOMARK_DBUS_INTERFACES_DIR ${DBUS_INTERFACES_INSTALL_DIR})
make_install_path_absolute(KONQBOOMARK_DBUS_SERVICES_DIR   ${DBUS_SERVICES_INSTALL_DIR})
make_install_path_absolute(KONQBOOMARK_XDG_MIME_DIR        ${XDG_MIME_INSTALL_DIR})

############### Generate files ###############

configure_file(KonqBookmarkConfig.cmake.in "${KONQBOOMARK_BIN_DIR}/KonqBookmarkConfig.cmake" @ONLY)

########### install files ###############

install( FILES
  akonadi_serializer_konqbookmark.desktop
  DESTINATION ${DATA_INSTALL_DIR}/akonadi/plugins/serializer)

set(_KONQBOOKMARKCONFIG_INSTALL_DIR ${LIB_INSTALL_DIR}/KonqBookmark/cmake/)
   
install(FILES ${KONQBOOMARK_BIN_DIR}/KonqBookmarkConfig.cmake
        DESTINATION ${_KONQBOOKMARKCONFIG_INSTALL_DIR} )

macro_display_feature_log()
