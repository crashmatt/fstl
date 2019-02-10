#include <QMenuBar>
#include <QThread>

#include "window.h"
#include "canvas.h"
#include "loader.h"
#include "radio.h"
#include "radpatterndata.h"
#include "radpatternloader.h"
#include "testpattern.h"
#include "dataprocessor.h"
#include "globject.h"
#include "radiosimulation.h"

Window::Window(QWidget *parent) :
    QMainWindow(parent),
    about_action(new QAction("About", this)),
    save_action(new QAction("Save radios", this)),
    load_action(new QAction("Load radios", this)),
    save_config_action(new QAction("Save config", this)),
    load_config_action(new QAction("Load config", this)),
    quit_action(new QAction("Quit", this)),
    start_test(new QAction("Start test", this)),
    stop_test(new QAction("Sto&p test", this)),
    reset_test(new QAction("Reset test", this)),
    step_antenna(new QAction("Step antenna", this)),
    test_pattern(NULL),
    rad_sim(NULL),
    reset_rotation(new QAction("Reset rotation", this)),
    fast_mode(new QAction("Fast mode", this)),
    start_rotations(new QAction("&Start Rotations", this)),
    start_simulation(new QAction("Start Si&mulation", this)),
    solid_visible(new QAction("&Soild", this)),
    transparent_visible(new QAction("&Transparent", this)),
    visibility_visible(new QAction("&Visibility", this)),
    rad_pattern_visible(new QAction("&Rad Pattern", this)),
    effective_visible(new QAction("&Effective", this)),
    visibility(new QMenu("&Object visibility", this)),
    objects_visibility(new QActionGroup(this))
{
    setWindowTitle("Antenna");
    setAcceptDrops(true);

    QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setStencilBufferSize(8);
	format.setVersion(2, 1);
	format.setProfile(QSurfaceFormat::CoreProfile);

	QSurfaceFormat::setDefaultFormat(format);
	
    canvas = new Canvas(format, this);
    setCentralWidget(canvas);

    radios = new Radios(this);
    rad_patterns = new RadPatternData(this);
    test_pattern = new TestPattern(this, radios);
    data_processor = new DataProcessor(this);

    QObject::connect(canvas, &Canvas::antenna_visibility, test_pattern, &TestPattern::antenna_visibility);
    QObject::connect(test_pattern, &TestPattern::set_rotation, canvas, &Canvas::set_rotation);
    QObject::connect(test_pattern, &TestPattern::set_obj_pos, canvas, &Canvas::set_object_pos);
    QObject::connect(test_pattern, &TestPattern::set_obj_rotation, canvas, &Canvas::set_object_rot);
    QObject::connect(test_pattern, &TestPattern::set_view_pos, canvas, &Canvas::set_view_pos);
    QObject::connect(test_pattern, &TestPattern::set_zoom, canvas, &Canvas::set_zoom);
    connect(test_pattern, SIGNAL(redraw()), canvas, SLOT(update()));

    connect(radios, &Radios::radio_data, data_processor, &DataProcessor::process_data);
    connect(test_pattern, &TestPattern::delete_object, canvas, &Canvas::delete_globject);

    connect(canvas, &Canvas::loaded_object, this, &Window::add_object);
    connect(canvas, &Canvas::deleted_object, this, &Window::remove_object);

    connect(data_processor, &DataProcessor::built_mesh, canvas, &Canvas::load_mesh);

    connect(this, &Window::set_object_visible, canvas, &Canvas::set_object_visible);
    connect(test_pattern, &TestPattern::set_object_visible, canvas, &Canvas::set_object_visible);

    connect(test_pattern, &TestPattern::antenna_debug_text, canvas, &Canvas::set_data_strings);

    QObject::connect(objects_visibility, &QActionGroup::triggered,
                     this, &Window::object_visible);

    quit_action->setShortcut(QKeySequence::Quit);
    QObject::connect(quit_action, &QAction::triggered,
                     this, &Window::close);

    save_action->setShortcut(QKeySequence::Save);
    QObject::connect(save_action, &QAction::triggered,
                     this, &Window::save_radios);

    load_action->setShortcut(QKeySequence::Open);
    QObject::connect(load_action, &QAction::triggered,
                     this, &Window::load_radios);

//    save_config_action->setShortcut(QKeySequence::Save);
    QObject::connect(save_config_action, &QAction::triggered,
                     this, &Window::save_config);

//    load_config_action->setShortcut(QKeySequence::Open);
    QObject::connect(load_config_action, &QAction::triggered,
                     this, &Window::load_config);

    QObject::connect(about_action, &QAction::triggered,
                     this, &Window::on_about);

    start_test->setShortcut(QKeySequence(Qt::Key_Space));
    QObject::connect(start_test, &QAction::triggered,
                     test_pattern, &TestPattern::start_pattern);

    stop_test->setShortcut(QKeySequence(Qt::Key_P));
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

    start_rotations->setShortcut(QKeySequence(Qt::Key_S));
    QObject::connect(start_rotations, &QAction::triggered,
                     this, &Window::start_random_rotations);

    start_simulation->setShortcut(QKeySequence(Qt::Key_M));
    QObject::connect(start_simulation, &QAction::triggered,
                     this, &Window::start_radio_simulation);

    fast_mode->setShortcut(QKeySequence(Qt::Key_F));
    QObject::connect(fast_mode, &QAction::toggled,
                     test_pattern, &TestPattern::set_speed);

    auto file_menu = menuBar()->addMenu("File");
    file_menu->addSeparator();
    file_menu->addAction(load_action);
    file_menu->addSeparator();
    file_menu->addAction(save_action);
    file_menu->addSeparator();
    file_menu->addAction(load_config_action);
    file_menu->addSeparator();
    file_menu->addAction(save_config_action);
    file_menu->addSeparator();
    file_menu->addAction(quit_action);

    fast_mode->setCheckable(true);

    auto test_menu = menuBar()->addMenu("Test");
    test_menu->addAction(start_test);
    test_menu->addAction(start_rotations);
    test_menu->addAction(start_simulation);
    test_menu->addAction(stop_test);
    test_menu->addSeparator();
    test_menu->addAction(step_antenna);
    test_menu->addSeparator();
    test_menu->addAction(reset_test);
    test_menu->addSeparator();
    test_menu->addAction(reset_rotation);
    test_menu->addSeparator();
    test_menu->addAction(fast_mode);

    auto view_menu = menuBar()->addMenu("&View");

//    solid_visible->setShortcut(QKeySequence(Qt::Key_S));
    QObject::connect(solid_visible, &QAction::toggled,
                     this, &Window::solid_visibile);

//    transparent_visible->setShortcut(QKeySequence(Qt::Key_T));
    QObject::connect(transparent_visible, &QAction::toggled,
                     this, &Window::transparent_visibile);

//    visibility_visible->setShortcut(QKeySequence(Qt::Key_V));
    QObject::connect(visibility_visible, &QAction::toggled,
                     this, &Window::visibility_visibile);

//    rad_pattern_visible->setShortcut(QKeySequence(Qt::Key_P));
    QObject::connect(rad_pattern_visible, &QAction::toggled,
                     this, &Window::rad_pattern_visibile);

//    effective_visible->setShortcut(QKeySequence(Qt::Key_Shift + Qt::Key_E));
    QObject::connect(effective_visible, &QAction::toggled,
                     this, &Window::effective_visibile);

    for (auto p : {solid_visible, transparent_visible, visibility_visible, rad_pattern_visible, effective_visible})
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

void Window::pattern_loaded()
{
    QObject *obj = QObject::sender();
    RadPatternLoader* loader =  qobject_cast<RadPatternLoader*>(obj);
    Q_ASSERT(loader != NULL);
    auto loaded_filename = loader->filename();
    pending_radpattern_loads.removeAll(loaded_filename);

    if(pending_radpattern_loads.isEmpty()){
        QSettings settings(QString("Radios"), QSettings::IniFormat, this);
        bool loaded = false;
        radios->delete_radios();

        if (settings.contains("last_filepath")){
            QString file = settings.value("last_filepath").toString();

            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "", "Load default",
                                          QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                loaded = load_radios_file(file);
            }
        }

        if(!loaded){
            generate_default();
        }
    }
}

