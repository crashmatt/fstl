#include <QDebug>
#include <QFileOpenEvent>

#include "app.h"
#include "window.h"

App::App(int& argc, char *argv[]) :
    QApplication(argc, argv), window(new Window())
{
    QCoreApplication::setOrganizationName("crashmatt");
    QCoreApplication::setOrganizationDomain("https://github.com/crashmatt/fstl");
    QCoreApplication::setApplicationName("fstl-antenna");

    window->load_stl("../models/transparent.stl", "mesh", QColor(50,50,100,50));
    window->load_stl("../models/solid.stl", "solid", QColor(200,50,50,200));
    window->load_stl(":gl/sphere.stl", "mesh", QColor(50,200,50,50));

    window->show();
}

App::~App()
{
	delete window;
}

bool App::event(QEvent* e)
{
    if (e->type() == QEvent::FileOpen)
    {
//        window->load_stl(static_cast<QFileOpenEvent*>(e)->file());
        return false;
    }
    else
    {
        return QApplication::event(e);
    }
}
