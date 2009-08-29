# search packages used by KDE
find_package(KDE4 REQUIRED)
find_package(Nepomuk REQUIRED)
include(NepomukAddOntologyClasses)
include(KDE4Defaults)

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

NEPOMUK_ADD_ONTOLOGY_CLASSES(
    ontologies_SRCS
    ONTOLOGIES
    ${CMAKE_CURRENT_SOURCE_DIR}/nie.rdfs
    ${CMAKE_CURRENT_SOURCE_DIR}/nco.rdfs
    ${CMAKE_CURRENT_SOURCE_DIR}/nmo.rdfs
    ${CMAKE_CURRENT_SOURCE_DIR}/nfo.rdfs
    VISIBILITY nepomuk)

set(ontologies_LIB_HDRS ${_out_headers} "nepomuk_export.h")
kde4_add_library(ontologies SHARED ${ontologies_SRCS})

target_link_libraries(ontologies ${QT_QTCORE_LIBRARY} ${NEPOMUK_LIBRARIES})
set_target_properties(ontologies PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})

install(TARGETS ontologies ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES ${ontologies_LIB_HDRS}
    DESTINATION ${INCLUDE_INSTALL_DIR}/nepomuk/ontologies
    COMPONENT Devel
)
