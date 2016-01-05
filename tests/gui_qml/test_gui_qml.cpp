#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>

#include "processmodel.h"
#include "socketunixmodel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    ProcessModel process_model;
    SocketUNIXModel socket_unix_model;

    QQmlApplicationEngine engine;

    QQmlContext* ctxt = engine.rootContext();
    ctxt->setContextProperty("process_model", &process_model);
    ctxt->setContextProperty("socket_unix_model", &socket_unix_model);

    engine.load(QUrl(QStringLiteral("qrc:/test_gui_qml.qml")));



    return app.exec();
}
