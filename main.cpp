#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml/qqml.h>
#include <QQmlEngine>
#include <QJSEngine>
#include "server/cppserver.h"


#include "modules/searcher.h"
#include "modules/injector.h"
#include "modules/starter.h"


#include <QQuickStyle>


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQuickStyle::setStyle("Material");
    qputenv("QT_QUICK_CONTROLS_MATERIAL_THEME", "Dark");    // 开启深色模式
    qputenv("QT_QUICK_CONTROLS_MATERIAL_ACCENT", "Teal");   // 设置强调色（如青色）


    //init---------------------
    QQmlApplicationEngine engine;
    CppServer server;
    server.startListening();

    //----------------------------


    // register--------------
    Searcher searcher;
    Injector injector;
    Starter starter;
    qmlRegisterSingletonInstance("wx.module.Searcher", 1, 0, "Searcher", &searcher);
    qmlRegisterSingletonInstance("wx.module.Injector", 1, 0, "Injector",&injector);
    qmlRegisterSingletonInstance("wx.module.Starter", 1, 0, "Starter",&starter);
    qmlRegisterSingletonInstance("wx.module.Server", 1, 0, "Server",&server);

    //----------------------
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("wx", "Main");

    return app.exec();
}
