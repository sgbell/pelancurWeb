cmake_minimum_required(VERSION 3.0)
set (CMAKE_CXX_STANDARD 14)

project (peluncurWeb)

set (QT_MIN_VERSION "5.3.0")
set (KF5_MIN_VERSION "5.2.0")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
set (CMAKE_AUTOMOC ON)

find_package (ECM 1.0.0 REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${ECM_MODULE_PATH}  ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(RESOURCES
       basetemplate.qrc)

include (KDEInstallDirs)
include (KDECMakeSettings)
include (KDECompilerSettings NO_POLICY_SCOPE)
include (FeatureSummary)

# Find Qt Modules
find_package (Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Widgets
)

QT5_ADD_RESOURCES(RES_SOURCES ${RESOURCES})

# Find KDE Modules
find_package (KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    CoreAddons
    I18n
    XmlGui
    TextWidgets
    ConfigWidgets
    Service
    Activities
    WindowSystem
    KIO
)

find_library (MHTD NAMES microhttpd)

feature_summary (WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

set (pelancurWeb_SRCS main.cpp 
                      mainwindow.cpp
                      RootController.cpp
                      ExecController.cpp
                      AssetsController.cpp
                      ActivityController.cpp
                      PlasmaMenuView.cpp
                      WebServer.cpp)

add_subdirectory(tpl_engine)

string (REPLACE "-fno-exceptions" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

add_executable (pelancurWeb ${pelancurWeb_SRCS} ${RES_SOURCES})
target_link_libraries (pelancurWeb
    Qt5::Widgets
    Qt5::Core
    KF5::CoreAddons
    KF5::I18n
    KF5::XmlGui
    KF5::TextWidgets
    KF5::ConfigWidgets
    KF5::Service
    KF5::Activities
    KF5::WindowSystem
    KF5::KIOCore
    KF5::KIOFileWidgets
    KF5::KIOWidgets
    KF5::KIONTLM
    microhttpd
    TplEngine
)

install (TARGETS pelancurWeb ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install (FILES pelancurWebui.rc DESTINATION ${KDE_INSTALL_KXMLGUI5DIR}/pelancurWeb)