## ======================================================================== ##
## Copyright 2009-2017 Intel Corporation                                    ##
##                                                                          ##
## Licensed under the Apache License, Version 2.0 (the "License");          ##
## you may not use this file except in compliance with the License.         ##
## You may obtain a copy of the License at                                  ##
##                                                                          ##
##     http://www.apache.org/licenses/LICENSE-2.0                           ##
##                                                                          ##
## Unless required by applicable law or agreed to in writing, software      ##
## distributed under the License is distributed on an "AS IS" BASIS,        ##
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. ##
## See the License for the specific language governing permissions and      ##
## limitations under the License.                                           ##
## ======================================================================== ##

INCLUDE(GNUInstallDirs)

if (NOT DEFINED EMBREE_ZIP_MODE)
  set(EMBREE_ZIP_MODE OFF)
endif()

IF (NOT EMBREE_ZIP_MODE AND NOT WIN32 AND NOT APPLE)
  SET(CMAKE_INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR}/embree${EMBREE_VERSION_MAJOR})
  SET(CMAKE_INSTALL_FULL_BINDIR ${CMAKE_INSTALL_FULL_BINDIR}/embree${EMBREE_VERSION_MAJOR})
ENDIF()

# use full absolute path as install name
IF (NOT EMBREE_ZIP_MODE)
  SET(CMAKE_INSTALL_NAME_DIR ${CMAKE_INSTALL_FULL_LIBDIR})
ELSE()
  IF(APPLE)
    SET(CMAKE_INSTALL_RPATH "@loader_path/../lib")
  ELSE()
    SET(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
  ENDIF()
ENDIF()

##############################################################
# Install Headers
##############################################################

INSTALL(DIRECTORY include/embree2 DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT devel)
CONFIGURE_FILE(include/embree2/rtcore.h rtcore.h @ONLY)
CONFIGURE_FILE(include/embree2/rtcore.isph rtcore.isph @ONLY)
INSTALL(FILES ${PROJECT_BINARY_DIR}/rtcore.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/embree2 COMPONENT devel)
INSTALL(FILES ${PROJECT_BINARY_DIR}/rtcore.isph DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/embree2 COMPONENT devel)

##############################################################
# Install Models
##############################################################
INSTALL(DIRECTORY tutorials/models DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT examples)

##############################################################
# Install Documentation
##############################################################

INSTALL(FILES ${PROJECT_SOURCE_DIR}/LICENSE.txt DESTINATION ${CMAKE_INSTALL_DOCDIR} COMPONENT lib)
INSTALL(FILES ${PROJECT_SOURCE_DIR}/CHANGELOG.md DESTINATION ${CMAKE_INSTALL_DOCDIR} COMPONENT lib)
INSTALL(FILES ${PROJECT_SOURCE_DIR}/README.md DESTINATION ${CMAKE_INSTALL_DOCDIR} COMPONENT lib)
INSTALL(FILES ${PROJECT_SOURCE_DIR}/readme.pdf DESTINATION ${CMAKE_INSTALL_DOCDIR} COMPONENT lib)

SET(CPACK_NSIS_MENU_LINKS ${CPACK_NSIS_MENU_LINKS} "${CMAKE_INSTALL_DOCDIR}/LICENSE.txt" "LICENSE")
SET(CPACK_NSIS_MENU_LINKS ${CPACK_NSIS_MENU_LINKS} "${CMAKE_INSTALL_DOCDIR}/CHANGELOG.txt" "CHANGELOG")
SET(CPACK_NSIS_MENU_LINKS ${CPACK_NSIS_MENU_LINKS} "${CMAKE_INSTALL_DOCDIR}/README.md" "README.md")
SET(CPACK_NSIS_MENU_LINKS ${CPACK_NSIS_MENU_LINKS} "${CMAKE_INSTALL_DOCDIR}/readme.pdf" "readme.pdf")

##############################################################
# Install scripts to set embree paths
##############################################################

IF (EMBREE_ZIP_MODE)
  IF (WIN32)
  ELSEIF(APPLE)
    CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/scripts/install_macosx/embree-vars.sh embree-vars.sh @ONLY)
    CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/scripts/install_macosx/embree-vars.csh embree-vars.csh @ONLY)
    INSTALL(FILES ${PROJECT_BINARY_DIR}/embree-vars.sh DESTINATION "." COMPONENT lib)
    INSTALL(FILES ${PROJECT_BINARY_DIR}/embree-vars.csh DESTINATION "." COMPONENT lib)
  ELSE()
    CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/scripts/install_linux/embree-vars.sh embree-vars.sh @ONLY)
    CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/scripts/install_linux/embree-vars.csh embree-vars.csh @ONLY)
    INSTALL(FILES ${PROJECT_BINARY_DIR}/embree-vars.sh DESTINATION "." COMPONENT lib)
    INSTALL(FILES ${PROJECT_BINARY_DIR}/embree-vars.csh DESTINATION "." COMPONENT lib)
  ENDIF()
