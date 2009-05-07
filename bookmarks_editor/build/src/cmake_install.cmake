# Install script for directory: /home/edulix/proyectos/kde4/kdesvn/gsoc/bookmarks_editor/src

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
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
    IF(EXISTS "$ENV{DESTDIR}/usr/local/bin/bookmarks")
      FILE(RPATH_CHECK
           FILE "$ENV{DESTDIR}/usr/local/bin/bookmarks"
           RPATH "/usr/local/lib:/home/edulix/proyectos/kde4/lib:/home/edulix/proyectos/kde4/qt4/lib")
    ENDIF(EXISTS "$ENV{DESTDIR}/usr/local/bin/bookmarks")
    FILE(INSTALL DESTINATION "/usr/local/bin" TYPE EXECUTABLE FILES "/home/edulix/proyectos/kde4/kdesvn/gsoc/bookmarks_editor/build/src/bookmarks")
    IF(EXISTS "$ENV{DESTDIR}/usr/local/bin/bookmarks")
      FILE(RPATH_CHANGE
           FILE "$ENV{DESTDIR}/usr/local/bin/bookmarks"
           OLD_RPATH "/home/edulix/proyectos/kde4/lib:/home/edulix/proyectos/kde4/qt4/lib:::::::::::::::"
           NEW_RPATH "/usr/local/lib:/home/edulix/proyectos/kde4/lib:/home/edulix/proyectos/kde4/qt4/lib")
      IF(CMAKE_INSTALL_DO_STRIP)
        EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/usr/local/bin/bookmarks")
      ENDIF(CMAKE_INSTALL_DO_STRIP)
    ENDIF(EXISTS "$ENV{DESTDIR}/usr/local/bin/bookmarks")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg][Ff][Uu][Ll][Ll])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")
  FILE(INSTALL DESTINATION "/usr/local/share/applications/kde4" TYPE FILE FILES "/home/edulix/proyectos/kde4/kdesvn/gsoc/bookmarks_editor/src/bookmarks.desktop")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")
  FILE(INSTALL DESTINATION "/usr/local/share/config.kcfg" TYPE FILE FILES "/home/edulix/proyectos/kde4/kdesvn/gsoc/bookmarks_editor/src/bookmarks.kcfg")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")
  FILE(INSTALL DESTINATION "/usr/local/share/apps/bookmarks" TYPE FILE FILES "/home/edulix/proyectos/kde4/kdesvn/gsoc/bookmarks_editor/src/bookmarksui.rc")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")

