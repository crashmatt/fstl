#include <QDebug>
#include <QFileOpenEvent>
#include <QDir>

#include "app.h"
#include "window.h"
#include "globject.h"

App::App(int& argc, char *argv[]) :
    QApplication(argc, argv), window(new Window())
{
    QCoreApplication::setOrganizationName("crashmatt");
    QCoreApplication::setOrganizationDomain("https://github.com/crashmatt/fstl");
    QCoreApplication::setApplicationName("fstl-antenna");

    auto working_dir = QDir::currentPath();
    qDebug() << "App Location:" <<  working_dir;
    auto models_dir = QDir(working_dir + "/../fstl_antenna/models");

    auto ant_config = ObjectConfig("antenna", "solid", QColor(0,255,0,255), 0);
    auto antenna_path = models_dir.filePath("antenna.stl");
    Q_ASSERT(QFileInfo::exists(antenna_path));
    window->load_stl(antenna_path, ant_config);

    auto solid_config = ObjectConfig("solid", "solid", QColor(0,0,0,255), 1);
    auto solid_path = models_dir.filePath("solid.stl");
    Q_ASSERT(QFileInfo::exists(solid_path));
    window->load_stl(solid_path, solid_config);

////    window->load_stl("../models/shadow0_25.stl", "shadow0_25" , "solid", QColor(0,0,0,150), 2);
///
    auto shadow0_5_config = ObjectConfig("shadow0_5" , "solid", QColor(0,0,0,100), 3);
    auto shadow0_5_path = models_dir.filePath("shadow0_5.stl");
    Q_ASSERT(QFileInfo::exists(shadow0_5_path));
    window->load_stl(shadow0_5_path, shadow0_5_config);

////    window->load_stl("../models/shadow0_75.stl", "shadow0_75" , "solid", QColor(0,0,0,75), 4);
////    window->load_stl("../models/shadow1_0.stl", "shadow1_0" , "solid", QColor(0,0,0,50), 4);

    auto transp_config = ObjectConfig("fuselage" , "solid", QColor(0,0,0,125), 5);
    auto fuselage_path = models_dir.filePath("transparent.stl");
    Q_ASSERT(QFileInfo::exists(fuselage_path));
    window->load_stl(fuselage_path, transp_config);

    auto mono_config = ObjectConfig("rad_monopole" , "visi", QColor(255,0,0,200), 8);
    auto monopole_path = models_dir.filePath("coax_monopole.csv");
    Q_ASSERT(QFileInfo::exists(monopole_path));
    window->load_rad_pattern(monopole_path, mono_config);

    auto mono_config_15 = ObjectConfig("rad_monopole_15" , "visi", QColor(255,0,0,200), 9);
    auto monopole_path_15 = models_dir.filePath("coax_monopole_15deg.csv");
    Q_ASSERT(QFileInfo::exists(monopole_path_15));
    window->load_rad_pattern(monopole_path_15, mono_config_15);

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
