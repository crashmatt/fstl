#include <QDebug>
#include <QFileOpenEvent>

#include "app.h"
#include "window.h"

//#define colored

App::App(int& argc, char *argv[]) :
    QApplication(argc, argv), window(new Window())
{
    QCoreApplication::setOrganizationName("crashmatt");
    QCoreApplication::setOrganizationDomain("https://github.com/crashmatt/fstl");
    QCoreApplication::setApplicationName("fstl-antenna");

#ifdef colored
    window->load_stl("../models/solid.stl", "solid", "solid", QColor(200,0,0,200), 0);
    window->load_stl("../models/transparent.stl", "fuselage" "solid", QColor(0,0,250,50), 1);
    window->load_stl(":gl/sphere.stl", "ball", "solid", QColor(50,200,50,50), 2);
#else
    window->load_stl("../models/antenna.stl", "antenna", "solid", QColor(0,255,0,255), 0);
    window->load_stl("../models/solid.stl", "solid", "solid", QColor(0,0,0,250), 1);
    window->load_stl("../models/shadow.stl", "shadow" , "solid", QColor(0,0,0,100), 2);
    window->load_stl("../models/transparent.stl", "fuselage" , "solid", QColor(0,0,0,50), 3);
//    window->load_stl(":gl/sphere.stl", "ball", "solid", QColor(50,200,50,50), 3);
#endif

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
