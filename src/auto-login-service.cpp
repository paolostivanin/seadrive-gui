#include <QDesktopServices>
#include <QHash>

#include "seadrive-gui.h"
#include "account-mgr.h"
#include "api/api-error.h"
#include "api/requests.h"
#include "utils/utils.h"

#include "auto-login-service.h"

namespace {

} // namespace

SINGLETON_IMPL(AutoLoginService)

AutoLoginService::AutoLoginService(QObject *parent)
    : QObject(parent)
{
}

void AutoLoginService::startAutoLogin(const Account& account,
                                      const QString& next_url)
{
    QUrl absolute_url = QUrl(next_url).isRelative()
                            ? account.getAbsoluteUrl(next_url)
                            : next_url;
    if (!account.isValid() || !account.isAtLeastVersion(4, 2, 0)) {
        QDesktopServices::openUrl(absolute_url);
        return;
    }

    QString next = absolute_url.path();
    if (absolute_url.hasQuery()) {
        next += "?" + absolute_url.query();
    }
    if (!absolute_url.fragment().isEmpty()) {
        next += "#" + absolute_url.fragment();
    }
    GetLoginTokenRequest *req = new GetLoginTokenRequest(account, next);

    connect(req, SIGNAL(success(const QString&)),
            this, SLOT(onGetLoginTokenSuccess(const QString&)));

    connect(req, SIGNAL(failed(const ApiError&)),
            this, SLOT(onGetLoginTokenFailed(const ApiError&)));

    req->send();
}

void AutoLoginService::onGetLoginTokenSuccess(const QString& token)
{
    GetLoginTokenRequest *req = (GetLoginTokenRequest *)(sender());
    // printf("login token is %s\n", token.toUtf8().data());

    QUrl url = req->account().getAbsoluteUrl("/client-login/");
    QString next_url = req->nextUrl();

    QMultiHash<QString, QString> params;
    params.insert("token", token);
    params.insert("next", req->nextUrl());
    url = includeQueryParams(url, params);

    QDesktopServices::openUrl(url);
    req->deleteLater();
}

void AutoLoginService::onGetLoginTokenFailed(const ApiError& error)
{
    GetLoginTokenRequest *req = (GetLoginTokenRequest *)(sender());
    qWarning("get login token failed: %s\n", error.toString().toUtf8().data());
    // server doesn't support client directly login, or other errors happened.
    // We open the server url directly in this case;
    QDesktopServices::openUrl(req->account().getAbsoluteUrl(req->nextUrl()));
    req->deleteLater();
}
