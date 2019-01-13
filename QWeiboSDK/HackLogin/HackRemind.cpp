#include "HackRemind.h"

namespace QWeiboSDK {
namespace HackLogin {

HackRemindUnreadCount::HackRemindUnreadCount(QObject *parent)
    : BaseHackRequest(parent)
{
    setUrlPath ("api/remind/unread", "");
    initiate ();
}



} //HackLogin
} //QWeiboSDK
