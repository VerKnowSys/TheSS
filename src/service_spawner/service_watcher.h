/**
 *  @author tallica
 *
 *   © 2013 - VerKnowSys
 *
 */

#ifndef __SERVICE_WATCHER_H__
#define __SERVICE_WATCHER_H__

#include "file_events_manager.h"
#include "service_config.h"
#include "service.h"
#include "api.h"

#include <QObject>
#include <QFile>
#include <QEventLoop>


class SvdHookTriggerFile: public QFile {

    public:
        explicit SvdHookTriggerFile(const QString& name) : QFile(name) {}
        bool touch();

};


class SvdHookIndicatorFile: public QFile {

    public:
        explicit SvdHookIndicatorFile(const QString& name) : QFile(name) {}

};


class SvdHookTriggerFiles {

    public:
        SvdHookTriggerFiles(const QString& path);
        ~SvdHookTriggerFiles();

        SvdHookTriggerFile *install, *configure, *reConfigure, *reConfigureWithoutDeps, *start, *startWithoutDeps, *afterStart, *stop, *stopWithoutDeps, *afterStop, *restart, *restartWithoutDeps, *reload, *validate;

};


class SvdHookIndicatorFiles {

    public:
        SvdHookIndicatorFiles(const QString& path);
        ~SvdHookIndicatorFiles();

        SvdHookIndicatorFile *running, *autostart, *installing, *afterStarting, *afterStopping, *configuring, *reloading, *validating;

};


class SvdServiceWatcher: public QObject {
    Q_OBJECT

    public:
        QString name();
        SvdAPI* api();
        SvdServiceWatcher(const QString& name, SvdAPI* api);
        ~SvdServiceWatcher();

    private:
        SvdFileEventsManager *fileEvents = nullptr;
        SvdHookTriggerFiles *triggerFiles = nullptr;
        SvdHookIndicatorFiles *indicatorFiles = nullptr;
        QString dataDir, appName;
        SvdService *service = nullptr;
        SvdAPI *svdapi = nullptr;

        void cleanupTriggerHookFiles();

    signals:
        void installService();
        void configureService();
        void reConfigureService();
        void reConfigureWithoutDepsService();
        void validateService();
        void startService();
        void startWithoutDepsService();
        void afterStartService();
        void stopService();
        void stopWithoutDepsService();
        void afterStopService();
        void restartService();
        void restartWithoutDepsService();
        void reloadService();
        void destroyService();

    public slots:
        void shutdownSlot();
        void dirChangedSlot(const QString& dir);
        void fileChangedSlot(const QString& file);

};


#endif