ENDIF()

##############################################################
# Install Embree CMake Configuration
##############################################################

IF (NOT EMBREE_ZIP_MODE)
  SET(EMBREE_CONFIG_VERSION ${EMBREE_VERSION})
ELSE()
  SET(EMBREE_CONFIG_VERSION ${EMBREE_VERSION_MAJOR})
ENDIF()

IF (APPLE AND NOT EMBREE_ZIP_MODE)
  CONFIGURE_FILE(scripts/install_macosx/uninstall.command uninstall.command @ONLY)
  INSTALL(PROGRAMS "${PROJECT_BINARY_DIR}/uninstall.command" DESTINATION ${CMAKE_INSTALL_BINDIR}/.. COMPONENT lib)
ENDIF()

IF (WIN32)
  SET(EMBREE_LIBRARY_NAME embree.lib)
ELSE()
  SET(EMBREE_LIBRARY_NAME ${CMAKE_SHARED_LIBRARY_PREFIX}embree${CMAKE_SHARED_LIBRARY_SUFFIX})
ENDIF()
IF (WIN32 OR EMBREE_ZIP_MODE)
  # for local "installs" and on Windows we want the cmake config files placed
  # in the install root, such that users can point the CMake variable
  # embree_DIR just to the install folder
  SET(EMBREE_CMAKECONFIG_DIR ".")
  SET(EMBREE_RELATIV_ROOT_DIR ".")
