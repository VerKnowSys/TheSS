
/* include tokens definitions */
#include "tokens.h"

#ifndef __GLOBALS__
#define __GLOBALS__

    /* global logger definitions */
    #define logTrace QLOG_TRACE
    #define logDebug QLOG_DEBUG
    #define logInfo  QLOG_INFO
    #define logWarn  QLOG_WARN
    #define logError QLOG_ERROR
    #define logFatal QLOG_FATAL

    /* global constants */
    #define APP_VERSION "0.100.5"
    #define COPYRIGHT "2oo8-2o15 verknowsys.com"

    /* default BSD case: */
    #define DEFAULT_HTTP_PORT 80
    #define DEFAULT_SSL_PORT 443
    #define DEFAULT_SYSTEM_CONSOLE "/dev/console"

    #define DEFAULT_BEHAVIORS_DIR "basesystem/behaviors"
    #define DEFAULT_BEHAVIORS_RAW "/output_raw_processes.raw.input"

    #define DEFAULT_STRING_CODEC "utf8"
    #define LOCALE "en_GB.UTF-8"
    #define DEFAULT_TEST_POSTFIX "-TEST"

    #define NOTIFICATIONS_OPTIONS_BAR_HEIGHT 2
    #define NOTIFICATIONS_LAST_SHOWN 11 /* amount of notification before moving to history */
    #define NOTIFICATIONS_HISTORY_KEEP_UPTO 100 /* how many files to keep in history */

    #ifdef __APPLE__
        #define CLOCK_REALTIME REALTIME_CLOCK
        #define DEFAULT_DEATH_WATCHER_PAUSE 500000 /* .5 seconds by default for developer machine */
    #else
        #define DEFAULT_DEATH_WATCHER_PAUSE 2000000 /* 2 seconds by default for production */
    #endif

    #define DEFAULT_USER_UID 501
    #define DEFAULT_ROOT_API_PORT 10
    #define DEFAULT_USER_API_PORT 12
    #define AMOUNT_OF_LOG_FILES_TO_KEEP 125

    #define DEFAULT_VPN_INTERFACE_SETUP_COMMAND "kldunload if_tap ; kldload if_tap && ifconfig tap0 create" /* &&  ifconfig tap1 create && ifconfig tap0 10.10.0.1 netmask 255.255.0.0" */
    #define DEFAULT_VPN_INTERFACE_SHUTDOWN_COMMAND "ifconfig tap0 destroy"

    #define DEFAULT_HOME_DOMAIN ".home"
    #define NULL_FILE "/dev/null"
    #define SOFTWARE_DIR "/Software"
    #define LOGS_DIR "/Logs"
    #define DEFAULT_APP_PROXY_FILE "/.proxy.conf" /* addition to proxy from app repo */
    #define DEFAULT_PUBLIC_DIR "/Public"
    #define DEFAULT_HOME_DIR "/User"
    #define DEFAULT_USER_NAME "worker"
    #define SOFTWARE_DATA_DIR "/SoftwareData"
    #define SYSTEM_USERS_DIR "/SystemUsers"
    #define DEFAULT_GIT_REPOSITORY_DIR "/Repos"
    #define DEFAULT_SOFTWARE_TEMPLATE_EXT ".json"
    #define DEFAULT_SERVICE_INSTALLED_EXT ".installed"
    #define DEFAULT_SERVICE_DISABLED_POSTFIX ".disabled"
    #define DEFAULT_SOFTWARE_TEMPLATE (SYSTEM_USERS_DIR "/Igniters/Default")
    #define DEFAULT_SOFTWARE_TEMPLATES_DIR (SYSTEM_USERS_DIR "/Igniters/Services")
    #define DEFAULT_USER_IGNITERS_DIR "/Igniters/Services"
    #define DEFAULT_LOCAL_ADDRESS "127.0.0.1"
    #define DEFAULT_WILDCARD_ADDRESS "0.0.0.0"
    #define DEFAULT_SYSTEM_DOMAIN "localhost"

    #define DEFAULT_SERVICE_SOCKET_FILE "/service.sock"
    #define DEFAULT_SERVICE_PID_FILE "/service.pid"
    #define DEFAULT_SERVICE_ENV_FILE "/service.env"
    #define DEFAULT_SERVICE_LOG_FILE "/service.log"
    #define DEFAULT_SERVICE_CONF_FILE "/service.conf"
    #define DEFAULT_SERVICE_DEPENDENCIES_FILE "/.dependencies"

    /* triggers */
    #define INSTALL_TRIGGER_FILE "/.install"
    #define CONFIGURE_TRIGGER_FILE "/.configure"
    #define RECONFIGURE_TRIGGER_FILE "/.reconfigure"
    #define RECONFIGURE_WITHOUT_DEPS_TRIGGER_FILE "/.reconfigureWithoutDeps"
    #define START_TRIGGER_FILE "/.start"
    #define START_WITHOUT_DEPS_TRIGGER_FILE "/.startWithoutDeps"
    #define STOP_TRIGGER_FILE "/.stop"
    #define STOP_WITHOUT_DEPS_TRIGGER_FILE "/.stopWithoutDeps"
    #define AFTERSTART_TRIGGER_FILE "/.afterStart"
    #define AFTERSTOP_TRIGGER_FILE "/.afterStop"
    #define RESTART_TRIGGER_FILE "/.restart"
    #define RESTART_WITHOUT_DEPS_TRIGGER_FILE "/.restartWithoutDeps"
    #define RELOAD_TRIGGER_FILE "/.reload"
    #define VALIDATE_TRIGGER_FILE "/.validate"
    #define AUTOSTART_TRIGGER_FILE "/.autostart"
    #define NOTIFICATION_TRIGGER_FILE "/.sendNotifications"

    /* states */
    #define DEFAULT_SERVICE_RUNNING_FILE "/.running"
    #define DEFAULT_SERVICE_INSTALLING_FILE "/.installing"
    #define DEFAULT_SERVICE_AFTERSTOPPING_FILE "/.afterStopping"
    #define DEFAULT_SERVICE_AFTERSTARTING_FILE "/.afterStarting"
    #define DEFAULT_SERVICE_CONFIGURING_FILE "/.configuring"
    #define DEFAULT_SERVICE_RELOADING_FILE "/.reloading"
    #define DEFAULT_SERVICE_VALIDATING_FILE "/.validating"
    #define DEFAULT_SERVICE_DEPLOYING_FILE "/.deploying"
    #define DEFAULT_SERVICE_CRON_WORKING_FILE "/.cronWorking"

    #define DEFAULT_SERVICE_CONFIGURED_FILE "/.configured"
    #define DEFAULT_SERVICE_VALIDATION_FAILURE_FILE "/.validationFailure"
    #define NOTIFICATIONS_HISTORY_DATA_DIR "/.notifications-history"
    #ifdef QT_DEBUG
        #define NOTIFICATIONS_CHANNEL_NAME "\%23dev-status"
    #else
        #define NOTIFICATIONS_CHANNEL_NAME "\%23status"
    #endif

    #define DEFAULT_STANDALONE_DIR "/standalone"
    #define DEFAULT_RELEASES_DIR "/releases"
    #define DEFAULT_SHARED_DIR "/shared"
    #define DEFAULT_SERVICE_LOGS_DIR "/.logs"
    #define DEFAULT_SERVICE_PORTS_DIR "/.ports"
    #define DEFAULT_SERVICE_ENVS_DIR "/.envs"
    #define DEFAULT_SERVICE_PIDS_DIR "/.pids"
    #define DEFAULT_SERVICE_CONFS_DIR "/.confs"
    #define DEFAULT_SERVICE_SSLS_DIR "/.ssls"

    #define WEB_APP_PUBLIC_EXT ".web-app"

    #define DEFAULT_CA_CERT_ROOT_SITE "http://curl.haxx.se/ca"
    #define DEFAULT_SSL_CA_FILE "/etc/ssl/cert.pem"


    /* global definitions */
    #define DATA_COLLECTOR_SOCKET_FILE "/data-collector.sock"
    #define NOTIFICATIONS_DATA_DIR "/.notifications"
    #define NOTIFICATION_MANAGER_LOG_FILE "/.notifications-manager.log"
    #define DEFAULT_SERVICE_PORT_NUMBER "0"
    #define DEFAULT_SERVICE_OUTPUT_FILE "/service.error.log"
    #define DEFAULT_SS_SHUTDOWN_FILE ".shutdown"
    #define DEFAULT_SS_SHUTDOWN_HOOK_FILE ("/" DEFAULT_SS_SHUTDOWN_FILE)
    #define DEFAULT_SS_GRACEFUL_SHUTDOWN_FILE ".shutdownGracefully"
    #define DEFAULT_SS_GRACEFUL_SHUTDOWN_HOOK_FILE ("/" DEFAULT_SS_GRACEFUL_SHUTDOWN_FILE)
    #define DEFAULT_SS_PROCESS_DATA_COLLECTION_HOOK_FILE "/.data-collector"
    #define DEFAULT_SS_LOG_FILE "/var/log/messages"
    #define CRON_DELIMITER "?!"
    #define DEFAULT_CRON_ENTRY "0 4 * * *" /* exactly 4am */

    #define DEFAULT_PROCESS_TIMEOUT 30000 // 30s
    #define DEFAULT_INSTALL_TIMEOUT 1800000 // 30m

    #define ONE_SECOND_OF_DELAY 1000000 // 1 second
    #define DEFAULT_CRON_CHECK_DELAY 60000000 // 60s
    #define DEFAULT_SERVICE_PAUSE_INTERVAL 5000000 // 5 seconds
    #define BABYSITTER_TIMEOUT_INTERVAL 15000000 // 15 seconds
    #define COLLECTOR_TIMEOUT_INTERVAL 1000000 // 1 second
    #define DEFAULT_GATHERING_PAUSE_MICROSECONDS 500000 // half a second
    #define DEFAULT_COUNT_OF_ROUNDS_OF_GATHERING 7200 // waiting half a second, hence 7200 is 60 minutes of gathering
    #define DEFAULT_USER_GROUP 0
    #define SOCK_DATA_PACKET_SIZE 32
    #define BUFFER_SIZE 256
    #define LOCK_FILE_OCCUPIED_ERROR 200
    #define CANNOT_LOCK_ERROR 201
    #define POPEN_ERROR 202
    #define CLASSPATH_DIR_MISSING_ERROR 203
    #define NOROOT_PRIVLEGES_ERROR 204
    #define SETUID_ERROR 205
    #define SETGID_ERROR 206
    #define FORK_ERROR 207
    #define EXEC_ERROR 208
    #define NO_UID_GIVEN_ERROR 209
    #define DIAGNOSTIC_LOG_ERROR 210
    #define AMBIGOUS_ENTRY_ERROR 211
    #define ROOT_UID_ERROR 212
    #define INSTALLATION_MISSING_ERROR 213
    #define STDIN_GETATTR_ERROR 214
    #define STDIN_SETATTR_ERROR 215
    #define STDIN_READ_ERROR 216
    #define STDOUT_WRITE_ERROR 217
    #define TERM_GETSIZE_ERROR 218
    #define TERM_GETATTR_ERROR 219
    #define PTY_WRITE_ERROR 220
    #define PTY_FORK_ERROR 221
    #define GETOPT_ERROR 222
    #define JSON_PARSE_ERROR 223
    #define NO_SUCH_FILE_ERROR 224
    #define NO_DEFAULT_IGNITERS_FOUND_ERROR 225
    #define JSON_FORMAT_EXCEPTION_ERROR 225
    #define OTHER_EXCEPTION_ERROR 226
    #define NO_NETWORK_ERROR 227

#endif
