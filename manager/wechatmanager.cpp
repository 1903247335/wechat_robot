#include "wechatmanager.h"
#include"global.h"
#include"server/cppserver.h"
#include"message.pb.h"
WechatManager::WechatManager() {}

void WechatManager::sendMessage(const QString &name, const QString &content,const QString &type)
{
    MyNetwork::Request request;
    request.set_type(MyNetwork::SENDMESSAGE);
    request.set_id(0);

    MyNetwork::SendMessageRequest data;
    std::string buffer;
    std::string sName;
    std::string sContent;


    sName=name.toStdString();
    sContent=content.toStdString();

    data.set_wxid(sName);
     data.set_content(sContent);
    data.set_msg_type(static_cast<MyNetwork::SendMsgType>(type.toInt()));

    data.SerializeToString(&buffer);

    request.set_data(buffer);

    g_cppserver->send(request);


}
