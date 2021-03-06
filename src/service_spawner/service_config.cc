/**
 *  @author dmilith
 *
 *   © 2013-2014 - VerKnowSys
 *
 */

#include "service_config.h"


SvdScheduler::SvdScheduler(const QString& initialCronEntry, const QString& initialCommands) {
    cronEntry = initialCronEntry;
    commands = initialCommands;
    auto hash = new QCryptographicHash(QCryptographicHash::Sha1);
    QString content = cronEntry + commands;
    hash->addData(content.toUtf8(), content.length());
    sha = hash->result().toHex();
    delete hash;
}


QString SvdServiceConfig::errors() {
    return QString(this->errbuf);
}


QStringList SvdServiceConfig::getArray(const char* element) {
    return JSONAPI::getArray(nodeDefault_, nodeRoot_, element);
}


QString SvdServiceConfig::getString(const char* element) {
    return JSONAPI::getString(nodeDefault_, nodeRoot_, element);
}


bool SvdServiceConfig::getBoolean(const char* element) {
    return JSONAPI::getBoolean(nodeDefault_, nodeRoot_, element);
}


long long SvdServiceConfig::getInteger(const char* element) {
    return JSONAPI::getInteger(nodeDefault_, nodeRoot_, element);
}


double SvdServiceConfig::getDouble(const char* element) {
    return JSONAPI::getDouble(nodeDefault_, nodeRoot_, element);
}


bool SvdServiceConfig::valid() {
    /* if any errors found in buffer => validation failed */
    if (not errors().isEmpty())
        return false;

    /* validate shell operations */
    if ((not install) or
        (not configure) or
        (not start) or
        (not afterStart) or
        (not stop) or
        (not afterStop) or
        (not reload) or
        (not validate) or
        (not babySitter))
            return false;

    /* validate schedulers */
    if (not schedulers.isEmpty()) {
        Q_FOREACH(auto sched, schedulers) {
            if (sched == NULL)
                return false;
        }
    }

    /* validate some more requirements */
    if (name.isEmpty())
        return false;

    if (nodeDefault_ == NULL)
        return false;
    if (nodeRoot_ == NULL)
        return false;

    return true;
}


SvdServiceConfig::SvdServiceConfig() { /* Load default values */
    name = "Default"; // must be declared first
    // uid = getuid();

    const auto defaults = loadDefaultIgniter();
    if (defaults.isEmpty()) {
        QString msg = "Default igniter must be always valid and not empty. Cannot continue.";
        notification(msg, FATAL);
        return;
    }
    nodeDefault_ = yajl_tree_parse(defaults.toUtf8(), errbuf, sizeof(errbuf));
    nodeRoot_ = yajl_tree_parse(defaults.toUtf8(), errbuf, sizeof(errbuf));
    if (QString(errbuf).length() > 0) {
        logError() << "ERR:" << errbuf;
        return;
    }
    if (not nodeDefault_) {
        QString msg = "Failed to parse default igniter which is mandatory. Cannot continue";
        notification(msg, FATAL);
        logFatal() << msg;
        return;
    }

    auto spt = QString(APP_VERSION).split("."); /* XXX: provide double value */
    auto appVerVal = (spt.at(0) + "." + spt.at(1)).toDouble();
    formatVersion = getDouble("formatVersion");
    logDebug() << "IGN-VER:" << QString::number(formatVersion) << "APP-VER:" << QString::number(appVerVal) << "difference:" << QString::number(appVerVal - formatVersion);
    if ((appVerVal - formatVersion) > 0.1) {
        logWarn() << "Outdated igniter format detected. Please update your igniters!";
    }
    shell = getString("shell");
    softwareName = getString("softwareName");
    autoStart = getBoolean("autoStart");
    notificationLevel = getInteger("notificationLevel");
    webApp = getBoolean("webApp");
    watchPort = getBoolean("watchPort");
    watchSocket = getBoolean("watchSocket");
    watchUdpPort = getBoolean("watchUdpPort");
    alwaysOn = getBoolean("alwaysOn");
    resolveDomain = getBoolean("resolveDomain");
    staticPort = getInteger("staticPort");
    minimumRequiredDiskSpace = getInteger("minimumRequiredDiskSpace");
    portsPool = getInteger("portsPool");
    repository = getString("repository");
    parentService = getString("parentService");
    configureOrder = getInteger("configureOrder");
    startOrder = getInteger("startOrder");

    dependencies = getArray("dependencies");

    standaloneDependencies = getArray("standaloneDeps");
    domains = getArray("domains");
    watchHttpAddresses = getArray("watchHttp");

    /* laod service hooks */
    install = new SvdShellOperations(getString("install/commands"), getString("install/expectOutput"));
    configure = new SvdShellOperations(getString("configure/commands"), getString("configure/expectOutput"));
    start = new SvdShellOperations(getString("start/commands"), getString("start/expectOutput"));
    afterStart = new SvdShellOperations(getString("afterStart/commands"), getString("afterStart/expectOutput"));
    stop = new SvdShellOperations(getString("stop/commands"), getString("stop/expectOutput"));
    afterStop = new SvdShellOperations(getString("afterStop/commands"), getString("afterStop/expectOutput"));
    reload = new SvdShellOperations(getString("reload/commands"), getString("reload/expectOutput"));
    validate = new SvdShellOperations(getString("validate/commands"), getString("validate/expectOutput"));
    babySitter = new SvdShellOperations(getString("babySitter/commands"), getString("babySitter/expectOutput"));

    //yajl_tree_free(node_);
    #ifdef QT_DEBUG
        prettyPrint();
    #endif
}


