/**
 *  @author dmilith
 *
 *   © 2013 - VerKnowSys
 *
 */

#include "webapp_types.h"


bool shouldExist(const QStringList filesThatShouldExist, const QString& path) {
    Q_FOREACH(QString file, filesThatShouldExist) {
        logDebug() << "Checking existance of file:" << path + file;
        if (not QFile::exists(path + file)) return false;
    }
    return true;
}


bool shouldNotExist(const QStringList filesThatShouldNotExist, const QString& path) {
    Q_FOREACH(QString file, filesThatShouldNotExist) {
        logDebug() << "Checking nonexistance of file:" << path + file;
        if (QFile::exists(path + file)) return false;
    }
    return true;
}


WebAppTypeDetector::WebAppTypeDetector(const QString& path) {

    QStringList filesThatShouldExist, filesThatShouldNotExist;
    for (WebAppTypes i = RubySite; i <= StaticSite; i = static_cast<WebAppTypes>(static_cast<int>(i) + 1)) {
        filesThatShouldExist = QStringList();
        filesThatShouldNotExist = QStringList();

        switch (i) {

            case RubySite:
                this->typeName = "Ruby";
                filesThatShouldExist << "/Gemfile";
                filesThatShouldNotExist << "/index.html" << "/index.php";
                break;

            case NodeSite:
                this->typeName = "Node";
                filesThatShouldExist << "/package.json";
                filesThatShouldNotExist << "/index.html" << "/Gemfile" << "/Rakefile" << "/Procfile";
                break;

            case PhpSite:
                this->typeName = "Php";
                filesThatShouldExist << "/index.php";
                filesThatShouldNotExist << "/Gemfile" << "/package.json" << "/Rakefile" << "/Procfile";
                break;

            default:
                this->typeName = "Static";
                this->appType = StaticSite;
                return;
        }

        if (shouldExist(filesThatShouldExist, path) and shouldNotExist(filesThatShouldNotExist, path)) {
            logInfo() << "Detected web-app type:" << this->typeName;
            this->appType = i;
            return;
        }

    }
}


WebAppTypes WebAppTypeDetector::getType() {
    return this->appType;
}
