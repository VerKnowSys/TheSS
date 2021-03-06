/**
 *  @author tallica, dmilith
 *
 *   © 2013 - VerKnowSys
 *
 */


#include "utils.h"

using namespace QsLogging;

void copyPath(QString src, QString dst) {
    QDir dir(src);
    if (!dir.exists()) {
        logWarn() << "Copy path don't exists:" << src << "Skipping.";
        return;
    }

    foreach (QString d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden)) {
        QString dst_path = dst + QDir::separator() + d;
        dir.mkpath(dst_path);
        copyPath(src + QDir::separator() + d, dst_path);
    }

    foreach (QString f, dir.entryList(QDir::Files | QDir::Hidden)) {
        QFile::copy(src + QDir::separator() + f, dst + QDir::separator() + f);
    }
}


QString tail(const QString& absoluteFileName, int lines, int positionModifier) {
    QFile file(absoluteFileName);
    if (not file.exists())
        return "File empty!";

    file.open(QIODevice::ReadOnly);
    auto mod = 0;
    if (positionModifier == 0) /* if positionModifier not given, then just seek end of a file - standard case */
        mod = file.size() - 1;
    else
        mod = file.size() - 1 - positionModifier;

    file.seek(mod);
    int count = 0;
    while ( (count <= lines) && (file.pos() > 0) ) {
        auto cha = file.read(1);
        file.seek(file.pos() - 2);
        if (cha == "\n")
            count++;
    }
    QString r = file.readAll();
    file.close();
    return r;
}


// QList<int> gatherUserUids() {
//     auto userDirs = QDir(DEFAULT_HOME_DIR + QString("/..")).entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
//     QList<int> dirs;

//     /* filter through invalid directories */
//     Q_FOREACH(QString directory, userDirs) {
//         bool ok;
//         int validUserDir = directory.toInt(&ok, 10); /* valid user directory must be number here */
//         if (ok)
//             dirs << validUserDir;
//         else
//             logTrace() << "Filtering out userDir:" << directory;
//     }
//     return dirs;
// }


void rotateFile(const QString& fileName) {
    if (QFile::exists(fileName)) {
        logDebug() << "Rotating file:" << fileName;
        const QDateTime now = QDateTime::currentDateTime();
        const QString fullPath = QFileInfo(fileName).absoluteFilePath();
        const QStringList splt = fullPath.split("/");
        const QString fileStandaloneName = splt.value(splt.length() - 1);
        const QString parentCwdFolderName = splt.value(splt.length() - 2);
        const QString destLogsDir = getHomeDir() + LOGS_DIR + "/" + parentCwdFolderName;
        const QString destinationFile = destLogsDir + "/" + fileStandaloneName + "." + now.toString("yyyy-MM-dd--hh_mm_ss") + ".gz";

        logTrace() << "Log folder name appendix:" << parentCwdFolderName;
        logDebug() << "Rotate file:" << fileName << ", Logs dir:" << destLogsDir;
        getOrCreateDir(destLogsDir);

        logTrace() << "Destination file:" << destinationFile;

        QFile input(fileName);
        input.open(QIODevice::ReadOnly);
        if (input.size() == 0) {
            logDebug() << "Skipping rotate of empty file:" << fileName;
        } else {
            QByteArray uncompressedData = input.readAll();

            auto zipfile = new QuaZip(destinationFile);
            zipfile->open(QuaZip::mdCreate);
            QuaZipFile file(zipfile);
            file.open(QIODevice::WriteOnly, QuaZipNewInfo(fileName));
            file.write(uncompressedData);

            file.close();
            zipfile->close();
            delete zipfile;

            QFile::remove(fileName);
        }
        input.close();
        performCleanupOfOldLogs();

    } else {
        logDebug() << "No file found to rotate:" << fileName;
    }
}


void performCleanupOfOldLogs() {
    auto rootDirectory = getOrCreateDir(getHomeDir() + LOGS_DIR);
    auto logDirs = QDir(rootDirectory).entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    logDebug() << "Log dirs cleanup invoke in dir with:" << logDirs.length() << "elements, Root Logs DIR:" << rootDirectory;
    Q_FOREACH(QString directory, logDirs) {
        auto cwdFiles = QDir(rootDirectory + "/" + directory).entryList(QDir::Files | QDir::Hidden, QDir::Name);
        auto fileAmount = cwdFiles.length();
        if (fileAmount > AMOUNT_OF_LOG_FILES_TO_KEEP) {
            for (int i = fileAmount; i > AMOUNT_OF_LOG_FILES_TO_KEEP; i--) {
                QString file = rootDirectory + "/" + directory + "/" + cwdFiles.at(fileAmount - i);
                logTrace() << "Removing old log file:" << file;
                QFile::remove(file);
            }
        }

        logTrace() << "Cleanup of dir:" << rootDirectory + "/" + directory << ", with files:" << cwdFiles.length();
    }
}


