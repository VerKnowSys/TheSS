# ServeD natives - Process DeathWatch - (c) 2o13-2o15 verknowsys.com
#
# author:
#   Daniel (dmilith) Dettlaff
#

include(src/Common.pro)

TEMPLATE           = subdirs
SUBDIRS            = src/core src/death_watcher

notifications.depends = src/qslog src/core
