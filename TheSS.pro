# ServeD natives - TheSS - (c) 2o13-2o15 verknowsys.com
#
# author:
#   Daniel (dmilith) Dettlaff
#

include(src/Common.pro)

TEMPLATE           = subdirs
SUBDIRS            = src/qslog src/core src/hiredis src/quazip src/yajl src/service_spawner
CONFIG += ordered

service_spawner.depends = src/qslog src/core src/hiredis src/quazip src/yajl
