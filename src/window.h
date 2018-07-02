#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QActionGroup>
#include <QFileSystemWatcher>
#include <QVector3D>

class Canvas;
class TestPattern;
class DataProcessor;
class RadPatternData;
class ObjectConfig;

class Window : public QMainWindow
{
    Q_OBJECT
public:
    explicit Window(QWidget* parent=0);
    bool load_stl(const QString& filename, const ObjectConfig& config);
    bool load_rad_pattern(const QString& filename, const ObjectConfig& config);    

protected:    
    typedef enum{
        Json,
        BJson
    } SaveFormat;

    QMap<QString, bool> object_visibilities;

    bool save(Window::SaveFormat saveFormat) const;
    void write(QJsonObject &json) const;

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
    void object_visible(QAction* a);
    void add_object(const QString &obj_name);
    void remove_object(const QString &obj_name);

signals:
    void set_object_visible(QString& obj_name, bool visible);

private slots:
    void save_json();
    void save_bsjson();
    void save_antennas();
    void load_antennas();

private:
    QAction* const about_action;
    QAction* const save_action;
    QAction* const load_action;
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
    RadPatternData* rad_patterns;

    QMenu* const visibility;
    QActionGroup* const objects_visibility;
};

#endif // WINDOW_H
