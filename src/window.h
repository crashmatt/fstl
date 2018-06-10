#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QActionGroup>
#include <QFileSystemWatcher>
#include <QVector3D>

class Canvas;
class TestPattern;
class DataProcessor;

class Window : public QMainWindow
{
    Q_OBJECT
public:
    explicit Window(QWidget* parent=0);
    bool load_stl(const QString& filename, const QString& name, const QString& shader_name, const QColor& base_color, int order);
    bool load_rad_pattern(const QString& filename, const QString& name, const QString& shader_name, const QColor& base_color, int order);

protected:

public slots:
    void on_about();
    void on_bad_stl();
    void on_empty_mesh();
    void on_missing_file();
    void on_confusing_stl();
    void solid_visibile(bool visible);
    void transparent_visibile(bool visible);
    void visibility_visibile(bool visible);
    void rad_pattern_visibile(bool visible);

signals:
    void set_object_visible(QString& obj_name, bool visible);

private slots:


private:
    QAction* const about_action;
    QAction* const quit_action;
    QAction* const start_test;
    QAction* const stop_test;
    QAction* const reset_test;
    QAction* const step_antenna;
    QAction* const reset_rotation;
    QAction* const fast_mode;

    QAction* const solid_visible;
    QAction* const transparent_visible;
    QAction* const visibility_visible;
    QAction* const rad_pattern_visible;

    Canvas*        canvas;
    TestPattern*   test_pattern;
    DataProcessor* data_processor;
};

#endif // WINDOW_H
