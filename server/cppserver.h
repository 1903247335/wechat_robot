#ifndef CPPSERVER_H
#define CPPSERVER_H
#include"global.h"
#include <QQmlEngine>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include "message.pb.h"
#include"logging.h"
#pragma comment(lib, "ws2_32.lib")
class CppServer: public QObject
{
    Q_OBJECT

public:

QTcpServer *server;
public:


    explicit CppServer(QObject *parent = nullptr);
    Q_INVOKABLE void startListening();

    void showQRcode(const MyNetwork::Response&);
    void showWaitAuth(const MyNetwork::Response& response);
    void showLoginSuccess();
    void showLoginFail();
    void showAuthSuccess();

    void handleMSG(const  MyNetwork::Response&);

    QTcpSocket *castToQTcpSocket();

public slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();
signals:
    void changeToScanQRcode(QString qrcodeUrl);
    void changeToWaitAuth(QString headUrl);
    void changeToLoginSuccess();
    void changeToLoginFail();
    void changeToAuthSuccess();

};

#endif // CPPSERVER_H
