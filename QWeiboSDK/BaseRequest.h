#ifndef BASEREQUEST_H
#define BASEREQUEST_H

#include <QObject>
#include <QMap>
#include <QStringList>

#include "global.h"

class QTimer;
class QNetworkAccessManager;
class QNetworkReply;
class QNetworkDiskCache;
namespace QWeiboSDK {

class QWEIBOSDK_EXPORT BaseRequest : public QObject
{
    Q_OBJECT
    Q_ENUMS(RequestRet)
public:
    enum RequestRet {
        RET_ABORT = 0x0,
        RET_SUCCESS,
        RET_FAILURE
    };

    explicit BaseRequest(QObject *parent = 0);
    virtual ~BaseRequest();

    void setBaseUrl(const QString &url);

    void initiate();

    Q_INVOKABLE void appendPostDataParameters(const QString &key, const QString &value);
    inline QMap<QString, QString> postDataParameters() const {
        return m_postDataParameters;
    }
    inline QString postDataParameter(const QString &key, const QString &defaultValue = QString()) const {
        if (!m_postDataParameters.isEmpty () && m_postDataParameters.contains (key))
            return m_postDataParameters.value (key, defaultValue);
        return QString();
    }
    Q_INVOKABLE void setParameters(const QString &key, const QString &value);
    inline QStringList parameterKeys() const {
        if (m_parameters.isEmpty ())
            return QStringList();
        return m_parameters.keys ();
    }
    inline QString parameter(const QString &key, const QString &defaultValue = QString()) const {
        if (!m_parameters.isEmpty () && m_parameters.contains (key))
            return m_parameters.value (key, defaultValue);
        return QString();
    }

    BaseRequest& operator ()(const QString &key, const QVariant &value);
    BaseRequest& operator ()(const QString &key, const char *value);
    BaseRequest& operator ()(const QString &key, int value);

    void setTimerInterval(int msec);
protected:
    virtual void initParameters();
    void setUrlPath(const QString &urlPath, const QString &tag = QString(".json"));
    QUrl initUrl();
    void startTimeout();
    void stopTimeout();
    QNetworkAccessManager *curNetworkAccessMgr();
    virtual QNetworkReply *curNetworkReply();
    bool requestAborted();
    void setRequestAborted(bool shouldAborted);
signals:
    void requestSuccess(const QString &replyData);
    void requestFailure(const QString &replyData);
    void requestAbort();
    void requestResult(RequestRet ret, const QString &replyData);
public slots:
    virtual void postRequest();
    virtual void getRequest();
//private slots:
//    void requestFinished();
private:
    bool m_Editable;
    bool m_requestAborted;
    int m_timerInterval;
    QString m_baseUrl;
    QString m_urlPath;
    QMap<QString, QString> m_parameters;
    QMap<QString, QString> m_postDataParameters;
    QNetworkAccessManager *m_networkMgr;
    QNetworkReply *m_reply;
    QNetworkDiskCache *m_diskCache;
    QTimer *m_timeout;
};
} //QWeiboSDK
#endif // BASEREQUEST_H
