/**
 *  @author dmilith
 *
 *   © 2013 - VerKnowSys
 *
 */


#include "../core/death_watch.h"


int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    setDefaultEncoding();
    QStringList args = app.arguments();
    QRegExp rxEnableDebug("-d");

    /* Logger setup */
    ConsoleAppender *consoleAppender = new ConsoleAppender();
    Logger::registerAppender(consoleAppender);
    consoleAppender->setFormat("%t{dd-HH:mm:ss} [%-7l] <%c:(%F:%i)> %m\n");
    bool debug = false, trace = false;
    for (int i = 1; i < args.size(); ++i) {
        if (rxEnableDebug.indexIn(args.at(i)) != -1 ) {
            debug = true;
        }
    }
    if (trace && debug)
        consoleAppender->setDetailsLevel(Logger::Trace);
    else if (debug && !trace)
        consoleAppender->setDetailsLevel(Logger::Debug);
    else {
        consoleAppender->setDetailsLevel(Logger::Info);
        consoleAppender->setFormat("%t{dd-HH:mm:ss} [%-7l] %m\n");
    }

    /* check args */
    if (args.size() == 1) {
        logInfo() << "You need to specify pid to put a death watch on.";
        return 1;
    }
    logInfo() << "DeathWatch version" << APP_VERSION << COPYRIGHT;
    bool ok = false;
    pid_t pid = args.at(1).toInt(&ok, 10);
    if (ok) {
        logInfo() << "Death watch is harvesting soul of pid:" << QString::number(pid);
        deathWatch(pid);
    } else {
        QString possiblePidFile = args.at(1);
        if (QFile::exists(possiblePidFile)) { /* case when given param is name of file with pid, not pid itself */
            logInfo() << "Reading file:" << possiblePidFile;
            pid = readFileContents(possiblePidFile).trimmed().toInt(&ok, 10);
            if (ok) {
                logInfo() << "Death watch is harvesting soul of pid:" << QString::number(pid);
                deathWatch(pid);
            } else {
                logError() << "Bad pid file given:" << possiblePidFile << " Aborting.";
                return 1;
            }
        } else {
            logError() << "Bad pid number given:" << args.at(1) << " Aborting.";
            return 1;
        }
    }
    return 0;
}