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

#ifndef QWEIBOREQUEST_H
#define QWEIBOREQUEST_H

#include <QMap>
#include <QVariant>

#include "qweiboapi_global.h"

namespace QSinaWeiboAPI {

static const QString kOAuthUrl = "https://api.weibo.com/oauth2/access_token";
static const QString kTokenCheckUrl = "https://api.weibo.com/oauth2/get_token_info";
static const QString kApiHost = "https://api.weibo.com/";
//appkey, appsecret are weico for iOS
//static QString sAppKey = "82966982";
//static QString sAppSecret = "72d4545a28a46a6f329c4f2b1e949e6a";
static QString sAppKey = "2323547071";
static QString sAppSecret = "16ed80cc77fea11f7f7e96eca178ada3";

///weico的新登陆验证方式（需要使用webview进行认证）
static const QString kOAuth2AccessAuthorize = "https://open.weibo.cn/oauth2/authorize";
static const QString kAppId = "211160679";
static const QString kAppKeyHash = "1e6e33db08f9192306c4afa0a61ad56c";
static const QString kRedirectUri = "http://oauth.weico.cc";
static const QString kPackageName = "com.eico.weico";
static const QString kScope = "email,direct_messages_read,direct_messages_write,friendships_groups_read,friendships_groups_write,statuses_to_me_read,follow_app_official_microblog,invitation_write";
static const QString kWeicoApi = kOAuth2AccessAuthorize+ "?" + "client_id=" + kAppId
        + "&response_type=token&redirect_uri=" + kRedirectUri
        + "&key_hash=" + kAppKeyHash + "&packagename=" + kPackageName
        + "&display=mobile" + "&scope=" + kScope;

class QWEIBOAPI_EXPORT QWeiboRequest/* : public QObject*/
{
//    Q_OBJECT
public:
    enum WeiboRequestType {
        Get, Post
    };

    QWeiboRequest(/*QObject *parent = 0*/);
    virtual ~QWeiboRequest() {}
    
    WeiboRequestType getRequestType() const;
    void setRequestType(WeiboRequestType type);
    
    QString apiUrl() const;
    QUrl url() const; //apiUrl() + parameters
    /*!
     * \brief prepare
     * \return
     * initialize api parameters. usually you don't have to call it. do nothing if already called
     */
    QWeiboRequest& prepare();
    //only the existing (name, value) will be modified, otherwise do nothing
    QWeiboRequest& operator ()(const QString& name, const QVariant& value);
    void addImage(const QString& file);
    void addImage(const QByteArray& data, const QString& format);

    QMap<QString, QVariant> paramsters() const;
protected:
    virtual void initParameters() {qDebug("Request::initParameters() !!!");}

    bool mEditable; //true in ctor
    WeiboRequestType mType;
    QString mApiUrl;
    QString mApiPath;
    QMap<QString, QVariant> mParameters;
};

} //namespace QWeiboAPI
#endif // QWEIBOREQUEST_H
