# ServeD natives - Coreginx Helper - (c) 2o13-2o14 verknowsys.com
#
# author:
#   Daniel (dmilith) Dettlaff
#

include(../Common.pro)

QT += network
HEADERS   += ../notifications/notifications.h \
             ../service_spawner/process.h \
             ../death_watch/death_watch.h \
             ../service_spawner/file_events_manager.h \
             ../notifications/notifications.h \
             ../service_spawner/cron_entry.h \
             ../service_spawner/logger.h \
             ../service_spawner/service_config.h \
             ../service_spawner/service.h \
             public_watcher.h

SOURCES   += ../service_spawner/process.cc \
             ../death_watch/death_watch.cc \
             ../notifications/notifications.cc \
             ../service_spawner/file_events_manager.cc \
             ../deployer/webapp_types.cc \
             ../service_spawner/logger.cc \
             ../service_spawner/cron_entry.cc \
             ../service_spawner/service_config.cc \
             ../service_spawner/service.cc \
             public_watcher.cc \
             coreginx_helper.cc

LIBS      += ../liblogger.a ../libcore.a ../libyajl.a ../libhiredis.a ../libquazip.a -lz
TARGET    = ../../bin/svdcoreginx_helper
