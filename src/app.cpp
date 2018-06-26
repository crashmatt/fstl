#include <QDebug>
#include <QFileOpenEvent>

#include "app.h"
#include "window.h"
#include "globject.h"

App::App(int& argc, char *argv[]) :
    QApplication(argc, argv), window(new Window())
{
    QCoreApplication::setOrganizationName("crashmatt");
    QCoreApplication::setOrganizationDomain("https://github.com/crashmatt/fstl");
    QCoreApplication::setApplicationName("fstl-antenna");

    auto ant_config = ObjectConfig("antenna", "solid", QColor(0,255,0,255), 0);
    window->load_stl("../models/antenna.stl", ant_config);

    auto solid_config = ObjectConfig("solid", "solid", QColor(0,0,0,255), 1);
    window->load_stl("../models/solid.stl", solid_config);

////    window->load_stl("../models/shadow0_25.stl", "shadow0_25" , "solid", QColor(0,0,0,150), 2);
///
    auto shadow0_5_config = ObjectConfig("shadow0_5" , "solid", QColor(0,0,0,100), 3);
    window->load_stl("../models/shadow0_5.stl", shadow0_5_config);

////    window->load_stl("../models/shadow0_75.stl", "shadow0_75" , "solid", QColor(0,0,0,75), 4);
////    window->load_stl("../models/shadow1_0.stl", "shadow1_0" , "solid", QColor(0,0,0,50), 4);

    auto transp_config = ObjectConfig("fuselage" , "solid", QColor(0,0,0,125), 5);
    window->load_stl("../models/transparent.stl", transp_config);

////    window->load_stl(":gl/sphere.stl", "ball", "solid", QColor(50,200,50,50), 3);
    auto mono_config = ObjectConfig("rad_monopole" , "visi", QColor(255,0,0,200), 8);
    window->load_rad_pattern("../models/coax_monopole_15deg.csv", mono_config);

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