// bool cpDir(const QString &srcPath, const QString &dstPath) {
//     // removeDir(dstPath);
//     QDir parentDstDir(QFileInfo(dstPath).path());
//     if (!parentDstDir.mkdir(QFileInfo(dstPath).fileName()))
//         return false;

//     QDir srcDir(srcPath);
//     foreach(const QFileInfo &info, srcDir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot)) {
//         QString srcItemPath = srcPath + "/" + info.fileName();
//         QString dstItemPath = dstPath + "/" + info.fileName();
//         if (info.isDir()) {
//             if (!cpDir(srcItemPath, dstItemPath)) {
//                 return false;
//             }
//         } else if (info.isFile()) {
//             if (!QFile::copy(srcItemPath, dstItemPath)) {
//                 return false;
//             }
//         } else {
//             logDebug() << "Unhandled item" << info.filePath() << "in cpDir";
//         }
//     }
//     return true;
// }


bool pidIsAlive(uint pid) {
    if (pid == 0) return false;
    int result = kill(pid, 0);
    if (result == 0)
        return true;
    else
        return false;
}


void unixSignalHandler(int sigNum) {
    if (sigNum == SIGTERM) {
        logWarn() << "Caught SIGTERM signal. Quitting application, leaving services intact.";
        logInfo() << "Forced shutdown";
        Logger::destroyInstance();
        exit(EXIT_SUCCESS);
    }
}


bool setPublicDirPriviledges(const QString& path) {
    char mode[] = "0733"; // no read: rwx-wx-wx equivalent
    int i = strtol(mode, 0, 8);
    auto err = errno;
    if (chmod(path.toUtf8(), i) < 0) {
        logDebug() << "Error chmoding:" << strerror(err);
        return false;
    }
    return true;
}


bool setUserDirPriviledges(const QString& path) {
    char mode[] = "0711"; // no read: rwx--x--x equivalent
    int i = strtol(mode, 0, 8);
    auto err = errno;
    if (chmod(path.toUtf8(), i) < 0) {
        logDebug() << "Error chmoding:" << strerror(err);
        return false;
    }
    return true;
}


bool expect(const QString& inputFileContent, const QString& expectedString) {
    logTrace() << "EXPECT INPUT:\n" << inputFileContent;
    if (expectedString.isEmpty()) {
        logTrace() << "Passed empty expectation";
        return true; /* return pass of expectation for empty string */
    }
    return inputFileContent.trimmed().contains(expectedString.trimmed());
}


const QString getOrCreateDir(const QString& path) {
    if (not QFile::exists(path)) {
        logTrace() << "Creating non existant dir:" << path ;
        QDir().mkpath(path);
    }
    return path;
}


const QString toHMS(uint duration) {
    QString res;
    const int seconds = (int)(duration % 60);
    duration /= 60;
    const int minutes = (int)(duration % 60);
    duration /= 60;
    const int hours = (int)(duration % 24);
    const int days = (int)(duration / 24);

    if ((hours == 0) && (days == 0))
        return res.sprintf("%02d:%02d", minutes, seconds);
    if (days == 0)
        return res.sprintf("%02d:%02d:%02d", hours, minutes, seconds);
    return res.sprintf("%dd%02d:%02d:%02d", days, hours, minutes, seconds);
}


void touch(const QString& fileName) {
    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.close();
    return;
}


bool removeDir(const QString& dirName, bool system_method) {
    if (system_method) {
        QString cmd = "/bin/rm -rf " + dirName;
        system(cmd.toUtf8());
        return true;
    }
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot|QDir::AllDirs|QDir::Files, QDir::DirsFirst)) {
            if (info.isDir())
                result = removeDir(info.absoluteFilePath(), system_method);
            else
                result = QFile::remove(info.absoluteFilePath());

            if (!result)
                return result;
        }
        result = dir.rmdir(dirName);
    } else {
        QFile file(dirName);
        if (file.exists())
            result = file.remove();
        else
            result = false;
    }
    return result;
}


void writeToFile(const QString& fileName, const QString& contents) {
    writeToFile(fileName, contents, false);
}


void writeToFile(const QString& fileName, const QString& contents, bool rotateThisFile) {
    if (rotateThisFile)
        rotateFile(fileName);
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << contents << endl << flush;
    }
    file.close();
}


const QString getHomeDir() {
    if (getuid() == 0)
        return QString(SYSTEM_USERS_DIR);
    else {
        return QString(DEFAULT_HOME_DIR);
    }
}


const QString getSoftwareDataDir() {
    QString dataDir = getHomeDir() + SOFTWARE_DATA_DIR;
    if (!QFile::exists(dataDir)) {
        logTrace() << "Software data dir:" << dataDir << "doesn't exists. Creating it.";
        QDir().mkpath(dataDir);
    }
    return dataDir;
}