void Window::generate_default()
{
    auto aircraft = new Radio( (QObject*) this, QString("aircraft"), QVector3D(1000,0,0));

    //Antenna just behind cockpit cover
    const QQuaternion cockpit_rot = QQuaternion();
    auto cp_antenna = new Antenna( QVector3D(0.00, 0.35, 0.05)
                                , cockpit_rot
                                , "rad_monopole"
                                , "cockpit"
                                , QColor(0,128,128,120) );
    aircraft->add_antenna(std::move(cp_antenna));

    //Antenna on right side behind wing
//            const QQuaternion rear_right_rot =
//                    QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), -135) *
//                    QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), -90.0);
    auto rr_antenna = new Antenna( QVector3D(0.05, -0.1, 0.0)
                                , QQuaternion()
                                , "rad_monopole"
                                , "rear_right"
                                , QColor(128,128,0,120));
    rr_antenna->addRotation(QVector3D(1,0,0), -90.0);
    rr_antenna->addRotation(QVector3D(0,0,1), -135);
    aircraft->add_antenna(std::move(rr_antenna));

    //Antenna on left side behind wing
//            const QQuaternion rear_left_rot =
//                    QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), 135) *
//                    QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), -90.0);
    auto rl_antenna = new Antenna( QVector3D(-0.05, -0.1, 0.0)
                                , QQuaternion()
                                , "rad_monopole"
                                , "rear_left"
                                , QColor(128,0,128,120));
    rl_antenna->addRotation(QVector3D(1,0,0), -90.0);
    rl_antenna->addRotation(QVector3D(0,0,1), 135);
    aircraft->add_antenna(rl_antenna);

    aircraft->add_object("fuselage");
    aircraft->add_object("solid");
    aircraft->add_object("shadow0_5");

    radios->add_radio(std::move(aircraft));

//            auto controller = new Radio( (QObject*) this, QString("controller"), QVector3D(0,0,0));
//            const QQuaternion controller_rot = QQuaternion();

//            auto cont_vert_antenna = Antenna( QVector3D(0.0, 0.0, 0.0)
//                                              , controller_rot
//                                              , "rad_monopole"
//                                              , "controller_vert"
//                                              , QColor(128,128,128,120));
//            controller->add_antenna(&cont_vert_antenna);
}


