#include <QGuiApplication>
#include <QWindow>

#include "xdg-file-dialog/xdg-file-dialog.h"

namespace {
const QString serviceName = QStringLiteral("org.freedesktop.portal.Desktop");
const QString objectPath = QStringLiteral("/org/freedesktop/portal/desktop");
}

XdgFileDialog::XdgFileDialog()
    : m_fileChooser(serviceName, objectPath, QDBusConnection::sessionBus())
{
}

void XdgFileDialog::setFolder(const QUrl &folder)
{
    if (folder == m_folder) {
        return;
    }

    m_folder = folder;
    emit folderChanged();
}

void XdgFileDialog::parseDbusReply(const QVariantMap &results)
{
    m_visible = false;
    emit visibleChanged();

    static QString uriName = QStringLiteral("uris");

    if (!results.contains(uriName)) {
        return;
    }

    QStringList uris = results[uriName].toStringList();

    if (uris.isEmpty()) {
        return;
    }

    QUrl folder = QUrl(uris.at(0));
    setFolder(folder);

    emit accepted();
    m_request->deleteLater();
}

bool XdgFileDialog::open()
{
    QString parentWindowId;

    if (QGuiApplication::platformName() == "xcb") {
        QWindow *window = qobject_cast<QWindow *>(m_parentWindow);
        if (window) {
            parentWindowId = QString("x11:0x%1").arg(window->winId(), 0, 16);
        }
    }

    QVariantMap options;
    options["directory"] = true;
    if (m_folder.isValid()) {
        options["current_folder"] = m_folder.toString();
    }

    QDBusPendingReply<QDBusObjectPath> reply = m_fileChooser.OpenFile(parentWindowId, m_title, options);
    reply.waitForFinished();

    if (reply.isError()) {
        qWarning() << "QDBusReply was error: " << reply.error();
        emit errorOccurred();
        return false;
    }

    if (reply.count() != 1) {
        qWarning() << "Incorrect DBUS reply for OrgFreedesktopPortalFileChooserInterface::Open";
        emit errorOccurred();
        return false;
    }

    QDBusObjectPath objectPath = reply.argumentAt(0).value<QDBusObjectPath>();

    m_request = new OrgFreedesktopPortalRequestInterface(serviceName,
                                                         objectPath.path(),
                                                         QDBusConnection::sessionBus());

    QObject::connect(m_request.get(),
                     &OrgFreedesktopPortalRequestInterface::Response,
                     this,
                     [&](uint response, const QVariantMap &results) {
                         parseDbusReply(results);
                     });
    return true;
}

QObject *XdgFileDialog::parentWindow() const
{
    return m_parentWindow;
}

void XdgFileDialog::setParentWindow(QObject *newParentWindow)
{
    if (m_parentWindow == newParentWindow)
        return;
    m_parentWindow = newParentWindow;
    emit parentWindowChanged();
}

void XdgFileDialog::setVisible(bool newVisible)
{
    if (m_visible == newVisible)
        return;

    if (newVisible) {
        if (!open()) {
            return;
        }
    } else {
        if (m_request) {
            m_request->Close();
        }
    }

    m_visible = newVisible;
    emit visibleChanged();
}

void XdgFileDialog::setTitle(const QString &newTitle)
{
    if (m_title == newTitle)
        return;
    m_title = newTitle;
    emit titleChanged();
}
