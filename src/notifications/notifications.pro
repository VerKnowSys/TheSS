# ServeD natives - low level notification mechanism
#
# author:
#   Daniel (dmilith) Dettlaff
#

include(../Common.pro)

QT += network

HEADERS   += notifications.h ../service_spawner/process.h ../core/utils-core.h
SOURCES   += notifier.cc notifications.cc ../core/utils.cc ../core/utils-core.cc ../service_spawner/process.cc

LIBS += -lz ../liblogger.a ../libyajl.a ../libhiredis.a ../libquazip.a

TARGET = ../../bin/svdnotify