void Window::solid_visibile(bool visible)
{
    QString name = "solid*";
    emit set_object_visible(name, visible);
    name = "shadow*";
    emit set_object_visible(name, visible);
    emit update();
}

void Window::transparent_visibile(bool visible)
{
    QString name = "fuselage*";
    emit set_object_visible(name, visible);
    emit update();
}

void Window::visibility_visibile(bool visible)
{
    QString name = "ant_vis*";
    emit set_object_visible(name, visible);
    emit update();
}

void Window::rad_pattern_visibile(bool visible)
{
    QString name = "rad_*";
    emit set_object_visible(name, visible);
    emit update();
}

void Window::effective_visibile(bool visible)
{
    QString name = "ant_eff*";
    emit set_object_visible(name, visible);
    emit update();
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
    pending_radpattern_loads.append(filename);
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
            this, &Window::pattern_loaded, Qt::ConnectionType::BlockingQueuedConnection);

    connect(loader, &RadPatternLoader::finished,
            loader, &Loader::deleteLater);

//    connect(loader, &Loader::finished,
//            canvas, &Canvas::clear_status);
    loader->start();
    return true;
}

void Window::save_radios()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                    "radios.dat",
                                                    tr("Radio data (*.dat)"));
    if(fileName == "") return;

    QFile saveFile(fileName);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return;
    }

    QDataStream out(&saveFile);
    out << *radios;
    saveFile.close();

    QSettings settings(QString("Radios"), QSettings::IniFormat, this);
    settings.setValue("last_filepath", fileName);
}

void Window::load_radios()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "radios.dat",
                                                    tr("Radio data (*.dat)"));
    if(filename == "") return;

    if(load_radios_file(filename)){
        QSettings settings(QString("Radios"), QSettings::IniFormat, this);
        settings.setValue("last_filepath", filename);
    }

}

void Window::save_config()
{
    save(Json);
}

void Window::load_config()
{
    load(Json);
}

void Window::start_random_rotations()
{
    //Hide radiation patterns for better visibility
    QString vis_pattern = "rad_*";
    emit set_object_visible(vis_pattern, false);

    test_pattern->start_rotations(10.0);
}

void Window::start_radio_simulation()
{
    if (rad_sim != NULL){
        if(rad_sim->isFinished()){
            rad_sim->deleteLater();
        } else {
            qDebug("Radio simulation running, can't start another until complete");
            return;
        }
        rad_sim = NULL;
    }
    rad_sim = new RadioSimulation(qobject_cast<QObject*>(this), this->radios, this->test_pattern, "output.csv");
    rad_sim->start();
    QThread::msleep(500);
}

bool Window::load_radios_file(QString &filename)
{
    QFile loadFile(filename);

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open load file.");
        return false;
    }

    radios->delete_radios();
    QDataStream in(&loadFile);
    in >> *radios;
    loadFile.close();
    return true;
}


bool Window::load(Window::SaveFormat saveFormat)
{
    QFile loadFile(saveFormat == Json
        ? QStringLiteral("save.json")
        : QStringLiteral("save.dat"));

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }

    QByteArray saveData = loadFile.readAll();

    QJsonDocument loadDoc(saveFormat == Json
        ? QJsonDocument::fromJson(saveData)
        : QJsonDocument::fromBinaryData(saveData));

    read(loadDoc.object());

    return true;
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
    QJsonObject radiosObject;
    radios->write_config(radiosObject);
    json["config"] = radiosObject;
}


void Window::read(const QJsonObject &json)
{
    QJsonObject radiosObject = json["config"].toObject();
    radios->load_config(radiosObject);
}