void SvdServiceConfig::prettyPrint() {
    logTrace() << "Service |                     name:" << name;
    logTrace() << "        |            formatVersion:" << formatVersion;
    logTrace() << "        |                    shell:" << shell;
    logTrace() << "        |             softwareName:" << softwareName;
    logTrace() << "        |                autoStart:" << autoStart;
    logTrace() << "        |        notificationLevel:" << notificationLevel;
    logTrace() << "        |                   webApp:" << webApp;
    logTrace() << "        |                watchPort:" << watchPort;
    logTrace() << "        |                watchSocket:" << watchSocket;
    logTrace() << "        |             watchUdpPort:" << watchUdpPort;
    logTrace() << "        |                 alwaysOn:" << alwaysOn;
    logTrace() << "        |            resolveDomain:" << resolveDomain;
    logTrace() << "        |               staticPort:" << staticPort;
    logTrace() << "        | minimumRequiredDiskSpace:" << minimumRequiredDiskSpace;
    logTrace() << "        |                portsPool:" << portsPool;
    logTrace() << "        |               repository:" << repository;
    logTrace() << "        |            parentService:" << parentService;
    logTrace() << "        |           configureOrder:" << configureOrder;
    logTrace() << "        |               startOrder:" << startOrder;
    logTrace() << "        |             dependencies:" << dependencies;
    logTrace() << "        |   standaloneDependencies:" << standaloneDependencies;
    logTrace() << "        |                  domains:" << domains;
    // logTrace() << "        |               schedulers:" << schedulers;
    logTrace() << "        |       watchHttpAddresses:" << watchHttpAddresses;
    logTrace() << "        |        install->commands:" << install->commands;
    logTrace() << "        |      configure->commands:" << configure->commands;
    logTrace() << "        |          start->commands:" << start->commands;
    logTrace() << "        |     afterStart->commands:" << afterStart->commands;
    logTrace() << "        |           stop->commands:" << stop->commands;
    logTrace() << "        |      afterStop->commands:" << afterStop->commands;
    logTrace() << "        |         reload->commands:" << reload->commands;
    logTrace() << "        |       validate->commands:" << validate->commands;
    logTrace() << "        |     babySitter->commands:" << babySitter->commands;

    return;
}


