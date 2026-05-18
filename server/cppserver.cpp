#include "cppserver.h"
#include <QFile>
#include <QImage>

#ifdef WX_WITH_PROTOBUF
#include "message.pb.h"
#endif

CppServer::CppServer(QObject *parent): QObject(parent)
{
    server = new QTcpServer(this);
}

void CppServer::startListening()
{
    if (!server->isListening()) {
        if (server->listen(QHostAddress::Any, 8888)) {
            qDebug() << "监听已开启: 8888";
            connect(server, &QTcpServer::newConnection, this, &CppServer::onNewConnection);
        }
    }
}



void CppServer::onNewConnection()
{
    auto clientSocket = server->nextPendingConnection();
    qDebug() << "客户端已连接: " << clientSocket->peerAddress().toString();

    connect(clientSocket, &QTcpSocket::readyRead, this, &CppServer::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &CppServer::onDisconnected);

    qDebug() << "New client connected:" << clientSocket->peerAddress().toString();
}

void CppServer::showQRcode(const MyNetwork::Response& response)
{
    const std::string& rawData=response.data();
    QByteArray data(rawData.data(), static_cast<int>(rawData.size()));


    QString base64Content = "data:image/png;base64," +data.toBase64();
    emit changeToScanQRcode(base64Content);




}


QTcpSocket * CppServer::castToQTcpSocket(){
     return qobject_cast<QTcpSocket *>(sender());

}

void CppServer::showWaitAuth(const MyNetwork::Response& response){

     const std::string& data=response.data();
    QString headUrl=QString::fromStdString(data);
    emit changeToWaitAuth(headUrl);
}
void CppServer::showLoginSuccess(){


    emit changeToLoginSuccess();
}
void CppServer::showLoginFail(){


    emit changeToLoginFail();
}
void CppServer::showAuthSuccess(){

    emit changeToAuthSuccess();
}


void CppServer::handleMSG(const  MyNetwork::Response&msg){
    qCDebug(logServer) << "类型"<<msg.type()<<"id"<<msg.id()<<"大小"<<msg.data().size();
    switch (msg.type()) {
    case MyNetwork::SCANQRCODE:
        showQRcode(msg);
        break;
    case MyNetwork::WAITAUTH:
        showWaitAuth(msg);
        break;
    case MyNetwork::LOGINSUCCESS:
        showLoginSuccess();
        break;
    case MyNetwork::LOGINFAIL:
        showLoginFail();
        break;
    case MyNetwork::AUTHSUCCESS:
        showAuthSuccess();
        break;
    default:
        break;
    }

}
void CppServer::onReadyRead()
{
    QTcpSocket *socket=castToQTcpSocket();
    if (!socket)
        return;

    const QByteArray data = socket->readAll();



#ifdef WX_WITH_PROTOBUF
    MyNetwork::Response msg;
    if (msg.ParseFromArray(data.constData(), int(data.size()))) {
        handleMSG(msg);


    } else {
        qDebug() << "[protobuf] 当前数据不是合法 Response（或需要长度分包后再解析）";
    }
#endif


}


void CppServer::onDisconnected()
{
}
