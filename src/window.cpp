#include <QMenuBar>

#include "window.h"
#include "canvas.h"
#include "loader.h"
#include "radpatterndata.h"
#include "radpatternloader.h"
#include "testpattern.h"
#include "dataprocessor.h"
#include "globject.h"

Window::Window(QWidget *parent) :
    QMainWindow(parent),
    about_action(new QAction("About", this)),
    save_action(new QAction("Save", this)),
    quit_action(new QAction("Quit", this)),
    start_test(new QAction("Start test", this)),
    stop_test(new QAction("Stop test", this)),
    reset_test(new QAction("Reset test", this)),
    step_antenna(new QAction("Step antenna", this)),
    test_pattern(NULL),
    reset_rotation(new QAction("Reset rotation", this)),
    fast_mode(new QAction("Fast mode", this)),
    solid_visible(new QAction("Soild", this)),
    transparent_visible(new QAction("Transparent", this)),
    visibility_visible(new QAction("Visibility", this)),
    rad_pattern_visible(new QAction("Rad Pattern", this)),
    visibility(new QMenu("Object visibility", this)),
    objects_visibility(new QActionGroup(this))
{
    setWindowTitle("fstl");
    setAcceptDrops(true);

    QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setStencilBufferSize(8);
	format.setVersion(2, 1);
	format.setProfile(QSurfaceFormat::CoreProfile);

	QSurfaceFormat::setDefaultFormat(format);
	
    canvas = new Canvas(format, this);
    setCentralWidget(canvas);

    rad_patterns = new RadPatternData(this);
    test_pattern = new TestPattern(this, rad_patterns);
    data_processor = new DataProcessor(this);

    QObject::connect(canvas, &Canvas::antenna_visibility, test_pattern, &TestPattern::antenna_visibility);
    QObject::connect(test_pattern, &TestPattern::set_rotation, canvas, &Canvas::set_rotation);
    QObject::connect(test_pattern, &TestPattern::set_obj_pos, canvas, &Canvas::set_object_pos);
    QObject::connect(test_pattern, &TestPattern::set_obj_rotation, canvas, &Canvas::set_object_rot);
    QObject::connect(test_pattern, &TestPattern::set_view_pos, canvas, &Canvas::set_view_pos);
    QObject::connect(test_pattern, &TestPattern::set_zoom, canvas, &Canvas::set_zoom);
    connect(test_pattern, SIGNAL(redraw()), canvas, SLOT(update()));

    connect(test_pattern, &TestPattern::antenna_data, data_processor, &DataProcessor::process_data);
    connect(test_pattern, &TestPattern::delete_object, canvas, &Canvas::delete_globject);

    connect(canvas, &Canvas::loaded_object, this, &Window::add_object);
    connect(canvas, &Canvas::deleted_object, this, &Window::remove_object);

    connect(data_processor, &DataProcessor::built_mesh, canvas, &Canvas::load_mesh);
    connect(data_processor, &DataProcessor::set_obj_pos, canvas, &Canvas::set_object_pos);

    connect(this, &Window::set_object_visible, canvas, &Canvas::set_object_visible);
    connect(test_pattern, &TestPattern::set_object_visible, canvas, &Canvas::set_object_visible);

    QObject::connect(objects_visibility, &QActionGroup::triggered,
                     this, &Window::object_visible);

    quit_action->setShortcut(QKeySequence::Quit);
    QObject::connect(quit_action, &QAction::triggered,
                     this, &Window::close);

    save_action->setShortcut(QKeySequence::Save);
    QObject::connect(save_action, &QAction::triggered,
                     this, &Window::save_antennas);

    QObject::connect(about_action, &QAction::triggered,
                     this, &Window::on_about);

    start_test->setShortcut(QKeySequence(Qt::Key_Space));
    QObject::connect(start_test, &QAction::triggered,
                     test_pattern, &TestPattern::start_pattern);

    QObject::connect(stop_test, &QAction::triggered,
                     test_pattern, &TestPattern::stop_pattern);

    QObject::connect(reset_test, &QAction::triggered,
                     test_pattern, &TestPattern::reset_pattern);

    step_antenna->setShortcut(QKeySequence(Qt::Key_N));
    QObject::connect(step_antenna, &QAction::triggered,
                     test_pattern, &TestPattern::step_antenna_pos);

    reset_rotation->setShortcut(QKeySequence(Qt::Key_R));
    QObject::connect(reset_rotation, &QAction::triggered,
                     canvas, &Canvas::reset_rotation);

    fast_mode->setShortcut(QKeySequence(Qt::Key_F));
    QObject::connect(fast_mode, &QAction::toggled,
                     test_pattern, &TestPattern::set_speed);

    auto file_menu = menuBar()->addMenu("File");
    file_menu->addAction(quit_action);

    fast_mode->setCheckable(true);

    auto test_menu = menuBar()->addMenu("Test");
    test_menu->addAction(start_test);
    test_menu->addAction(stop_test);
    test_menu->addSeparator();
    test_menu->addAction(step_antenna);
    test_menu->addSeparator();
    test_menu->addAction(reset_test);
    test_menu->addSeparator();
    test_menu->addAction(reset_rotation);
    test_menu->addSeparator();
    test_menu->addAction(fast_mode);

    auto view_menu = menuBar()->addMenu("View");

    solid_visible->setShortcut(QKeySequence(Qt::Key_S));
    QObject::connect(solid_visible, &QAction::toggled,
                     this, &Window::solid_visibile);

    transparent_visible->setShortcut(QKeySequence(Qt::Key_T));
    QObject::connect(transparent_visible, &QAction::toggled,
                     this, &Window::transparent_visibile);

    visibility_visible->setShortcut(QKeySequence(Qt::Key_V));
    QObject::connect(visibility_visible, &QAction::toggled,
                     this, &Window::visibility_visibile);

    rad_pattern_visible->setShortcut(QKeySequence(Qt::Key_D));
    QObject::connect(rad_pattern_visible, &QAction::toggled,
                     this, &Window::rad_pattern_visibile);

    for (auto p : {solid_visible, transparent_visible, visibility_visible, rad_pattern_visible})
    {
        p->setCheckable(true);
        p->setChecked(true);
        view_menu->addAction(p);
    }

    menuBar()->addMenu(visibility);

    auto help_menu = menuBar()->addMenu("Help");
    help_menu->addAction(about_action);

    resize(600, 400);
}

