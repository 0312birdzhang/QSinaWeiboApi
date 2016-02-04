#include "FriendshipsGroupsWrapper.h"

#include <QDebug>

#include "HackLogin/HackFriendshipsGroups.h"
#include "HackLogin/BaseHackRequest.h"

#include "QWeiboRequest.h"
#include "TokenProvider.h"

namespace QWeiboSDK {
namespace Wrapper {

using namespace HackLogin;
using namespace htmlcxx;
using namespace std;

FriendshipsGroupsTimelineWrapper::FriendshipsGroupsTimelineWrapper(QObject *parent)
    : BaseWrapper(parent)
{
    registerRequest<FriendshipsGroupsTimeline, HackFriendshipsGroupsTimeline>();
}

QString FriendshipsGroupsTimelineWrapper::parseContent(const QString &content)
{
    qDebug()<<Q_FUNC_INFO<<"=======";

    HTML::ParserDom parser;
    tree<HTML::Node> dom = parser.parseTree(QString(content).toStdString ());

    tree<HTML::Node>::iterator domBeg = dom.begin();
    tree<HTML::Node>::iterator domEnd = dom.end();

    bool success = true;
    QString tmp = QString();
//    for (; domBeg != domEnd; ++domBeg) {  // 遍历文档中所有的元素
    while (domBeg != domEnd) {
        if (!(*domBeg).tagName ().compare ("a")) {
            domBeg->parseAttributes();
            QString cv = QString::fromStdString ((*domBeg).attribute ("href").second);
//            qDebug()<<Q_FUNC_INFO<<"a value ["<<cv<<"], text is "<<QString::fromStdString ((*domBeg).text ());
//            if (cv == "me") { //captcha error
//                success = false;
//                for (; domBeg != domEnd; ++domBeg) {
//                    if (! (*domBeg).isTag ()) {
//                        tmp = QString::fromStdString ((*domBeg).text ());
//                        break;
//                    }
//                }
//                break;
//            }
            if (cv.contains ("attgroup/show")) {
                qDebug()<<Q_FUNC_INFO<<"***************************";
                qDebug()<<Q_FUNC_INFO<<"a value ["<<cv<<"], text is "<<QString::fromStdString ((*domBeg).text ());
                ++domBeg;
                if (! (*domBeg).isTag ()) {
                    tmp = QString::fromStdString ((*domBeg).text ());
                    qDebug()<<Q_FUNC_INFO<<"text is "<<tmp;
                    continue;
                }
            }
        }
        ++domBeg;
    }
    return QString();
}

} //Wrapper
} //QWeiboSDK
