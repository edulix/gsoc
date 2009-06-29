set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
project(NEPOTEST)

set(KDE_MIN_VERSION "4.2.68")  # for the < 4.2 macro
find_package(KDE4 4.2.68 REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Nepomuk REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)

include_directories(
    ${KDE4_INCLUDES}
    ${SOPRANO_INCLUDE_DIRS}
    ${NEPOMUK_INCLUDE_DIRS}
    ${KDEPIMLIBS_INCLUDE_DIRS}
)

set(nepotest_SRCS
    nepotest.cpp)

kde4_add_executable(nepotest ${nepotest_SRCS})

target_link_libraries(nepotest
    konqbookmark
    ontologies
    ${QT_QTCORE_LIBRARY}
    ${KDE4_KDECORE_LIBS}
    ${NEPOMUK_LIBRARIES}
    ${SOPRANO_LIBRARIES})

install(TARGETS nepotest ${INSTALL_TARGETS_DEFAULT_ARGS})