SvdServiceConfig::SvdServiceConfig(const QString& serviceName) {
    name = serviceName; // this must be declared first!
    uid = getuid();

    const auto defaults = loadDefaultIgniter();
    const auto root = loadIgniter();
    if (root.isEmpty()) {
        logError() << "Empty service config for:" << serviceName;
        auto serviceLoc = getSoftwareDataDir() + "/" + name;
        if (QFile::exists(serviceLoc + DEFAULT_SERVICE_RUNNING_FILE))
            touch(serviceLoc + STOP_TRIGGER_FILE);
    }
    nodeRoot_ = yajl_tree_parse(root.toUtf8(), errbuf, sizeof(errbuf));
    logTrace() << "INSIGHT of igniter:" << name << "::" << root;
    if (QString(errbuf).length() > 0) {
        auto source = getHomeDir() + DEFAULT_USER_IGNITERS_DIR + "/" + name + DEFAULT_SOFTWARE_TEMPLATE_EXT;
        auto dest = getOrCreateDir(getHomeDir() + DEFAULT_USER_IGNITERS_DIR) + "/../" + name + DEFAULT_SOFTWARE_TEMPLATE_EXT + ".error";
        if (QFile::exists(source)) {
            QFile::rename(source, dest);
            QFile::remove(source);
            // copyPath(getSoftwareDataDir() + "/" + name, getSoftwareDataDir() + "/" + name + DEFAULT_SERVICE_DISABLED_POSTFIX);
            QString msg = "Error loading igniter: " + name + " :: " + errbuf;
            notification(msg, FATAL);
        }
        return;
    }
    nodeDefault_ = yajl_tree_parse(defaults.toUtf8(), errbuf, sizeof(errbuf));
    if (QString(errbuf).length() > 0) {
        logError() << "Error in Defaults:" << name << "::" << errbuf;
        return;
    }
    if (not nodeDefault_) {
        logFatal() << "Json parse failure for Defaults!";
        return;
    }
    if (not nodeRoot_) {
        logError() << "Json parse failure for:" << serviceName;
        return;
    }

    // if (root.isEmpty()) {
    //     if (defaultsCache.isEmpty()) {
    //         logFatal() << "Failed to load mandatory" << name << "igniter. Cannot continue.";
    //         return;
    //     } else {
    //         logInfo() << "Updating igniter cache";
    //         defaultsCache = root;
    //     }
    // }

    auto hash = new QCryptographicHash(QCryptographicHash::Sha1);
    hash->addData(root.toUtf8(), root.length());
    this->sha = hash->result().toHex();
    delete hash;

    getOrCreateDir(prefixDir() + DEFAULT_SERVICE_PORTS_DIR);
    getOrCreateDir(prefixDir() + DEFAULT_SERVICE_ENVS_DIR);
    getOrCreateDir(prefixDir() + DEFAULT_SERVICE_PIDS_DIR);
    getOrCreateDir(prefixDir() + DEFAULT_SERVICE_LOGS_DIR);
    getOrCreateDir(prefixDir() + DEFAULT_SERVICE_CONFS_DIR);

    portsPool = getInteger("portsPool");
    QString portsDirLocation = prefixDir() + DEFAULT_SERVICE_PORTS_DIR;
    logTrace() << "Port pool for service:" << name << "=>" << QString::number(portsPool);

    if (portsPool > 1)
        if (QDir().exists(portsDirLocation))
            for (int indx = 1; indx < portsPool; indx++) {
                QString portFilePath = QString(portsDirLocation + "/" + QString::number(indx)).trimmed();
                if (not QFile::exists(portsDirLocation + QString::number(indx))) {
                    logTrace() << "Creating port file:" << portsDirLocation + QString::number(indx);
                    uint freePort = registerFreeTcpPort();
                    writeToFile(portFilePath, QString::number(freePort));
                }
            }

    /* then replace main port */
    staticPort = getInteger("staticPort");
    QString portFilePath = portsDirLocation + "/" + DEFAULT_SERVICE_PORT_NUMBER; // getOrCreateDir
    if (staticPort != -1) { /* defined static port */
        logTrace() << "Set static port:" << staticPort << "for service" << name;
        writeToFile(portFilePath, QString::number(staticPort));
        generatedDefaultPort = staticPort;
    } else {
        if (not QFile::exists(portFilePath)) {
            generatedDefaultPort = registerFreeTcpPort();
            logTrace() << "Set random free port:" << QString::number(generatedDefaultPort) << "for service" << name;
            writeToFile(portFilePath, QString::number(generatedDefaultPort));
        } else
            generatedDefaultPort = readFileContents(portFilePath).trimmed().toInt();
    }
    #ifdef QT_DEBUG
        Q_ASSERT(not QString::number(generatedDefaultPort).isEmpty());
    #endif

    shell = getString("shell");
    softwareName = getString("softwareName");
    autoStart = getBoolean("autoStart");
    notificationLevel = getInteger("notificationLevel");
    webApp = getBoolean("webApp");
    watchPort = getBoolean("watchPort");
    watchSocket = getBoolean("watchSocket");
    watchUdpPort = getBoolean("watchUdpPort");
    alwaysOn = getBoolean("alwaysOn");
    resolveDomain = getBoolean("resolveDomain");
    minimumRequiredDiskSpace = getInteger("minimumRequiredDiskSpace");
    repository = getString("repository");
    parentService = getString("parentService");
    configureOrder = getInteger("configureOrder");
    startOrder = getInteger("startOrder");

    dependencies = getArray("dependencies");
    standaloneDependencies = getArray("standaloneDeps");
    domains = getArray("domains");
    watchHttpAddresses = getArray("watchHttp");

    /* load service scheduler data */
    QStringList igniterSchedulers = getArray("schedulers");
    Q_FOREACH(QString scheduler, igniterSchedulers) {
        QString entry = scheduler.split(CRON_DELIMITER).first();
        QString commands = scheduler.split(CRON_DELIMITER).last();
        schedulers.push_back(
            new SvdScheduler(
                entry, replaceAllSpecialsIn(commands)
            ));
    }

    /* laod service hooks */
    install = new SvdShellOperations(replaceAllSpecialsIn(getString("install/commands")), replaceAllSpecialsIn(getString("install/expectOutput")));
    configure = new SvdShellOperations(replaceAllSpecialsIn(getString("configure/commands")), replaceAllSpecialsIn(getString("configure/expectOutput")));
    start = new SvdShellOperations(replaceAllSpecialsIn(getString("start/commands")), replaceAllSpecialsIn(getString("start/expectOutput")));
    afterStart = new SvdShellOperations(replaceAllSpecialsIn(getString("afterStart/commands")), replaceAllSpecialsIn(getString("afterStart/expectOutput")));
    stop = new SvdShellOperations(replaceAllSpecialsIn(getString("stop/commands")), replaceAllSpecialsIn(getString("stop/expectOutput")));
    afterStop = new SvdShellOperations(replaceAllSpecialsIn(getString("afterStop/commands")), replaceAllSpecialsIn(getString("afterStop/expectOutput")));
    reload = new SvdShellOperations(replaceAllSpecialsIn(getString("reload/commands")), replaceAllSpecialsIn(getString("reload/expectOutput")));
    validate = new SvdShellOperations(replaceAllSpecialsIn(getString("validate/commands")), replaceAllSpecialsIn(getString("validate/expectOutput")));
    babySitter = new SvdShellOperations(replaceAllSpecialsIn(getString("babySitter/commands")), replaceAllSpecialsIn(getString("babySitter/expectOutput")));


    /* on this stage, we want to replace these igniter constants: */
    QMap<QString, QString> injectHooks;
    injectHooks["SERVICE_INSTALL_HOOK"] = install->commands;
    injectHooks["SERVICE_START_HOOK"] = start->commands;
    injectHooks["SERVICE_STOP_HOOK"] = stop->commands;
    injectHooks["SERVICE_AFTERSTART_HOOK"] = afterStart->commands;
    injectHooks["SERVICE_AFTERSTOP_HOOK"] = afterStop->commands;
    injectHooks["SERVICE_CONFIGURE_HOOK"] = configure->commands;
    injectHooks["SERVICE_BABYSITTER_HOOK"] = babySitter->commands;
    injectHooks["SERVICE_VALIDATE_HOOK"] = validate->commands;

    QList<SvdShellOperations*> operations;
    operations << reload << install << start << stop << afterStart << afterStop << configure << babySitter << validate;
    Q_FOREACH(auto operation, operations) { /* for each operation, do hook injection */
        Q_FOREACH(QString hook, injectHooks.keys()) {
            QString old_commds = operation->commands;
            operation->commands = operation->commands.replace(hook, injectHooks.value(hook));
            if (operation->commands != old_commds) {
                logTrace() << "Performing igniter injections of hook:" << hook << "in service:" << name;
                logTrace() << hook << "- injecting content:" << injectHooks.value(hook) << " - COMMANDS: " << operation->commands;
                logTrace() << "OLD value:" << old_commds;
                logTrace() << "NEW value:" << operation->commands;
            }
        }
    }

    QString serviceDataDir = getOrCreateDir(
    #ifdef THESS_TEST_MODE
        getServiceDataDir(name + DEFAULT_TEST_POSTFIX)
    #else
        getServiceDataDir(name)
    #endif
    );
    QString autoStFile = serviceDataDir + AUTOSTART_TRIGGER_FILE;
    if (autoStart and not QFile::exists(autoStFile)) {
        logInfo() << "Autostart predefined on igniter side (has highest priority): Autostart state set for service:" << name;
        logTrace() << "Touching:" << autoStFile;
        touch(autoStFile);
    }

    #ifdef QT_DEBUG
        prettyPrint();
    #endif
}


