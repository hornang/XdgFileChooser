# Native folder chooser for Qt6 apps via desktop portals

This is a Qt based library to launch the native file dialog using [xdg-desktop-portal](https://github.com/flatpak/xdg-desktop-portal/tree/main).

The motivation behind this library was my struggle with using [QML FolderDialog](https://doc.qt.io/qt-6/qml-qtquick-dialogs-folderdialog.html) on Ubuntu 22.04 and Qt 6. 

I could not resolve the following error:

```
Bail out! Gtk:ERROR:../../../../gtk/gtkiconhelper.c:494:ensure_surface_for_gicon: assertion failed (error == NULL): Failed to load /usr/share/icons/Yaru/16x16/status/image-missing.png: Fatal error reading PNG image file: Invalid IHDR data (gdk-pixbuf-error-quark, 0)
**
Gtk:ERROR:../../../../gtk/gtkiconhelper.c:494:ensure_surface_for_gicon: assertion failed (error == NULL): Failed to load /usr/share/icons/Yaru/16x16/status/image-missing.png: Fatal error reading PNG image file: Invalid IHDR data (gdk-pixbuf-error-quark, 0)

```

I suspect this is a compatibility issue between the Qt6 Gui module and the GTK library in Ubuntu 22.04.

To use the library add it as a submodule in your project and include in your CMake build:

- `add_subdirectory(xdg-file-dialog)`
- `target_link_libraries(yourapp xdg-file-dialog)` 

Then register the type to QML:

```cpp

#include <xdg-file-dialog/xdg-file-dialog.h>
...
qmlRegisterType<XdgFileDialog>("YourApp", 1, 0, "XdgFileDialog");

```

This library uses the D-Bus directly because [libportal](https://github.com/flatpak/libportal) doesn't support the "directory" flag to let user select a directory and not file(s). (https://github.com/flatpak/xdg-desktop-portal/blob/main/data/org.freedesktop.portal.FileChooser.xml#L66-L68)

The current implementation only supports selecting folder.

