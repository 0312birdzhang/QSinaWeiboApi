/******************************************************************************
    Weibo: login, logout and upload api
    Copyright (C) 2012-2014 Wang Bin <wbsecg1@gmail.com>
    Copyright (C) 2014 wanggjghost <41245110@qq.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
******************************************************************************/

#include <QDir>
#include <QFile>
#include <QUrlQuery>
#include <QImage>
#include <QtDebug>
#include <QJsonObject>
#include <QJsonDocument>

#include "include/QSinaWeibo.h"
#include "include/QWeiboPut.h"
#include "include/QWeiboRequest.h"


namespace QSinaWeiboAPI {

QSinaWeibo::QSinaWeibo(QObject *parent)
    :QObject(parent)
{
    mPut = new QWeiboPut(this);
    //connect(mPut, SIGNAL(ok(QString)), this, SIGNAL(ok()));
    connect(mPut, SIGNAL(fail(QString)), this, SIGNAL(weiboPutFail(QString)));
    connect(mPut, SIGNAL(fail(QString)), this, SLOT(dumpError(QString)));
    connect(mPut, SIGNAL(weiboPutSucceed(QString)), this, SIGNAL(weiboPutSucceed(QString)));
    connect(mPut, SIGNAL(weiboPutSucceed(QString)), this, SLOT(dumpOk(QString)));
    connect(this, SIGNAL(loginSucceed(QString, QString)), SLOT(processNextRequest()), Qt::DirectConnection);
    connect(this, SIGNAL(loginSucceed(QString, QString)), this, SIGNAL(loginSucceed(QString, QString)));
}

QSinaWeibo::~QSinaWeibo()
{
    if (!mRequests.isEmpty()) {
        qDeleteAll(mRequests);
        mRequests.clear();
    }
}

void QSinaWeibo::setUser(const QString &user)
{
    mUser = user;
}

void QSinaWeibo::setPassword(const QString &passwd)
{
    mPasswd = passwd;
}

void QSinaWeibo::setAccessToken(const QString &token)
{
    mAccessToken = token;
    emit accessTokenChanged();
}

QString QSinaWeibo::getUid() const
{
    return mUid;
}

void QSinaWeibo::setUid(const QString &uid)
{
    mUid = uid;
}

QString QSinaWeibo::getAccessToken() const
{
    return mAccessToken;
}

void QSinaWeibo::createRequest(QWeiboRequest *request)
{
    mRequests.append(request);
    //TODO: better way to check login(from error code)
    if (mAccessToken.isEmpty()) {
        qDebug("Not login.");
        //login();
        emit weiboPutFail(QString("AccessToken empty!!"));
    } else {
        processNextRequest();
    }
}

void QSinaWeibo::login()
{
    if (mUser.isEmpty() || mPasswd.isEmpty()) {
        qWarning("user name and password can't be empty");
        return;
    }
    mPut->reset();
    QUrl url(kOAuthUrl);
    QUrlQuery urlQuery;
    urlQuery.addQueryItem("client_id", sAppKey);
    urlQuery.addQueryItem("client_secret", sAppSecret);
    urlQuery.addQueryItem("grant_type", "password");
    urlQuery.addQueryItem("username", mUser);
    urlQuery.addQueryItem("password", mPasswd);
    url.setQuery(urlQuery);
    connect(mPut, SIGNAL(weiboPutSucceed(QString)), SLOT(parseOAuth2ReplyData(QString)));
    connect(mPut, SIGNAL(fail(QString)), SIGNAL(loginFail(QString)));
    mPut->setUrl(url);
    qDebug("begin login...");
    mPut->post();
}

void QSinaWeibo::logout()
{

}
////仅支持JPEG、GIF、PNG格式，图片大小小于5M
//void QSinaWeibo::updateStatusWithPicture(const QString &status, const QString &fileName)
//{
//    mStatus = status;
//    mFile = fileName;
//    if (mAccessToken.isEmpty()) {
//        qDebug("Not login.");
//        connect(this, SIGNAL(loginOk()), SLOT(sendStatusWithPicture()));
//        login();
//        return;
//    }
//    sendStatusWithPicture();
//}

void QSinaWeibo::processNextRequest()
{
    if (mRequests.isEmpty())
        return;
    QWeiboRequest *request = mRequests.takeFirst();
    (*request)
            ("access_token", mAccessToken)
            ("uid", mUid)
            ;
    mPut->reset();
    mPut->setUrl(request->url());
    if (request->getRequestType() == QWeiboRequest::Get) {
        mPut->get();
    } else if (request->getRequestType() == QWeiboRequest::Post){
        mPut->post();
    }
    delete request;
}

void QSinaWeibo::parseOAuth2ReplyData(const QString &data)
{
    static bool in = false;
    if (in)
        return;
    in = true;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data.toLocal8Bit(), &error);

    if (error.error != QJsonParseError::NoError) {
        emit loginFail(QString("QJsonParseError %1").arg(error.errorString()));
        return;
    }
    QJsonObject jsonObj = doc.object();

    mAccessToken = jsonObj.value("access_token").toString();
    mUid = jsonObj.value("uid").toString();

    qDebug("token=%s, uid=%s", mAccessToken.constData(), mUid.constData());

    disconnect(this, SLOT(parseOAuth2ReplyData(QString)));
    disconnect(this, SIGNAL(loginFail()));
    //emit loginOk(mAccessToken, mUid);
}

//void QSinaWeibo::sendStatusWithPicture()
//{
//    qDebug("update weibo with picture");
//    QString path(mFile);
//    //TODO: gif
//    if (!path.endsWith("jpg", Qt::CaseInsensitive)
//            && !path.endsWith("jpeg", Qt::CaseInsensitive)
//            && !path.endsWith("png", Qt::CaseInsensitive)
//            && !path.endsWith("gif", Qt::CaseInsensitive)) {
//        QImage image(path);
//        path = QDir::tempPath() + "/weibotemp" + ".jpg";
//        if (!image.save(path)) {
//            qWarning("convert image failed! %s", qPrintable(path));
//            return;
//        }
//    }
//    QFile f(path);
//    if (!f.open(QIODevice::ReadOnly)) {
//        qDebug("open error: %s", qPrintable(f.errorString()));
//        return;
//    }
//    QByteArray data = f.readAll();
//    f.close();

//    connect(mPut, SIGNAL(ok(QString)), this, SIGNAL(sendOk()));

//    mPut->reset();
//    QUrl url(kApiHost + "statuses/upload.json");
//    mPut->setUrl(url);
//    mPut->addTextPart("access_token", mAccessToken);
//    mPut->addTextPart("status", QUrl::toPercentEncoding(mStatus));
//    mPut->addDataPart("image/jpg", "pic", data, path);
//    mPut->upload();
//}

void QSinaWeibo::dumpError(const QString &error)
{
    qDebug() << ">>>>>>>" << __FUNCTION__ << error  << "<<<<<<<";
}

void QSinaWeibo::dumpOk(const QString &data)
{
    qDebug() << ">>>>>>>" << __FUNCTION__ << data << "<<<<<<<";
}

} //namespace QWeiboAPI
