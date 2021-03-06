/**
 *  @author tallica, dmilith
 *
 *   © 2013-2014 - VerKnowSys
 *
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#include "../qslog/QsLog.h"
#include "../globals/globals.h"
#include "../service_spawner/process.h"
#include "../quazip/quazip.h"
#include "../quazip/quazipfile.h"
#include "utils-core.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>

#include <QtCore>
#include <QTime>
#include <QTextCodec>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QNetworkInterface>

#ifdef __linux__
    #include <sys/statfs.h>
    #include <sys/types.h>
#else
    #include <sys/ucred.h>
#endif

#include <sys/param.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstab.h>
#include <unistd.h>

using namespace std;

#ifdef __FreeBSD__
    #define DEFAULT_VPN_SPAWN_SHELL "/Software/Zsh/exports/zsh"
#endif

// QList<int> gatherUserUids();

bool expect(const QString& inputFileContent, const QString& expectedString);
bool removeDir(const QString& dirName, bool system_method = false);
void copyPath(QString src, QString dst);
bool setPublicDirPriviledges(const QString& path);
bool setUserDirPriviledges(const QString& path);
bool pidIsAlive(uint pid);

void performCleanupOfOldLogs();
void touch(const QString& fileName);
void writeToFile(const QString& fileName, const QString& contents);
void writeToFile(const QString& fileName, const QString& contents, bool rotateFile);
void rotateFile(const QString& fileName);
void unixSignalHandler(int sigNum);
QMap<QString, long> getDiskFree(const QString& path = DEFAULT_HOME_DIR); /* in MiB */

uint registerFreeTcpPort(uint specificPort = 0);
uint registerFreeUdpPort(uint specificPort = 0);
// Json::Value* parseJSON(const QString& filename);

QString tail(const QString& pathToFile, int lines, int positionModifier = 0);
const QString toHMS(uint duration);
const QString getOrCreateDir(const QString& path);
const QString getHomeDir();
const QString getSoftwareDataDir();
const QString getServiceDataDir(const QString& name);

#endif