const QString getServiceDataDir(const QString& name) {
    return getSoftwareDataDir() + "/" + name;
}


/* author: dmilith */
uint registerFreeTcpPort(uint specificPort) {
    /* 2015-03-15 18:12:08 - dmilith - lookup local hostname and append home domain for it to get main address */
    QHostInfo info = QHostInfo();
    QString defaultDomain = info.localHostName() + DEFAULT_HOME_DOMAIN;
    auto addresses = info.fromName(defaultDomain).addresses();
    if (addresses.empty()) {
        logDebug() << "No host address available.";
        return -1;
    }
    QString addr = addresses.first().toString();
    QTime midnight(0, 0, 0);
    qsrand(midnight.msecsTo(QTime::currentTime())); // accuracy is in ms.. so let's hack it a bit
    usleep(1000); // this practically means no chance to generate same port when generating multiple ports at once
    uint port = 0, rand = (qrand() % 40000);
    if (specificPort == 0) {
        port = 10000 + rand;
    } else
        port = specificPort;

    if (not QNetworkInterface::allAddresses().contains(QHostAddress(addr))) {
        logDebug() << "Empty host address?";
        return -1;
    //     if (addr != QString(DEFAULT_LOCAL_ADDRESS)) {
    //         logError() << "Requested binding to unbound address:" << addr << "Doing fallback to wildcard address";
    //         addr = DEFAULT_WILDCARD_ADDRESS;
    //     } else {
    //         logDebug() << "Requested binding home domain:" << addr;
    //         addr = QHostInfo::localHostName() + DEFAULT_HOME_DOMAIN;
    //     }
    }

    logTrace() << "Trying address:" << addr << "on port:" << port << "Randseed:" << rand;
    auto tcpServer = new QTcpServer();
    tcpServer->listen(QHostAddress(addr), port);
    if (not tcpServer->isListening()) {
        logDebug() << "Taken port on TCP:" << port;
        delete tcpServer;
        return registerFreeTcpPort(10000 + rand);
    } else
        tcpServer->close();
    delete tcpServer;

    return port;
}


/* author: dmilith */
uint registerFreeUdpPort(uint specificPort) {
    QTime midnight(0, 0, 0);
    qsrand(midnight.msecsTo(QTime::currentTime())); // accuracy is in ms.. so let's hack it a bit
    usleep(10000); // this practically means no chance to generate same port when generating multiple ports at once
    uint port = 0, rand = (qrand() % 40000);
    if (specificPort == 0) {
        port = 10000 + rand;
    } else
        port = specificPort;

    int sockfd;
    struct sockaddr_in servaddr;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    if (::bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) {
        close(sockfd);
        return registerFreeUdpPort(10000 + rand);
    }
    close(sockfd);
    return port;
}


// /*
//  *  Parse string contents to Json value.
//  */
// Json::Value* parseJSON(const QString& filename) {
//     Json::Reader reader; /* parse json file */
//     auto root = new Json::Value();

//     auto parsedSuccess = reader.parse(readFileContents(filename).toUtf8().data(), *root, false);
//     if (!parsedSuccess) {
//         logError() << "JSON Parse Failure of file:" << filename;
//         root = NULL; /* this indicates that json parser failed to get data from igniter */
//         return root;
//     }

//     /* checking schema rules for Igniter format */
//     QStringList listOfObjects, objectStringFields, objectIntFields, listOfArrays, listOfStrings, listOfBools, listOfInts; //, schedulerFields;
//     listOfObjects << "install" << "configure" << "start" << "afterStart" << "afterStart" << "stop" << "afterStop" << "reload" << "validate" << "babySitter";
//     objectStringFields << "commands" << "expectOutput";
//     objectIntFields << "expectOutputTimeout";
//     // schedulerFields << "cronEntry" << "commands";

//     listOfArrays << "dependencies" << "schedulers" << "watchHttpAddresses" << "domains" << "standaloneDependencies";

//     listOfInts << "staticPort" << "portsPool" << "minimumRequiredDiskSpace" << "notificationLevel";
//     listOfStrings << "softwareName" << "repository" << "parentService";
//     listOfBools << "autoStart" << "watchPort" << "watchUdpPort" << "alwaysOn" << "resolveDomain" << "webApp";

//     /* objects */
//     for (int indx = 0; indx < listOfObjects.length(); indx++) {
//         auto valueObject = (*root)[listOfObjects.at(indx).toStdString()]; /* NOTE: Null is allowed, element may not exists */
//         if (valueObject.isInt() or valueObject.isBool() or valueObject.isNumeric() or valueObject.isString()) {
//             logError() << "JSON Type: Object - Failure in file:" << filename << "In object:" << listOfObjects.at(indx);
//             root = NULL;
//             return root;
//         }