/* destructor with memory free - welcome in C++ dmilith */
SvdServiceConfig::~SvdServiceConfig() {

    /* laod service hooks */
    if (install != NULL)
        delete install;
    if (configure != NULL)
        delete configure;
    if (start != NULL)
        delete start;
    if (afterStart != NULL)
        delete afterStart;
    if (stop != NULL)
        delete stop;
    if (afterStop != NULL)
        delete afterStop;
    if (reload != NULL)
        delete reload;
    if (validate != NULL)
        delete validate;
    if (babySitter != NULL)
        delete babySitter;
    for (int i = 0; i < schedulers.length(); i++) {
        if (schedulers.at(i) != NULL)
            delete schedulers.at(i);
    }
    if (nodeDefault_ != NULL)
        yajl_tree_free(nodeDefault_);
    if (nodeRoot_ != NULL)
        yajl_tree_free(nodeRoot_);
}


const QString SvdServiceConfig::serviceRoot() {
    return QString(SOFTWARE_DIR) + "/" + softwareName; // low prio
}


bool SvdServiceConfig::serviceInstalled() { /* XXX: it's not working properly.. and even can't for now. For services with != name than original software this function is useless.. TBR */
    logTrace() << "Software check:" << name;
    return QFile::exists(serviceRoot() + "/" + softwareName.toLower() + DEFAULT_SERVICE_INSTALLED_EXT);
}


