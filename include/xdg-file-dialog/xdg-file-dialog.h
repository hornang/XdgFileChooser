#pragma once

#include <QObject>
#include <QUrl>
#include <QPointer>

#include "OrgFreedesktopPortalFileChooser.h"
#include "OrgFreedesktopPortalRequest.h"

#include "xdg-file-dialog_export.h"

class XDG_FILE_DIALOG_EXPORT XdgFileDialog : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl folder MEMBER m_folder NOTIFY folderChanged)
    Q_PROPERTY(QString title MEMBER m_title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QObject *parentWindow READ parentWindow WRITE setParentWindow NOTIFY parentWindowChanged)
    Q_PROPERTY(bool visible MEMBER m_visible WRITE setVisible NOTIFY visibleChanged)

public:
    XdgFileDialog();
    QObject *parentWindow() const;
    void setTitle(const QString &newTitle);
    void setParentWindow(QObject *newParentWindow);
    void setVisible(bool newVisible);

signals:
    void errorOccurred();
    void folderChanged();
    void accepted();
    void titleChanged();
    void parentWindowChanged();
    void visibleChanged();

private:
    void parseDbusReply(const QVariantMap &results);
    void setFolder(const QUrl &folder);
    bool open();

    QDBusConnection m_dbusConnection = QDBusConnection::sessionBus();
    OrgFreedesktopPortalFileChooserInterface m_fileChooser;
    QPointer<OrgFreedesktopPortalRequestInterface> m_request;
    QObject *m_parentWindow = nullptr;
    QUrl m_folder;
    QString m_title;
    bool m_visible = false;
};