void Window::on_about()
{
    QMessageBox::about(this, "",
        "<p align=\"center\"><b>fstl</b></p>"
        "<p>A fast viewer for <code>.stl</code> files.<br>"
        "<a href=\"https://github.com/mkeeter/fstl\""
        "   style=\"color: #93a1a1;\">https://github.com/mkeeter/fstl</a></p>"
        "<p>© 2014-2017 Matthew Keeter<br>"
        "<a href=\"mailto:matt.j.keeter@gmail.com\""
        "   style=\"color: #93a1a1;\">matt.j.keeter@gmail.com</a></p>");
}

void Window::on_bad_stl()
{
    QMessageBox::critical(this, "Error",
                          "<b>Error:</b><br>"
                          "This <code>.stl</code> file is invalid or corrupted.<br>"
                          "Please export it from the original source, verify, and retry.");
}

void Window::on_empty_mesh()
{
    QMessageBox::critical(this, "Error",
                          "<b>Error:</b><br>"
                          "This file is syntactically correct<br>but contains no triangles.");
}

void Window::on_confusing_stl()
{
    QMessageBox::warning(this, "Warning",
                         "<b>Warning:</b><br>"
                         "This <code>.stl</code> file begins with <code>solid </code>but appears to be a binary file.<br>"
                         "<code>fstl</code> loaded it, but other programs may be confused by this file.");
}

void Window::on_missing_file()
{
    QMessageBox::critical(this, "Error",
                          "<b>Error:</b><br>"
                          "The target file is missing.<br>");
}

void Window::solid_visibile(bool visible)
{
    QString name = "solid*";
    emit set_object_visible(name, visible);
    name = "shadow*";
    emit set_object_visible(name, visible);
}

void Window::transparent_visibile(bool visible)
{
    QString name = "fuselage*";
    emit set_object_visible(name, visible);
}

void Window::visibility_visibile(bool visible)
{
    QString name = "ant_vis*";
    emit set_object_visible(name, visible);
}

