# ServeD natives - Control Panel
#
# author:
#   Daniel (dmilith) Dettlaff
#

include(../Common.pro)

QT += network
HEADERS   += ../service_spawner/utils.h \
             ../cutelogger/Logger.h \
             ../cutelogger/ConsoleAppender.h \
             ../cutelogger/FileAppender.h \
             ../service_spawner/logger.h \
             ../service_spawner/process.h \
             panel.h

SOURCES   += ../service_spawner/utils.cc \
             ../service_spawner/logger.cc \
             ../service_spawner/process.cc \
             main.cc \
             panel.cc \
             gui.cc \
             scroll_list.cc \
             service.cc

mac {
      LIBS      += ../liblogger.a ../libquazip.a ../libjsoncpp.a -lz -lncurses
} else {
      LIBS      += ../liblogger.a ../libquazip.a ../libjsoncpp.a -lz -lncursesw
}
TARGET    = ../../bin/svdpanel