bool SvdServiceConfig::serviceConfigured() {
    return QFile::exists(prefixDir() + DEFAULT_SERVICE_CONFS_DIR + "/" + releaseName() + DEFAULT_SERVICE_CONFIGURED_FILE);
}


const QString SvdServiceConfig::prefixDir() {
    QString optionalPostFix = "";
    #ifdef THESS_TEST_MODE
        optionalPostFix = DEFAULT_TEST_POSTFIX;
    #endif
    if (uid == 0) {
        return QString(SYSTEM_USERS_DIR) + SOFTWARE_DATA_DIR + "/" + name + optionalPostFix;
    } else {
        return QString(DEFAULT_HOME_DIR) + SOFTWARE_DATA_DIR + "/" + name + optionalPostFix;
    }
}


const QString SvdServiceConfig::releaseName() {
    return sha.right(20) + "." + QString::number(generatedDefaultPort); /* that's enough (?) */
}


const QString SvdServiceConfig::defaultTemplateFile() {
    /* pick of two possible locations: /SystemUsers/Igniters and ~/Igniters */

    if (getuid() == 0) {
        /* then, try system wide defaults */
        QString rootSideDefaultIgniter = QString(DEFAULT_SOFTWARE_TEMPLATE) + DEFAULT_SOFTWARE_TEMPLATE_EXT;
        if (QFile::exists(rootSideDefaultIgniter)) {
            return rootSideDefaultIgniter;
        } else {
            logFatal() << "Default igniter wasn't found. TheSS cannot continue.";
            return "";
        }
    }

    /* try user side defaults first */
    QString userSideDefaultIgniter = QString(DEFAULT_HOME_DIR) + "/Igniters/Default" + QString(DEFAULT_SOFTWARE_TEMPLATE_EXT);
    if (QFile::exists(userSideDefaultIgniter)) {
        logTrace() << "User side igniter Defaults found, and will be used:" << userSideDefaultIgniter;
        return userSideDefaultIgniter;
    }
    logFatal() << "Default root igniter wasn't found. TheSS cannot continue.";
    return "";
}


const QString SvdServiceConfig::address() {
    return tcpAddress;
}


const QString SvdServiceConfig::rootIgniter() {
    return QString(SYSTEM_USERS_DIR) + DEFAULT_USER_IGNITERS_DIR + "/" + name + DEFAULT_SOFTWARE_TEMPLATE_EXT;
}


const QString SvdServiceConfig::userIgniter() {
    return QString(DEFAULT_HOME_DIR) + DEFAULT_USER_IGNITERS_DIR + "/" + name + DEFAULT_SOFTWARE_TEMPLATE_EXT;
}


