set(bookmarks_SRCS
    bookmarks.cpp
    main.cpp
    bookmarksview.cpp
    actionsimpl.cpp
    modelwatcher.cpp
    watchitemcreatejob.cpp
)
# Needed to be able to use item.setPayload()
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}" ) 
include_directories(
    ${KDE4_INCLUDES}
    ${KDEPIMLIBS_INCLUDE_DIRS}
)

kde4_add_ui_files(bookmarks_SRCS bookmarksview_base.ui)

kde4_add_kcfg_files(bookmarks_SRCS settings.kcfgc )

kde4_add_executable(bookmarks ${bookmarks_SRCS})

target_link_libraries(bookmarks
    konqbookmark
    akonadi_next
    ${KDE4_KDEUI_LIBS}
    ${KDE4_AKONADI_LIBS}
    ${KDE4_AKONADI_KMIME_LIBS})

install(TARGETS bookmarks ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( FILES bookmarks.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES bookmarks.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES bookmarksui.rc  DESTINATION  ${DATA_INSTALL_DIR}/bookmarks )
