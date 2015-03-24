/**
 *  @author dmilith
 *
 *   © 2014 - VerKnowSys
 *
 */

#ifndef __WEBPANEL_H__
#define __WEBPANEL_H__

// #include "../core/utils.h"
#include <QtCore>
#include <QProcess>
#include <QtWebSockets/QtWebSockets>
#include <iostream>
#include <unistd.h>

#include <errno.h>
#include <sys/stat.h>
#include <sys/user.h>
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <libgen.h>
#include <getopt.h>
#include <termios.h>
#include <signal.h>
#include <dirent.h>

#ifdef __FreeBSD__
    #include <kvm.h>
    #include <sys/capability.h>
    #include <libprocstat.h>
    #include <libutil.h>
    #include <paths.h>
    using namespace std;
#endif

#include "../globals/globals.h"
#include "../core/utils-core.h"
#include "../core/logger-core.h"


QString getJSONProcessesList(uint uid);


QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)


class SvdAPI: public QObject {

    Q_OBJECT

    public:
        explicit SvdAPI(const QString& host, quint16 port, QObject *parent = Q_NULLPTR);
        ~SvdAPI();

    Q_SIGNALS:
        void closed();

    private Q_SLOTS:
        void onNewConnection();
        void processTextMessage(QString message);
        void processBinaryMessage(QByteArray message);
        void socketDisconnected();

    public:
        QString getServiceStatus(QString name);
        QString packJsonRpcResponse(QString input);
        void executeCommand(QString command);
        void sendUserStatsToAllClients();
        void sendListServices();
        void sendCustomMessageToAllClients(QString name, QString content);
        void sendMessageToAllClients(QString name, QString reason, QString hookName);

        void startService(QString name, QString reason = "");
        void installService(QString name, QString reason = "");
        void configureService(QString name, QString reason = "");
        void reConfigureService(QString name, QString reason = "");
        void reConfigureWithoutDepsService(QString name, QString reason = "");
        void validateService(QString name, QString reason = "");
        void startWithoutDepsService(QString name, QString reason = "");
        void afterStartService(QString name, QString reason = "");
        void stopService(QString name, QString reason = "");
        void stopWithoutDepsService(QString name, QString reason = "");
        void afterStopService(QString name, QString reason = "");
        void restartService(QString name, QString reason = "");
        void restartWithoutDepsService(QString name, QString reason = "");
        void reloadService(QString name, QString reason = "");
        void destroyService(QString name, QString reason = "");


    private:
        QWebSocketServer* m_pWebSocketServer;
        QList<QWebSocket*> m_clients;

};


#endif
