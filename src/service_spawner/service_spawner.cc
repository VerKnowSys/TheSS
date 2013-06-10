/**
 *  @author tallica, dmilith
 *
 *   © 2013 - VerKnowSys
 *
 */


#include <QtCore>

#include "../globals/globals.h"
#include "../notifications/notifications.h"
#include "../notifications/transports/hipchat_appender.h"
#include "logger.h"
#include "service_config.h"
#include "service_watcher.h"
#include "user_watcher.h"
#include "utils.h"


// void spawnSSForEachUser() {
//     /* spawn ss for each of uids in /Users */
//     Q_FOREACH(int userUid, gatherUserUids()) {
//         logDebug() << "Spawning user SS for:" << QString::number(userUid);

//         /* creating lock file for given user */
//         QString lockName = QString(USERS_HOME_DIR) + "/" + QString::number(userUid) + "/." + QString::number(userUid) + ".pid";
//         writeToFile(lockName, QString::number(0), false); /* NOTE: if pid == 0 it means that SS is runned from SS maintainer */

//         auto process = new SvdProcess("SS", userUid, false); // don't redirect output
//         process->spawnProcess(DEFAULT_SS_COMMAND);
//     }
// }


int main(int argc, char *argv[]) {

    QCoreApplication app(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName(DEFAULT_STRING_CODEC));
    QStringList args = app.arguments();
    QRegExp rxEnableDebug("-d");
    QRegExp rxEnableTrace("-t");
    QRegExp rxPrintVersion("-v");
    uint uid = getuid();

    bool debug = false, trace = false;
    for (int i = 1; i < args.size(); ++i) {
        if (rxEnableDebug.indexIn(args.at(i)) != -1 ) {
            debug = true;
        }
        if (rxEnableTrace.indexIn(args.at(i)) != -1 ) {
            debug = true;
            trace = true;
        }
        if (rxPrintVersion.indexIn(args.at(i)) != -1) {
            cout << "ServeD Service Spawner v" << APP_VERSION << ". " << COPYRIGHT;
            exit(EXIT_SUCCESS);
        }
    }

    /* Logger setup */
    ConsoleAppender *consoleAppender = new ConsoleAppender();
    Logger::registerAppender(consoleAppender);
    consoleAppender->setFormat("%t{dd-HH:mm:ss} [%-7l] <%c:(%F:%i)> %m\n");
    if (trace && debug)
        consoleAppender->setDetailsLevel(Logger::Trace);
    else if (debug && !trace)
        consoleAppender->setDetailsLevel(Logger::Debug);
    else {
        consoleAppender->setDetailsLevel(Logger::Info);
        consoleAppender->setFormat("%t{dd-HH:mm:ss} [%-7l] %m\n");
    }

    HipChatAppender *hipchatAppender = new HipChatAppender();
    Logger::registerAppender(hipchatAppender);
    hipchatAppender->setFormat("%t{dd-HH:mm:ss} [%-7l] %m\n");
    hipchatAppender->setDetailsLevel(Logger::Error);

    /* file lock setup */
    QString lockName = getHomeDir() + "/." + getenv("USER") + ".pid";
    if (QFile::exists(lockName)) {
        bool ok;
        QString aPid = QString(readFileContents(lockName).c_str()).trimmed();
        uint pid = aPid.toInt(&ok, 10);
        if (ok) {
            if (pidIsAlive(pid) or pid == 0) { /* NOTE: if pid == 0 it means that SS is runned from SS root maintainer */
                logError() << "Service Spawner is already running.";
                exit(LOCK_FILE_OCCUPIED_ERROR); /* can not open */
            } else
                logDebug() << "No alive Service Spawner pid found";

        } else {
            logWarn() << "Pid file is damaged or doesn't contains valid pid. File will be removed";
            QFile::remove(lockName);
        }
    }
    logDebug() << "Lock name:" << lockName;
    writeToFile(lockName, QString::number(getpid()), false); /* get process pid and record it to pid file no logrotate */

    signal(SIGINT, unixSignalHandler);
    signal(SIGPIPE, SIG_IGN); /* ignore broken pipe signal */

    if (uid == 0) {
        logInfo("Root Mode Service Spawner v" + QString(APP_VERSION) + ". " + QString(COPYRIGHT));
        setPublicDirPriviledges(getOrCreateDir(DEFAULT_PUBLIC_DIR));
        setupDefaultVPNNetwork();

        // TODO: auto coreginxWatcher = …

        /* disabled cause completely no output from user ss, and cause of temporary unmagability */
        // spawnSSForEachUser();

        /* Setting up root watchers */
        new SvdUserWatcher();

    } else {
        logInfo("Service Spawner v" + QString(APP_VERSION) + ". " + QString(COPYRIGHT));
        logDebug() << "Spawning for user:" << getenv("USER");

        logDebug() << "Checking user directory priviledges";
        setUserDirPriviledges(getHomeDir());

        /* Setting up user watchers */
        new SvdUserWatcher();
    }

    new LoggerTimer(consoleAppender);

    return app.exec();
}