ELSE()
  SET(EMBREE_CMAKECONFIG_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/embree-${EMBREE_VERSION}")
  SET(EMBREE_RELATIV_ROOT_DIR "../../..")
ENDIF()
CONFIGURE_FILE(common/cmake/embree-config.cmake embree-config-install.cmake @ONLY)
CONFIGURE_FILE(common/cmake/embree-config-version.cmake embree-config-version.cmake @ONLY)
# create a config file for the build directory
CONFIGURE_FILE(common/cmake/embree-config-builddir.cmake embree-config.cmake @ONLY)

INSTALL(FILES "${PROJECT_BINARY_DIR}/embree-config-install.cmake" DESTINATION ${EMBREE_CMAKECONFIG_DIR} RENAME "embree-config.cmake" COMPONENT devel)
INSTALL(FILES "${PROJECT_BINARY_DIR}/embree-config-version.cmake" DESTINATION ${EMBREE_CMAKECONFIG_DIR} COMPONENT devel)

##############################################################
# CPack specific stuff
##############################################################

SET(CPACK_PACKAGE_NAME "Embree")
SET(CPACK_PACKAGE_FILE_NAME "embree-${EMBREE_VERSION}")
#SET(CPACK_PACKAGE_ICON ${PROJECT_SOURCE_DIR}/embree-doc/images/icon.png)
#SET(CPACK_PACKAGE_RELOCATABLE TRUE)
SET(CPACK_STRIP_FILES TRUE)

SET(CPACK_PACKAGE_VERSION_MAJOR ${EMBREE_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${EMBREE_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${EMBREE_VERSION_PATCH})
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Embree: High Performance Ray Tracing Kernels")
SET(CPACK_PACKAGE_VENDOR "Intel Corporation")
SET(CPACK_PACKAGE_CONTACT embree_support@intel.com)

SET(CPACK_COMPONENT_LIB_DISPLAY_NAME "Library")
SET(CPACK_COMPONENT_LIB_DESCRIPTION "The Embree library including documentation.")

SET(CPACK_COMPONENT_DEVEL_DISPLAY_NAME "Development")
SET(CPACK_COMPONENT_DEVEL_DESCRIPTION "Header Files for C and ISPC required to develop applications with Embree.")

SET(CPACK_COMPONENT_EXAMPLES_DISPLAY_NAME "Examples")
SET(CPACK_COMPONENT_EXAMPLES_DESCRIPTION "Tutorials demonstrating how to use Embree.")

# dependencies between components
SET(CPACK_COMPONENT_DEVEL_DEPENDS lib)
SET(CPACK_COMPONENT_EXAMPLES_DEPENDS lib)
SET(CPACK_COMPONENT_LIB_REQUIRED ON) # always install the libs

# point to readme and license files
SET(CPACK_RESOURCE_FILE_README ${PROJECT_SOURCE_DIR}/README.md)
SET(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/LICENSE.txt)

# Windows specific settings
IF(WIN32)

  IF (CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(ARCH x64)
    SET(PROGRAMFILES "\$PROGRAMFILES64")
	SET(CPACK_PACKAGE_NAME "${CPACK_PACKAGE_NAME} x64")
  ELSE()
    SET(ARCH win32)
    SET(PROGRAMFILES "\$PROGRAMFILES")
	SET(CPACK_PACKAGE_NAME "${CPACK_PACKAGE_NAME} Win32")
  ENDIF()

  # NSIS specific settings
  IF (NOT EMBREE_ZIP_MODE)
    SET(CPACK_GENERATOR NSIS)
    SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}.${ARCH}")
    SET(CPACK_COMPONENTS_ALL lib devel examples)
    SET(CPACK_NSIS_INSTALL_ROOT "${PROGRAMFILES}\\\\Intel")
    SET(CPACK_PACKAGE_INSTALL_DIRECTORY "Embree v${EMBREE_VERSION} ${ARCH}")
    SET(CPACK_NSIS_DISPLAY_NAME "Embree v${EMBREE_VERSION} ${ARCH}")
    SET(CPACK_NSIS_PACKAGE_NAME "Embree v${EMBREE_VERSION} ${ARCH}")
    SET(CPACK_NSIS_URL_INFO_ABOUT http://embree.github.io/)
    #SET(CPACK_NSIS_HELP_LINK http://embree.github.io/downloads.html#windows)
    SET(CPACK_NSIS_MUI_ICON ${PROJECT_SOURCE_DIR}/scripts/install_windows/icon32.ico)
    SET(CPACK_NSIS_CONTACT ${CPACK_PACKAGE_CONTACT})
	#SET(CPACK_NSIS_EXTRA_PREINSTALL_COMMANDS ${CPACK_NSIS_EXTRA_PREINSTALL_COMMANDS} "\n CreateDirectory \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\"")
	#SET(CPACK_NSIS_EXTRA_PREINSTALL_COMMANDS ${CPACK_NSIS_EXTRA_PREINSTALL_COMMANDS} "\n CreateDirectory \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\documentation\\\"")
	#SET(CPACK_NSIS_EXTRA_PREINSTALL_COMMANDS ${CPACK_NSIS_EXTRA_PREINSTALL_COMMANDS} "\n CreateDirectory \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\tutorials\\\" \n")
	#SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS  ${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS} "\n ${UNINSTALL_LIST}\n RMDir \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\documentation\\\"")
	#SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS  ${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS} "\n ${UNINSTALL_LIST}\n RMDir \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\tutorials\\\"")
	#SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS  ${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS} "\n ${UNINSTALL_LIST}\n RMDir \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\"\n")
    IF (EMBREE_TESTING_PACKAGE)
      ADD_TEST(NAME "BuildPackage" WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMAND ${PROJECT_SOURCE_DIR}/scripts/package_win.bat ${CPACK_PACKAGE_FILE_NAME}.exe ${CMAKE_BUILD_TYPE})
    ENDIF()
  ELSE()
    SET(CPACK_GENERATOR ZIP)
    SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}.${ARCH}.windows")
    SET(CPACK_MONOLITHIC_INSTALL 1)
    IF (EMBREE_TESTING_PACKAGE)
      ADD_TEST(NAME "BuildPackage" WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMAND ${PROJECT_SOURCE_DIR}/scripts/package_win.bat  ${CPACK_PACKAGE_FILE_NAME}.zip ${CMAKE_BUILD_TYPE})
    ENDIF()
  ENDIF()

# MacOSX specific settings
ELSEIF(APPLE)

  CONFIGURE_FILE(README.md README.txt)
  SET(CPACK_RESOURCE_FILE_README ${PROJECT_BINARY_DIR}/README.txt)

  IF (NOT EMBREE_ZIP_MODE)
    SET(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
    SET(CPACK_GENERATOR PackageMaker)
    SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}.x86_64")
    #SET(CPACK_COMPONENTS_ALL lib devel examples)
    SET(CPACK_MONOLITHIC_INSTALL 1)
    SET(CPACK_PACKAGE_NAME embree-${EMBREE_VERSION})
    SET(CPACK_PACKAGE_VENDOR "intel") # creates short name com.intel.embree2.xxx in pkgutil
    SET(CPACK_OSX_PACKAGE_VERSION 10.7)
    IF (EMBREE_TESTING_PACKAGE)
      ADD_TEST(NAME "BuildPackage" WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMAND ${PROJECT_SOURCE_DIR}/scripts/package_macosx.sh ${CMAKE_BUILD_TYPE} ${CPACK_PACKAGE_FILE_NAME}.dmg)
    ENDIF()
  ELSE()
    SET(CPACK_GENERATOR TGZ)
    SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}.x86_64.macosx")
    SET(CPACK_MONOLITHIC_INSTALL 1)
    IF (EMBREE_TESTING_PACKAGE)
      ADD_TEST(NAME "BuildPackage" WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMAND ${PROJECT_SOURCE_DIR}/scripts/package_macosx.sh ${CMAKE_BUILD_TYPE} ${CPACK_PACKAGE_FILE_NAME}.tgz)
    ENDIF()
  ENDIF()

# Linux specific settings
ELSE()


  IF (NOT EMBREE_ZIP_MODE)

    SET(CPACK_GENERATOR RPM)
    SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-${CPACK_RPM_PACKAGE_RELEASE}.x86_64")
    SET(CPACK_COMPONENTS_ALL devel lib examples)
    SET(CPACK_RPM_COMPONENT_INSTALL ON)
    SET(CPACK_RPM_PACKAGE_LICENSE "ASL 2.0") # Apache Software License, Version 2.0
    SET(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")
    #SET(CPACK_RPM_CHANGELOG_FILE "") # ChangeLog of the RPM; also CHANGELOG.md is not in the required format
    SET(CPACK_RPM_PACKAGE_ARCHITECTURE x86_64)
    SET(CPACK_RPM_PACKAGE_URL http://embree.github.io/)
    SET(CPACK_RPM_DEFAULT_DIR_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE) # otherwise cmake directory has wrong 775 permissions

    # post install and uninstall scripts
    SET(CPACK_RPM_lib_POST_INSTALL_SCRIPT_FILE ${PROJECT_SOURCE_DIR}/common/cmake/rpm_ldconfig.sh)
    SET(CPACK_RPM_lib_POST_UNINSTALL_SCRIPT_FILE ${PROJECT_SOURCE_DIR}/common/cmake/rpm_ldconfig.sh)
    IF (EMBREE_TESTING_PACKAGE)
      ADD_TEST(NAME "BuildPackage" WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMAND ${PROJECT_SOURCE_DIR}/scripts/package_linux.sh ${EMBREE_ZIP_MODE} ${EMBREE_VERSION})
    ENDIF()
  ELSE()
  
    SET(CPACK_GENERATOR TGZ)
    SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}.x86_64.linux")
    SET(CPACK_MONOLITHIC_INSTALL 1)
    IF (EMBREE_TESTING_PACKAGE)
      ADD_TEST(NAME "BuildPackage" WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMAND ${PROJECT_SOURCE_DIR}/scripts/package_linux.sh ${EMBREE_ZIP_MODE} ${EMBREE_VERSION})
    ENDIF()
  ENDIF()
  
ENDIF()