//         /* string elements */
//         Q_FOREACH(QString value, objectStringFields) {
//             auto obj = valueObject.get(value.toStdString(), "");
//             if (not obj.isString()) { /* NOTE: Null is allowed, element may not exists */
//                 logError() << "JSON Type: Object - Failed in file:" << filename << "In object:" << listOfObjects.at(indx) << ", field:" << value;
//                 root = NULL;
//                 return root;
//             }
//         }

//         /* int elements */
//         Q_FOREACH(QString value, objectIntFields) {
//             auto obj = valueObject.get(value.toStdString(), 0); /* it's zero cause it's timeout field */
//             if (not obj.isNumeric()) { /* NOTE: Null is allowed, element may not exists */
//                 logError() << "JSON Type: Object - Failed in file:" << filename << "In object:" << listOfObjects.at(indx) << ", field:" << value;
//                 root = NULL;
//                 return root;
//             }
//         }
//     }

//     /* arrays */
//     for (int indx = 0; indx < listOfArrays.length(); indx++) {
//         auto value = (*root)[listOfArrays.at(indx).toStdString()]; /* NOTE: Null is allowed, element might not exists */
//         if (not value.isArray() or value.isString() or value.isInt() or value.isBool() or value.isNumeric()) {
//             logError() << "JSON Type: Array - Failed in file:" << filename << "In field:" << listOfArrays.at(indx);
//             root = NULL;
//             return root;
//         }

//         // /* check scheduler fields */
//         // Q_FOREACH(QString val, schedulerFields) {
//         //     if (value.isValidIndex(indx)) {
//         //         auto entry = (*root)[listOfArrays.at(indx).toStdString()][indx];
//         //         if (not entry.empty() and not entry.isNull() and entry.isArray()) {
//         //             auto innerEntry = entry[val.toStdString()];
//         //             if (not innerEntry.isString()) {
//         //                 logError() << "JSON Type: Array - Failed in file:" << filename << "In field:" << listOfArrays.at(indx) << " index:" << val;
//         //                 root = NULL;
//         //                 return root;
//         //             }
//         //         }
//         //     }
//         // }
//     }

//     /* ints */
//     for (int indx = 0; indx < listOfInts.length(); indx++) {
//         auto value = root->get(listOfInts.at(indx).toStdString(), -1); /* default value: -1 is required */
//         if (not value.isNumeric()) {
//             logError() << "JSON Type: Numeric - Failed in file:" << filename << "In field:" << listOfInts.at(indx);
//             root = NULL;
//             return root;
//         }
//     }

//     /* strings */
//     for (int indx = 0; indx < listOfStrings.length(); indx++) {
//         auto value = root->get(listOfStrings.at(indx).toStdString(), ""); /* default value: "" for Strings */
//         if (not value.isString()) {
//             logError() << "JSON Type: String - Failed in file:" << filename << "In field:" << listOfStrings.at(indx);
//             root = NULL;
//             return root;
//         }
//     }

//     /* bools */
//     for (int indx = 0; indx < listOfBools.length(); indx++) {
//         auto value = root->get(listOfBools.at(indx).toStdString(), false); /* default value: false is required */
//         if (not value.isBool()) {
//             logError() << "JSON Type: Boolean - Failed in file:" << filename << "In field:" << listOfBools.at(indx);
//             root = NULL;
//             return root;
//         }
//     }

//     return root; /* return user side igniter first by default */
// }


QMap<QString, long> getDiskFree(const QString& path) {
    QMap<QString, long> results;

    #ifdef __linux__
        char tmp[1024];
        struct statfs sb;

        statfs(path.toUtf8().data(), &sb);
        long mibSizeCount = sb.f_bfree * sb.f_bsize / 1024 / 1024;
        if (path.startsWith("/") and not path.endsWith("dev") and not path.endsWith("net")) {
            results[path.toUtf8()] = mibSizeCount;
        }
    #else
        struct statfs *mntbuf;
        long mntsize = getmntinfo(&mntbuf, 0);
        statfs(path.toUtf8(), mntbuf);

        for (int index = 0; index < mntsize; index++) {
            long mibSizeCount = mntbuf[index].f_bfree * mntbuf[index].f_bsize / 1024 / 1024;
            QString value = QString(mntbuf[index].f_mntonname);

            if (value.startsWith("/") and not value.endsWith("dev")
                #ifdef __APPLE__
                    and not value.endsWith("home")
                #endif
                and not value.endsWith("net")) {
                    results[value.toUtf8()] = mibSizeCount;
            }
        }
    #endif

    // qDebug() << "FREE disk map: ";
    // Q_FOREACH(QString key, results.keys()) {
    //     qDebug() << key << " -> " << results[key];
    // }
    return results;
}
