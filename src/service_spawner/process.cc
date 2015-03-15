/**
 *  @author tallica
 *
 *   © 2013 - VerKnowSys
 *
 */

#include "process.h"


void SvdProcess::init(const QString& name) {
    outputFile = getSoftwareDataDir() + "/" + name + DEFAULT_SERVICE_LOGS_DIR + DEFAULT_SERVICE_OUTPUT_FILE;
    setProcessChannelMode(MergedChannels);

    // if (QFile::exists(outputFile)) {
    //     logTrace() << "Rotating log file" << outputFile;
    //     rotateFile(outputFile);
    // }
    // QFile file(outputFile);
    // if (file.size() > 200) {
    //     rotateFile(outputFile);
    // }
    // file.close();
    if (this->redirectOutput)
        setStandardOutputFile(outputFile, QIODevice::Append);
}


SvdProcess::SvdProcess(const QString& name, bool redirectOutput) {
    this->redirectOutput = redirectOutput;
    init(name);
}


SvdProcess::SvdProcess(const QString& name) {
    init(name);
}


void SvdProcess::spawnProcess(const QString& command, const QString& shell, const QStringList& args) {
    if (not command.trimmed().isEmpty()) {
        start(shell, args);
        logTrace() << "Spawning command:" << QString(command);
        write(command.toUtf8());
        write("\nexit\n");
    }
}


void SvdProcess::setupChildProcess() {
    const QString home = getHomeDir();
    const QString user = DEFAULT_USER_NAME;
    // logDebug() << "Setup process environment with home:" << home << "and user:" << user;

    #ifdef __FreeBSD__
        setgroups(0, 0);
    #endif
    setuid(DEFAULT_USER_UID);
    chdir(home.toUtf8());
    setenv("HOME", home.toUtf8(), 1);
    setenv("~", home.toUtf8(), 1);
    setenv("PWD", home.toUtf8(), 1);
    setenv("OLDPWD", home.toUtf8(), 1);
    setenv("USER", user.toUtf8(), 1);
    setenv("LOGNAME", user.toUtf8(), 1);
    setenv("LC_ALL", LOCALE, 1);
    setenv("LANG", LOCALE, 1);
    unsetenv("USERNAME");
    unsetenv("SUDO_USERNAME");
    unsetenv("SUDO_USER");
    unsetenv("SUDO_UID");
    unsetenv("SUDO_GID");
    unsetenv("SUDO_COMMAND");
    unsetenv("MAIL");
}
