#include "BaseWrapper.h"

#include <QDebug>

#include "BaseRequest.h"
#include "BaseHackRequest.h"
#include "HackRequestCookieJar.h"
#include "TokenProvider.h"

namespace QWeiboSDK {
namespace Wrapper {
using namespace HackLogin;
BaseWrapper::BaseWrapper(QObject *parent)
    : QObject(parent)
    , m_request(nullptr)
    , m_tokenProvider(TokenProvider::instance ())
    , m_cookieJar(nullptr)
{
    m_useHackLogin = m_tokenProvider->useHackLogin ();
}

BaseWrapper::~BaseWrapper()
{
    if (m_request)
        m_request->deleteLater ();
}

void BaseWrapper::appendPostDataParameters(const QString &key, const QString &value)
{
    if (m_request)
        m_request->appendPostDataParameters (key, value);
}

void BaseWrapper::setParameters(const QString &key, const QString &value)
{
    if (!m_request) {
        qWarning()<<Q_FUNC_INFO<<"BaseRequest is nullptr, can't set parameters!!!!";
        return;
    }
    QString s = convertParameterKey (key);
    if (!s.isEmpty ())
        m_request->setParameters (convertParameterKey (key), value);
    else
        qDebug()<<Q_FUNC_INFO<<QString(">>>>>> ignore empty key with value [%1=%2] <<<<<<")
                  .arg (key).arg (value);
}

void BaseWrapper::appendExtraRequestCookie(HackRequestCookieJar *cookieJar)
{
    if (m_cookieJar != cookieJar)
        m_cookieJar = cookieJar;
    if (m_useHackLogin && m_request) {
        BaseHackRequest *r = qobject_cast<BaseHackRequest*>(m_request);
        if (r)
            r->appendExtraRequestCookie (cookieJar);
    }
}

void BaseWrapper::resetBaseUrl(const QString &newUrl)
{
    if (newUrl.isEmpty ()) {
        qWarning()<<Q_FUNC_INFO<<"Invalid empty new url";
        return;
    }
    if (!m_request) {
        qWarning()<<Q_FUNC_INFO<<"BaseRequest is nullptr, can't set parameters!!!!";
        return;
    }
    if (!m_useHackLogin) {
        qWarning()<<Q_FUNC_INFO<<"resetBaseUrl only support hack login, current use oauth login";
        return;
    }
    BaseHackRequest *req = qobject_cast<BaseHackRequest*>(m_request);
    if (!req) {
        qWarning()<<Q_FUNC_INFO<<"covert to BaseHackRequest error";
        return;
    }
    req->resetBaseUrl (newUrl);
}

QString BaseWrapper::convertParameterKey(const QString &key)
{
    return key;
}

void BaseWrapper::setRequest(BaseRequest *request)
{
    if (m_request == request)
        return;

    if (m_request) {
        m_request->disconnect ();
    }
    m_request = request;
    if (m_request) {
        connect (m_request, &BaseRequest::requestAbort, this, &BaseWrapper::requestAbort);
        connect (m_request, &BaseRequest::requestFailure, this, &BaseWrapper::requestFailure);
        connect (m_request, &BaseRequest::requestResult, this, &BaseWrapper::requestResult);
        connect (m_request, &BaseRequest::requestSuccess,
                 [&](const QString &replyData){
            QString str = parseContent (replyData);
            emit requestSuccess (str);
            emit requestResult (BaseRequest::RET_SUCCESS, str);
        });
    }
}

QString BaseWrapper::parseContent(const QString &content)
{
    qDebug()<<Q_FUNC_INFO<<">>>>>> use default parseContent <<<<<<";
    return content;
}

void BaseWrapper::postRequest()
{
    if (m_request)
        m_request->postRequest ();
}

void BaseWrapper::getRequest()
{
    if (m_request)
        m_request->getRequest ();
}

} //Wrapper
} //QWeiboSDK