/* XXX: TODO: OPTIMIZE, define cache for values explictly read from files for each service hook for each service */
const QString SvdServiceConfig::replaceAllSpecialsIn(const QString content) {
    QString ccont = content;

    if (name == QString("Default")) {
        logTrace() << "No specials in Default file.";
        return ccont;
    } else if (content.trimmed().isEmpty()) {
        logTrace() << "No content to replace specials in.";
        return ccont;
    } else {

        /* Replace SERVICE_ROOT */
        logTrace() << "Service root for uid:" << uid << "should be located in:" << serviceRoot();
        ccont = ccont.replace("SERVICE_ROOT", serviceRoot());

        /* set service repository */
        if (not repository.isEmpty())
            ccont = ccont.replace("SERVICE_REPOSITORY", repository);

        /* Replace PARENT_SERVICE_PREFIX */
        QString depsFull;
        if (uid != 0)
            depsFull = QString(DEFAULT_HOME_DIR) + SOFTWARE_DATA_DIR + "/" + parentService; // getOrCreateDir(
        else
            depsFull = QString(SYSTEM_USERS_DIR) + SOFTWARE_DATA_DIR + "/" + parentService; // getOrCreateDir(

        if (parentService.isEmpty()) {
            logTrace() << "No dependencies for:" << name;
            ccont = ccont.replace("PARENT_SERVICE_PREFIX", prefixDir()); /* fallback to original prefix dir */
        } else {
            ccont = ccont.replace("PARENT_SERVICE_PREFIX", depsFull);
        }

        /* Replace SERVICE_PORT */
        ccont = ccont.replace("SERVICE_PORT", QString::number(generatedDefaultPort));
        for (int indx = 1; indx < portsPool; indx++) {
            QString additionalPort = prefixDir() + DEFAULT_SERVICE_PORTS_DIR + "/" + QString::number(indx);
            logTrace() << "Defined additional port:" << additionalPort;
            ccont = ccont.replace("SERVICE_PORT" + QString::number(indx), readFileContents(additionalPort).trimmed());
        }

        /* Replace SERVICE_PREFIX */
        ccont = ccont.replace("SERVICE_PREFIX", prefixDir());

        /* Replace SERVICE_RELEASE and related values */
        ccont = ccont.replace("SERVICE_RELEASE", releaseName());

        // todo: add SERVICE_CONFIG,
        ccont = ccont.replace("SERVICE_CONF", getOrCreateDir(prefixDir() + DEFAULT_SERVICE_CONFS_DIR + "/" + releaseName()) + DEFAULT_SERVICE_CONF_FILE);
        ccont = ccont.replace("SERVICE_LOG", getOrCreateDir(prefixDir() + DEFAULT_SERVICE_LOGS_DIR + "/" + releaseName()) + DEFAULT_SERVICE_LOG_FILE);
        ccont = ccont.replace("SERVICE_ENV", getOrCreateDir(prefixDir() + DEFAULT_SERVICE_ENVS_DIR + "/" + releaseName()) + DEFAULT_SERVICE_ENV_FILE);
        ccont = ccont.replace("SERVICE_PID", getOrCreateDir(prefixDir() + DEFAULT_SERVICE_PIDS_DIR + "/" + releaseName()) + DEFAULT_SERVICE_PID_FILE);
        ccont = ccont.replace("SERVICE_SOCK", prefixDir() + DEFAULT_SERVICE_SOCKET_FILE);

        /* Replace SERVICE_DOMAINS and SERVICE_DOMAIN */
        QStringList userDomains;
        if (domains.length() > 0) {
            /* add igniter side domains to user domains: */
            userDomains << domains;
        }
        if (userDomains.isEmpty()) {
            QHostInfo info = QHostInfo();
            if (info.localHostName().isEmpty()) {
                logWarn() << "Detected no domains in local DNS nand base igniter. You might want to update your Default.json (bin/ignitersinstall). Falling back to default:" << DEFAULT_SYSTEM_DOMAIN;
                userDomains << DEFAULT_SYSTEM_DOMAIN;
            } else
                userDomains << info.localHostName() + DEFAULT_HOME_DOMAIN;
        }

        logTrace() << "User domains:" << userDomains.join(" ");
        ccont = ccont.replace("SERVICE_DOMAINS", userDomains.join(" "));
        ccont = ccont.replace("SERVICE_DOMAIN", userDomains.last()); /* replace with last domain on list */

        /* Replace SERVICE_ADDRESS */
        QString address = QString(DEFAULT_LOCAL_ADDRESS);
        QString userAddress = "";
        QHostInfo info;
        if (not userDomains.isEmpty()) {
            if (resolveDomain) { /* by default domain resolve is done for each domain given by user */
                Q_FOREACH(auto domdom, userDomains) {
                    info = QHostInfo::fromName(domdom);
                    if (not info.addresses().isEmpty()) {
                        auto list = info.addresses();
                        QString replaceWith = "";
                        Q_FOREACH(QHostAddress value, list) {
                            logTrace() << "Processing an address:" << value.toString();
                            userAddress = value.toString();
                            if (userAddress == DEFAULT_LOCAL_ADDRESS) {
                                logTrace() << "Ignoring localhost address for domain resolve.";
                            } else if (userAddress.contains(":")) {
                                logTrace() << "Ignoring IPV6 address:" << userAddress;
                            } else {
                                replaceWith = userAddress;
                                logTrace() << "Resolved address of domain " << domdom << " is " << userAddress;
                            }
                        }
                        /* replace address */
                        if (replaceWith.isEmpty()) {
                            logTrace() << "Fallback to local address for domain:" << domdom;
                            ccont = ccont.replace("SERVICE_ADDRESS", DEFAULT_LOCAL_ADDRESS);
                            tcpAddress = DEFAULT_LOCAL_ADDRESS;
                        } else {
                            logTrace() << "Final result of domain resolve is:" << replaceWith;
                            tcpAddress = replaceWith;
                            ccont = ccont.replace("SERVICE_ADDRESS", replaceWith); /* replace with user address content */
                        }
                    } else {
                        logTrace() << "Empty domain resolve of: " << domdom << "for service:" << name << "Setting local address: " << DEFAULT_LOCAL_ADDRESS;
                        tcpAddress = DEFAULT_LOCAL_ADDRESS;
                        ccont = ccont.replace("SERVICE_ADDRESS", DEFAULT_LOCAL_ADDRESS);
                    }
                }

            } else { /* don't resolve domain, just bind to local address.. */
                logTrace() << "Set address to local address: " << DEFAULT_LOCAL_ADDRESS;
                ccont = ccont.replace("SERVICE_ADDRESS", DEFAULT_LOCAL_ADDRESS);
                tcpAddress = DEFAULT_LOCAL_ADDRESS;
            }
        } else {
            logTrace() << "Set address to local address: " << DEFAULT_LOCAL_ADDRESS;
            ccont = ccont.replace("SERVICE_ADDRESS", address);
            tcpAddress = address;
        }
        return ccont;
    }
}


