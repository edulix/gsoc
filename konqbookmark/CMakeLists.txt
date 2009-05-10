project(konqbookmark)

# search packages used by KDE
find_package (KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)
include(MacroOptionalAddSubdirectory)
include(CheckIncludeFiles)
find_package (KdepimLibs REQUIRED)

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

############# library ################

set(konqbookmark_LIB_SRCS
  konqbookmark.cpp
  konqbookmarkmodel.cpp
)

kde4_add_library(konqbookmark SHARED ${konqbookmark_LIB_SRCS})

target_link_libraries(konqbookmark ${KDE4_AKONADI_LIBS} ${QT_QTCORE_LIBRARY} ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS})
set_target_properties(konqbookmark PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})

install(TARGETS konqbookmark ${INSTALL_TARGETS_DEFAULT_ARGS})


set(konqbookmark_LIB_HDRS
  konqbookmark.h
  konqbookmarkmodel.h
  konqbookmark_export.h
)

install( FILES ${konqbookmark_LIB_HDRS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/konqbookmark
  COMPONENT Devel
)

########### serializer ###############

set(akonadi_serializer_konqbookmark_SRCS
  akonadi_serializer_konqbookmark.cpp
  konqbookmarkxmlio.cpp
)

kde4_add_plugin(akonadi_serializer_konqbookmark ${akonadi_serializer_konqbookmark_SRCS})

target_link_libraries(akonadi_serializer_konqbookmark konqbookmark ${KDE4_AKONADI_LIBS} ${QT_QTCORE_LIBRARY} ${KDE4_KDECORE_LIBS})

install(TARGETS akonadi_serializer_konqbookmark DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES
  akonadi_serializer_konqbookmark.desktop
  DESTINATION ${DATA_INSTALL_DIR}/akonadi/plugins/serializer)
