include(FetchContent)

FetchContent_Declare(xdg-desktop-portal
    GIT_REPOSITORY https://github.com/flatpak/xdg-desktop-portal
    GIT_TAG        1.18.2
)

FetchContent_MakeAvailable(xdg-desktop-portal)

find_package(Qt6
    COMPONENTS Core Gui DBus
    REQUIRED
)

set(ANNOTATION_STRING "<annotation name=\"org.qtproject.QtDBus.QtTypeName.In2\" value=\"QVariantMap\"/>")

file(READ ${CMAKE_BINARY_DIR}/_deps/xdg-desktop-portal-src/data/org.freedesktop.portal.FileChooser.xml FILE_CONTENTS)

string(REPLACE "<method name=\"OpenFile\">" "<method name=\"OpenFile\">\n${ANNOTATION_STRING}" FILE_CONTENTS ${FILE_CONTENTS})
string(REPLACE "<method name=\"SaveFile\">" "<method name=\"SaveFile\">\n${ANNOTATION_STRING}" FILE_CONTENTS ${FILE_CONTENTS})
string(REPLACE "<method name=\"SaveFiles\">" "<method name=\"SaveFiles\">\n${ANNOTATION_STRING}" FILE_CONTENTS ${FILE_CONTENTS})

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.portal.FileChooser.xml ${FILE_CONTENTS})

qt_add_dbus_interface(
    ORG_FREEDESKTOP_PORTAL_FILECHOOSER_SRC
    ${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.portal.FileChooser.xml
    OrgFreedesktopPortalFileChooser
)

qt_add_dbus_interface(
    ORG_FREEDESKTOP_PORTAL_REQUEST_SRC
    ${CMAKE_BINARY_DIR}/_deps/xdg-desktop-portal-src/data/org.freedesktop.portal.Request.xml
    OrgFreedesktopPortalRequest
)

add_library(xdg-file-dialog
    src/xdg-file-dialog.cpp
    include/xdg-file-dialog/xdg-file-dialog.h
    ${ORG_FREEDESKTOP_PORTAL_FILECHOOSER_SRC}
    ${ORG_FREEDESKTOP_PORTAL_REQUEST_SRC}
)

include(GenerateExportHeader)
generate_export_header(xdg-file-dialog)

set_target_properties(xdg-file-dialog PROPERTIES AUTOMOC TRUE)

target_include_directories(xdg-file-dialog
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        include
)

target_link_libraries(xdg-file-dialog PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::DBus
    Qt6::Gui
)
