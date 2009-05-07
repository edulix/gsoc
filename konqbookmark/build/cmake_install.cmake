# Install script for directory: /home/edulix/proyectos/kde4/kdesvn/gsoc/konqbookmark

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
    IF(EXISTS "$ENV{DESTDIR}/home/edulix/proyectos/kde4/lib/kde4/akonadi_serializer_konqbookmark.so")
      FILE(RPATH_CHECK
           FILE "$ENV{DESTDIR}/home/edulix/proyectos/kde4/lib/kde4/akonadi_serializer_konqbookmark.so"
           RPATH "/home/edulix/proyectos/kde4/lib:/home/edulix/proyectos/kde4/lib:/home/edulix/proyectos/kde4/qt4/lib")
    ENDIF(EXISTS "$ENV{DESTDIR}/home/edulix/proyectos/kde4/lib/kde4/akonadi_serializer_konqbookmark.so")
    FILE(INSTALL DESTINATION "/home/edulix/proyectos/kde4/lib/kde4" TYPE MODULE FILES "/home/edulix/proyectos/kde4/kdesvn/gsoc/konqbookmark/build/lib/akonadi_serializer_konqbookmark.so")
    IF(EXISTS "$ENV{DESTDIR}/home/edulix/proyectos/kde4/lib/kde4/akonadi_serializer_konqbookmark.so")
      FILE(RPATH_CHANGE
           FILE "$ENV{DESTDIR}/home/edulix/proyectos/kde4/lib/kde4/akonadi_serializer_konqbookmark.so"
           OLD_RPATH "/home/edulix/proyectos/kde4/lib:/home/edulix/proyectos/kde4/qt4/lib::::::::::::::::::::::::::::::::"
           NEW_RPATH "/home/edulix/proyectos/kde4/lib:/home/edulix/proyectos/kde4/lib:/home/edulix/proyectos/kde4/qt4/lib")
      IF(CMAKE_INSTALL_DO_STRIP)
        EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/edulix/proyectos/kde4/lib/kde4/akonadi_serializer_konqbookmark.so")
      ENDIF(CMAKE_INSTALL_DO_STRIP)
    ENDIF(EXISTS "$ENV{DESTDIR}/home/edulix/proyectos/kde4/lib/kde4/akonadi_serializer_konqbookmark.so")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg][Ff][Uu][Ll][Ll])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")
  FILE(INSTALL DESTINATION "/home/edulix/proyectos/kde4/share/apps/akonadi/plugins/serializer" TYPE FILE FILES "/home/edulix/proyectos/kde4/kdesvn/gsoc/konqbookmark/akonadi_serializer_konqbookmark.desktop")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")

IF(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
ELSE(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
ENDIF(CMAKE_INSTALL_COMPONENT)

FILE(WRITE "/home/edulix/proyectos/kde4/kdesvn/gsoc/konqbookmark/build/${CMAKE_INSTALL_MANIFEST}" "")
FOREACH(file ${CMAKE_INSTALL_MANIFEST_FILES})
  FILE(APPEND "/home/edulix/proyectos/kde4/kdesvn/gsoc/konqbookmark/build/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
ENDFOREACH(file)
