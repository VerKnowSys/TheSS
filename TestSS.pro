# ServeD natives - TheSS - (c) 2o13-2o14 verknowsys.com
#
# author:
#   Daniel (dmilith) Dettlaff
#

include(src/Common.pro)

TEMPLATE           = subdirs
SUBDIRS            = src/test

test.depends = src/yajl src/cutelogger src/quazip src/notifications src/service_spawner
