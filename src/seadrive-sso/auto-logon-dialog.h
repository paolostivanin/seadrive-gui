#ifndef SEAFILE_CLIENT_AUTO_LOGON_H
#define SEAFILE_CLIENT_AUTO_LOGON_H
#include <QDialog>
#include <QUrl>

#include "account.h"

class AutoLogonDialog : public QDialog
{
    Q_OBJECT
public:
    AutoLogonDialog(const QUrl& url = QUrl(),
                    QWidget* parent = 0);

private slots:
    void startAutoLogon();

private:
    void warn(const QString& msg);
    QUrl readServerUrlFromRegistry();
    QUrl askForServerUrl();
    QUrl readServerUrlFromConfigFile(const QString& key);
    void errorAndExit(const QString& msg = QString());
    Account parseAccount(const QString& cookie_value);

    QUrl login_url_;
};

#endif
