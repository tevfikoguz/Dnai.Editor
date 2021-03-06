#include <QStringList>
#include <QByteArray>
#include <QDebug>
#include <string>
#include <QThread>
#include <QTcpServer>
#include <QHostAddress>
#include <QSystemSemaphore>
#include <QSettings>
#include <QVariant>
#include <QGuiApplication>
#include <QDir>
#include <cstdlib>
#include <iostream>
#include <QFileInfo>
#include <QTimer>

#include "dnai/processmanager.h"

ProcessManager::ProcessManager(QString const &file)
    : QObject(nullptr),
      m_file(file),
      m_server(),
      m_core(),
      m_updaterApp(),
      m_port(0),
      m_isTimeOut(false)
{
}

void ProcessManager::setFile(const QString &file)
{
    m_file = file;
}

ProcessManager::~ProcessManager() {
}

void ProcessManager::setTimeOut()
{
    m_isTimeOut = true;
}

void ProcessManager::closeAll() {
    qDebug() << "==ProcessManager== Closing server process and core process";
    m_server.terminate();
    m_core.terminate();

    //Continue after terminate or timeout
    QTimer *timer = new QTimer;
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(setTimeOut()));
    timer->start(8000);

    while ((!m_server.atEnd() || !m_core.atEnd()) && !m_isTimeOut)
        QThread::msleep(200);
}


void ProcessManager::launchUpdater(QString const &actualVer, QString const &newVersion) {
    if (m_updaterApp.size() != 0) {
    QProcess proc;
    QStringList args;

#if defined(Q_OS_MAC)
    QString path = QGuiApplication::applicationDirPath();
    int len = sizeof("/DNAI.app/Contents/MacOS");
    int idx = path.length() - (len - 1);
    path.remove(idx, len);

    QFile moveUpdater;
    QString applicationUpdater = QDir::tempPath() + "/DNAI_UPDATER.app";
    QFileInfo updaterFile(applicationUpdater);

    if (updaterFile.exists()) {
        QDir dir(applicationUpdater);
        dir.removeRecursively();
    }

    moveUpdater.rename(m_updaterApp, applicationUpdater);

    QString app = "\"" + applicationUpdater + "/Contents/MacOS/DNAI Updater\" " +  actualVer + " " + newVersion + " \"" + path + "\" " + "DNAI";
    qDebug() << app;
    proc.startDetached(app);
#else
    args.push_back(actualVer);
    args.push_back(newVersion);

 /*   QString appDirPath = QGuiApplication::applicationDirPath();
    while (appDirPath.size() >= 0 && appDirPath.back() != '/') {
        appDirPath.remove(appDirPath.size() - 1, 1);
    }
    appDirPath.remove(appDirPath.size() - 1, 1);
    args.push_back(appDirPath);
    qDebug() << appDirPath;*/
    args.push_back(QGuiApplication::applicationDirPath());

    args.push_back("DNAI");
    qDebug() << "==ProcessManager== ApplicationDirPath: " << QGuiApplication::applicationDirPath();
    QString updaterDir = m_updaterApp;
    m_updaterApp = "\"" + m_updaterApp + "\" ";// +  actualVer + " " + newVersion + " \"" + QGuiApplication::applicationDirPath() + "\" " + "DNAI";
    qDebug() << "==ProcessManager== Updater application: " << m_updaterApp;

//    int idx = m_updaterApp.size();
    while (updaterDir.size() >= 0 && updaterDir.back() != '/') {
        updaterDir.remove(updaterDir.size() - 1, 1);
    }
    updaterDir.remove(updaterDir.size() - 1, 1);
    qDebug() << "==ProcessManager== Updater directory: " << updaterDir;

    /*QDir dirRemove(QDir::tempPath() + "/DNAI_UPDATER");

    dirRemove.removeRecursively();*/

    QDir dir;

    bool returnRename = dir.rename(updaterDir, QDir::tempPath() + "/DNAI_UPDATER");
    qDebug() << "==ProcessManager== Updater renamed directory: " << returnRename;
    proc.startDetached(QDir::tempPath() + "/DNAI_UPDATER/DNAI Updater.exe", args);

#endif
    } else {
        qDebug() << "==ProcessManager== [WARNING] Can't launch DNAI Updater";
    }
}

quint16 ProcessManager::findUnusedPort() const {
     QTcpServer serverTmp;
     quint16 port = 0;

     if (serverTmp.listen(QHostAddress::LocalHost, 0) == false) {
         return port;
     }
     port = serverTmp.serverPort();
     serverTmp.close();

     return port;
}

void ProcessManager::launch() {
    QSettings settingsBin(m_file, QSettings::IniFormat);

    m_updaterApp = settingsBin.value("BINARIES/updater", "").toString();
    m_updaterApp.replace("{OUT_DIR}", QGuiApplication::applicationDirPath());

    QStringList argumentsServer;
    QSystemSemaphore sem("SERVER_CORE_DNAI", 0, QSystemSemaphore::Create);

    m_port = findUnusedPort();
    if (m_port <= 0) {
        qDebug() << "==ProcessManager== No port available" << m_port;
        return;
    }

    QString serverPath = settingsBin.value("BINARIES/server", "").toString();
    if (serverPath.size() == 0) {
        qDebug() << "==ProcessManager== Failed to launch server from file " << m_file;
        qDebug() << "==ProcessManager== DO BINARIES/server=\"./PATH/Server\" ";
        return;
    }

    serverPath.replace("{OUT_DIR}", QGuiApplication::applicationDirPath());

    argumentsServer << "-p";
    QString portStr;
    portStr.setNum(m_port);
    argumentsServer << portStr;
    argumentsServer << "-S";
    argumentsServer << sem.key();

    m_server.start(serverPath, argumentsServer);
    if (m_server.waitForStarted() == false) {
        qDebug() << "==ProcessManager== [FAILED] Failed to launch Server";
        qDebug() << "==ProcessManager== [FAILED]" << serverPath;
        return;
    }

    qDebug() << "==ProcessManager== Server started on port: " << portStr;

    sem.acquire();

    QString corePath = settingsBin.value("BINARIES/core", "").toString();
    if (corePath.size() == 0) {
        qDebug() << "==ProcessManager== Failed to launch Core from file " << m_file;
        qDebug() << "==ProcessManager== DO BINARIES/core=\"[mono] ./PATH/CoreDaemon.exe\" ";
        return;
    }

    corePath.replace("{OUT_DIR}", QGuiApplication::applicationDirPath());

    QStringList argumentsCore;

    argumentsCore << "-p";
    argumentsCore << portStr;

    m_core.start(corePath, argumentsCore);
    if (m_core.waitForStarted() == false) {
        qDebug() << "==ProcessManager== [FAILED] Failed to launch Core";
        qDebug() << "==ProcessManager== [FAILED]" << corePath <<  argumentsCore;
        abort();
//        return;
    }

    qDebug() << "==ProcessManager== Server and Core successfully launched (Processes OK)!";
}

quint16 ProcessManager::getServerPort() {
    return m_port;
}
