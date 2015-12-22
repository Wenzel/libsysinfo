#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>

#include "processmodel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    ProcessModel model;

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/test_gui_qml.qml")));

    QQmlContext* ctxt = engine.rootContext();
    ctxt->setContextProperty("myModel", &model);

    return app.exec();
}

