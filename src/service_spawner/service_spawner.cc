/**
 *  @author tallica, dmilith
 *
 *   © 2013 - VerKnowSys
 *
 */


#include <QtCore>

#include "../globals/globals.h"
#include "../core/logger-core.h"
#include "../notifications/notifications.h"
#include "service_config.h"
#include "service_watcher.h"
#include "user_watcher.h"
#include "../core/utils.h"


int main(int argc, char *argv[]) {

    QCoreApplication app(argc, argv);
    setDefaultEncoding();
    QCoreApplication::setOrganizationName("VerKnowSys");
    QCoreApplication::setOrganizationDomain("verknowsys.com");
    QCoreApplication::setApplicationName("ServeD");
    QStringList args = app.arguments();
    bool background = true; /* by default launch svdss as daemon */
    QRegExp rxEnableForeground("-f"); /* run in foreground */
    QRegExp rxEnableDebug("-d");
    QRegExp rxEnableTrace("-t");
    QRegExp rxPrintVersion("-v");
    QRegExp rxInteractive("-i");
    uint uid = getuid();

    bool debug = false, trace = false, interactive = false;
    for (int i = 1; i < args.size(); ++i) {
        if (rxEnableDebug.indexIn(args.at(i)) != -1 ) {
            debug = true;
        }
        if (rxEnableForeground.indexIn(args.at(i)) != -1 ) {
            background = false;
        }
        if (rxEnableTrace.indexIn(args.at(i)) != -1 ) {
            debug = true;
            trace = true;
        }
        if (rxInteractive.indexIn(args.at(i)) != -1 ) {
            interactive = true;
        }
        if (rxPrintVersion.indexIn(args.at(i)) != -1) {
            cout << "ServeD Service Spawner v" << APP_VERSION  ". " << COPYRIGHT << endl;
            return EXIT_SUCCESS;
        }
    }

    using namespace QsLogging;
    Logger& logger = Logger::instance();
    const QString sLogPath(DEFAULT_SS_LOG_FILE);
    Level logLevel = InfoLevel;
    if (debug)
        logLevel = DebugLevel;
    if (trace)
        logLevel = TraceLevel;
    logger.setLoggingLevel(logLevel);

    /* Logger setup */
    if (not background) {
        DestinationPtr consoleDestination(DestinationFactory::MakeDebugOutputDestination());
        logger.addDestination(consoleDestination);
    } else {
        DestinationPtr fileDestination(
            DestinationFactory::MakeFileDestination(sLogPath, DisableLogRotation, MaxSizeBytes(512), MaxOldLogCount(0)));
        logger.addDestination(fileDestination);
    }
    new LoggerTimer(&logger);

    signal(SIGTERM, unixSignalHandler);
    // signal(SIGPIPE, SIG_IGN); /* ignore broken pipe signal */

    QString pidFile = getHomeDir() + "/." + DEFAULT_USER_NAME + ".pid";
    if (getuid() == 0) {
        pidFile = getHomeDir() + "/.root.pid";
    }
    if (QFile::exists(pidFile)) {
        bool ok;
        QString aPid = readFileContents(pidFile).trimmed();
        uint pid = aPid.toInt(&ok, 10);
        if (ok) {
            if (pidIsAlive(pid) or pid == 0) { /* NOTE: if pid == 0 it means that SS is runned from SS root maintainer */
                logError() << "Service Spawner is already running with pid:" << QString::number(pid);
                return LOCK_FILE_OCCUPIED_ERROR; /* can not open */
            } else {
                logInfo() << "Dead Service Spawner pid found. Removing pid file.";
                QFile::remove(pidFile);
            }

        } else {
            logWarn() << "Pid file is damaged or doesn't contains valid pid. Removing pid file.";
            QFile::remove(pidFile);
        }
    }

    /* mem lock setup */
    // auto startTimer = QDateTime::currentMSecsSinceEpoch() / 5000;
    // auto hash = new QCryptographicHash(QCryptographicHash::Sha1);
    // hash->addData(QString::number(startTimer).toUtf8(), QString::number(startTimer).length());
    // auto shaId = hash->result().toHex();
    // delete hash;
    // const QString key = shaId.right(25) + ":" + QString::number(uid);
    // QSharedMemory memLock(key);
    // if (not memLock.create(1)) {
    //     logError() << "Memory locked! You cannot launch more than one process per 5 seconds. (key: " + key + ")";
    //     logError() << "Internal cause:" << memLock.errorString();
    //     return EXIT_SUCCESS;
    // }

    auto userEntries = QDir(getHomeDir() + QString(DEFAULT_USER_IGNITERS_DIR)).entryList(QDir::Files);
    auto rootEntries = QDir(QString(SYSTEM_USERS_DIR) + QString(DEFAULT_USER_IGNITERS_DIR)).entryList(QDir::Files);
    logDebug() << "Validating igniters existance.\nUser entries:" << userEntries << "\nRoot entries:" << rootEntries;
    if (((getuid() == 0) and rootEntries.isEmpty()) or userEntries.isEmpty()) {
        logFatal() << "Please install igniters for TheSS to work with first.";
    }

    #ifdef THESS_TEST_MODE
        logFatal() << "Please rebuild TheSS after tests. Service Spawner can't be running in test mode.";
    #endif

    logInfo() << "=================================================================================";

    writeToFile(pidFile, QString::number(getpid()), false); /* get process pid and record it to pid file no logrotate */
    notification("Launching TheSS v" + QString(APP_VERSION) + " on host: " + QHostInfo::localHostName() + " for uid: " + QString::number(uid));

    QSettings settings;
    if (interactive) {
        char input[255];
        logInfo() << "Enter" << NOTIFICATIONS_API_HOST << ":";
        cin.getline(input, 255);
        if (not QString(input).trimmed().isEmpty())
            settings.setValue(NOTIFICATIONS_API_HOST, input);

        logInfo() << "Enter" << NOTIFICATIONS_API_TOKEN << ":";
        cin.getline(input, 255);
        if (not QString(input).trimmed().isEmpty())
            settings.setValue(NOTIFICATIONS_API_TOKEN, input);
    }

    if (uid == 0) {
        logInfo() << "Root Mode Service Spawner v" + QString(APP_VERSION) + ". " + QString(COPYRIGHT);
        setPublicDirPriviledges(getOrCreateDir(DEFAULT_PUBLIC_DIR));
        SvdService::setupDefaultVPNNetwork();
    } else {
        logInfo() << "Service Spawner v" + QString(APP_VERSION) + ". " + COPYRIGHT;
        setUserDirPriviledges(getHomeDir());
    }

    /* Launching watchers */
    new SvdUserWatcher();

    return app.exec();
}