/*
 *  Load igniter data in Json.
 */
QString SvdServiceConfig::loadDefaultIgniter() {
    QFile defaultIgniter(defaultTemplateFile()); /* try loading root igniter as second */
    if(not defaultIgniter.open(QIODevice::ReadOnly)) { /* check file access */
        logError() << "NO loadDefaultIgniter: " << defaultTemplateFile();
    } else {
        defaultIgniter.close();
        QString buffer = readFileContents(defaultTemplateFile()).replace("\\\n", "\\n");
        return buffer;
    }
    defaultIgniter.close();
    return "";
}


/*
 *  Load igniter data in Json.
 */
QString SvdServiceConfig::loadIgniter() {
    // auto result = new Json::Value();
    QFile fileUser(userIgniter()); /* try loading user igniter as first */
    QFile fileRoot(rootIgniter()); /* try loading root igniter as third */

    if (not fileUser.open(QIODevice::ReadOnly)) { /* check file access */
        logDebug() << "No file: " << userIgniter();
    } else {
        fileUser.close();
        QString buffer = readFileContents(userIgniter()).trimmed().replace("\\\n", "\\n");;
        return buffer;
    }
    fileUser.close();

    if (not fileRoot.open(QIODevice::ReadOnly)) {
        logDebug() << "No file: " << rootIgniter();
        fileRoot.close();
        return "";
    }
    fileRoot.close();

    return readFileContents(rootIgniter()).trimmed().replace("\\\n", "\\n");;
}


SvdShellOperations::SvdShellOperations() {
    commands = QString();
    expectOutput = QString();
}


SvdShellOperations::SvdShellOperations(const QString& initialCommand, const QString& initialExpectOutput) {
    commands += initialCommand;
    expectOutput += initialExpectOutput;
}
