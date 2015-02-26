#include "settingsstore.h"

#include <QCoreApplication>
#include <QDir>
#include <QString>
#include <QSettings>
#include <QStandardPaths>

SettingsStore::SettingsStore(const QString &fileName) {
    // Set platform
#ifdef Q_OS_LINUX
    platform = "linux/";
#elif defined(Q_OS_WIN)
    platform = "windows/";
#elif defined(Q_OS_MAC)
    platform = "mac/";
#endif


    // Read settings file
    settings = new QSettings(QSettings::IniFormat,
                             QSettings::UserScope,
                             "visualino",
                             "visualino");

    // If IDE path is empty, copy original settings
    if (settings->value(platform + "arduino_ide_path", "").toString() == "") {
        // Locate config.ini in standard locations
        QString settingsFile = QStandardPaths::locate(
                    QStandardPaths::DataLocation,
                    fileName,
                    QStandardPaths::LocateFile);

        // If couldn't locate config.ini in DataLocation dirs,
        // search in the binary path.
        if (settingsFile.isEmpty()) {
            settingsFile = QDir(QCoreApplication::applicationDirPath())
                    .filePath(fileName);
        }

        // Set final settings path
        QString localSettingsFile = settings->fileName();
        QString localSettingsDir = QDir(localSettingsFile).dirName();

        // Free settings file
        delete settings;

        // Create directory if it doesn't exist
        QDir dir(localSettingsDir);
        if (!dir.exists()) {
            dir.mkpath(localSettingsDir);
        }

        // Copy settings
        QFile::copy(settingsFile, localSettingsFile);

        // Reload settings
        settings = new QSettings(QSettings::IniFormat,
                                 QSettings::UserScope,
                                 "visualino",
                                 "visualino");
    }
}

SettingsStore::~SettingsStore() {
    delete settings;
}

QString SettingsStore::arduinoIdePath() {
    return relativePath("arduino_ide_path", "/usr/bin/arduino");
}

QString SettingsStore::tmpDirName() {
    return relativePath("tmp_dir_name", "/tmp/visualino/");
}

QString SettingsStore::tmpFileName() {
    return relativePath("tmp_file_name", "/tmp/visualino/visualino.ino");
}

QString SettingsStore::htmlIndex() {
    return relativePath("html_index", "/usr/share/visualino/html/index.html");
}

void SettingsStore::setArduinoIdePath(const QString &value) {
    settings->setValue(platform + "arduino_ide_path", value);
}

void SettingsStore::setTmpDirName(const QString &value) {
    settings->setValue(platform + "tmp_dir_name", value);
}

void SettingsStore::setTmpFileName(const QString &value) {
    settings->setValue(platform + "tmp_file_name", value);
}

void SettingsStore::setHtmlIndex(const QString &value) {
    settings->setValue(platform + "html_index", value);
}

QString SettingsStore::relativePath(const QString &value,
                                    const QString &defaultValue) {

    QString settingsValue = settings->value(platform + value,
                                             defaultValue).toString();

    // Append the binary path if relative
    if (QDir::isRelativePath(settingsValue)) {
        return QDir(QCoreApplication::applicationDirPath()).
                filePath(settingsValue);
    }

    return settingsValue;
}