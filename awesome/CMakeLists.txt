project( awesome )
find_package( KDE4 REQUIRED )
include_directories( ${KDE4_INCLUDES} )

set(konqsidebar_awesome_SRCS
  awesome.cpp awesomewidget.cpp)

kde4_add_plugin(konqsidebar_awesome ${konqsidebar_awesome_SRCS})

target_link_libraries(konqsidebar_awesome ${KDE4_KPARTS_LIBS} konqsidebarplugin)

install( TARGETS konqsidebar_awesome  DESTINATION ${PLUGIN_INSTALL_DIR} )
install( FILES awesome_add.desktop    DESTINATION ${DATA_INSTALL_DIR}/konqsidebartng/add )