void Window::rad_pattern_visibile(bool visible)
{
    QString name = "monopole";
    emit set_object_visible(name, visible);
}

void Window::object_visible(QAction* a)
{
    QString obj_name = a->data().toString();
    emit set_object_visible(obj_name , !object_visibilities[obj_name] );
    emit update();
    object_visibilities[obj_name] ^= true;
}


void Window::add_object(const QString &obj_name)
{
    auto list = objects_visibility->actions();
    foreach(QAction* a, list){
        if(a->data().toString() == obj_name)
            return;
    }

    const auto a = new QAction(obj_name, visibility);
    object_visibilities[obj_name] = true;
//    a->setCheckable(true);
//    a->setChecked(true);
    a->setData(obj_name);
    auto next = list.count();
    if(next < 9){
        a->setShortcut(QKeySequence(Qt::Key_0+next));
    } else {
        a->setShortcut(QKeySequence(Qt::Key_A+next-9));
    }
    objects_visibility->addAction(a);
    visibility->addAction(a);
}

void Window::remove_object(const QString &obj_name)
{
    auto actions = visibility->actions();
    foreach(auto action, actions){
        if(action->text() == obj_name){
            visibility->removeAction(action);
            delete action;
        }
    }
}


bool Window::load_stl(const QString& filename, const ObjectConfig& config)
{
    canvas->set_status("Loading " + filename);

    Loader* loader = new Loader(this, filename, config);

    connect(loader, &Loader::got_mesh,
            canvas, &Canvas::load_mesh, Qt::ConnectionType::BlockingQueuedConnection);
    connect(loader, &Loader::error_bad_stl,
              this, &Window::on_bad_stl);
    connect(loader, &Loader::error_empty_mesh,
              this, &Window::on_empty_mesh);
    connect(loader, &Loader::warning_confusing_stl,
              this, &Window::on_confusing_stl);
    connect(loader, &Loader::error_missing_file,
              this, &Window::on_missing_file);

    connect(loader, &Loader::finished,
            loader, &Loader::deleteLater);

    connect(loader, &Loader::finished,
            canvas, &Canvas::clear_status);


    loader->start();
    return true;
}

bool Window::load_rad_pattern(const QString& filename, const ObjectConfig& config)
{
    RadPatternLoader* loader = new RadPatternLoader(this, filename, config);

    connect(loader, &RadPatternLoader::got_mesh,
            canvas, &Canvas::load_mesh, Qt::ConnectionType::BlockingQueuedConnection);

    connect(loader, &RadPatternLoader::got_rad_pattern,
                rad_patterns, &RadPatternData::add_pattern_data, Qt::ConnectionType::BlockingQueuedConnection);

//    connect(loader, &Loader::error_bad_stl,
//              this, &Window::on_bad_stl);
//    connect(loader, &Loader::error_empty_mesh,
//              this, &Window::on_empty_mesh);
//    connect(loader, &Loader::warning_confusing_stl,
//              this, &Window::on_confusing_stl);

    connect(loader, &RadPatternLoader::error_missing_file,
              this, &Window::on_missing_file);

    connect(loader, &RadPatternLoader::finished,
            loader, &Loader::deleteLater);

//    connect(loader, &Loader::finished,
//            canvas, &Canvas::clear_status);
    loader->start();
    return true;
}

void Window::save_json()
{
    save(Json);
}

void Window::save_bsjson()
{
    save(BJson);
}

void Window::save_antennas()
{
    QFile saveFile(QStringLiteral("antennas.dat"));

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return;
    }
//    saveFile << test_pattern;
    saveFile.close();
}

bool Window::save(Window::SaveFormat saveFormat) const
{
    QFile saveFile(saveFormat == Json
        ? QStringLiteral("save.json")
        : QStringLiteral("save.dat"));

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }

    QJsonObject antennasObject;
    write(antennasObject);
    QJsonDocument saveDoc(antennasObject);
    saveFile.write(saveFormat == Json
        ? saveDoc.toJson()
        : saveDoc.toBinaryData());

    return true;
}


void Window::write(QJsonObject &json) const
{
    QJsonObject radpatternsObject;
    rad_patterns->write(radpatternsObject);
    json["rad_patterns"] = radpatternsObject;
}
