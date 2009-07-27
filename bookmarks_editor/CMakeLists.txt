project(bookmarks)

find_package(KDE4 REQUIRED)
find_package(KdepimLibs REQUIRED)
find_package(KonqBookmark REQUIRED)

include (KDE4Defaults)

include_directories( ${KDE4_INCLUDES} ${KDEPIMLIBS_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory( doc )
add_subdirectory( src )
add_subdirectory( icons )
