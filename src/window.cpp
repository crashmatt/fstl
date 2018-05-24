#include <QMenuBar>

#include "window.h"
#include "canvas.h"
#include "loader.h"
#include "testpattern.h"

Window::Window(QWidget *parent) :
    QMainWindow(parent),
    about_action(new QAction("About", this)),
    quit_action(new QAction("Quit", this)),
    start_test(new QAction("Start test", this)),
    pause_test(new QAction("Pause test", this)),
    reset_test(new QAction("Reset test", this)),
    test_pattern(NULL)
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

    test_pattern = new TestPattern(this);

    QObject::connect(canvas, &Canvas::center_color, test_pattern, &TestPattern::center_color, Qt::QueuedConnection);
    QObject::connect(test_pattern, &TestPattern::set_rotation, canvas, &Canvas::set_rotation, Qt::QueuedConnection);
    QObject::connect(test_pattern, &TestPattern::set_obj_pos, canvas, &Canvas::set_object_pos);

    quit_action->setShortcut(QKeySequence::Quit);
    QObject::connect(quit_action, &QAction::triggered,
                     this, &Window::close);

    QObject::connect(about_action, &QAction::triggered,
                     this, &Window::on_about);

    QObject::connect(start_test, &QAction::triggered,
                     test_pattern, &TestPattern::start_pattern);

    QObject::connect(pause_test, &QAction::triggered,
                     test_pattern, &TestPattern::stop_pattern);

    QObject::connect(reset_test, &QAction::triggered,
                     test_pattern, &TestPattern::reset_pattern);

    auto file_menu = menuBar()->addMenu("File");
    file_menu->addAction(quit_action);

    auto test_menu = menuBar()->addMenu("Test");
    test_menu->addAction(start_test);
    test_menu->addAction(pause_test);
    test_menu->addSeparator();
    test_menu->addAction(reset_test);

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


bool Window::load_stl(const QString& filename, const QString& name, const QString& shader_name, const QColor& base_color, int order)
{
    canvas->set_status("Loading " + filename);

    Loader* loader = new Loader(this, filename, name, shader_name, base_color, order);

    connect(loader, &Loader::got_mesh,
            canvas, &Canvas::load_mesh);
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
