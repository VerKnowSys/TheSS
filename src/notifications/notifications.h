/**
 *  @author dmilith
 *
 *   © 2013 - VerKnowSys
 *
 */


#ifndef __NOTIFICATIONS_CENTER__
#define __NOTIFICATIONS_CENTER__


#include <QtCore>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>


enum NotificationType {HIPCHAT, DEFAULT = 1};


static QNetworkAccessManager *networkAccessManager = new QNetworkAccessManager();
static QMap<QString, int> history = QMap<QString, int>(); /* content, amount */


class Notification {


    public:
        QString fromName() { return "TheSS"; };
        QString roomName() { return "DevOps"; };
        QString notifyColor() { return "green"; };
        QString warningColor() { return "yellow"; };
        QString errorColor() { return "red"; };
        QString notifyAll() { return "0"; };

        QString urlAddress();
        QString apiKey();

};


void notification(const QString& notificationMessage);
void notification(const QString& notificationMessage, NotificationType type);


#endif
