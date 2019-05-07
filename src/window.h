#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QActionGroup>
#include <QFileSystemWatcher>
#include <QVector3D>
#include <QFileInfo>
#include "radio.h"
#include "configeditdialog.h"

class Canvas;
class TestPattern;
class DataProcessor;
class RadPatternData;
class ObjectConfig;
class RadioSimulation;
class Radios;

class Window : public QMainWindow
{
    Q_OBJECT
public:
    explicit Window(QWidget* parent=0);
    bool load_stl(const QString& filename, const ObjectConfig& config);
    bool load_rad_pattern(const QString& filename, ObjectConfig& config);

protected:    
    QMap<QString, bool> object_visibilities;

    bool save(QString filepath) const;
    bool load(QString filepath);
    void write(QJsonObject &json) const;
    void read(const QJsonObject &json);

    void generate_default();

    ConfigEditDialog *m_editDialog;

public slots:
    void on_about();
    void on_bad_stl();
    void on_empty_mesh();
    void on_missing_file();
    void pattern_loaded();
    void on_confusing_stl();
    void solid_visibile(bool visible);
    void transparent_visibile(bool visible);
    void visibility_visibile(bool visible);
    void rad_pattern_visibile(bool visible);
    void effective_visibile(bool visible);
    void object_visible(QAction* a);
    void add_object(ObjectConfig &obj);
    void remove_object(ObjectConfig &obj);
    void show_editor();

signals:
    void set_object_visible(QString& obj_name, bool visible);

private slots:
    void save_radios();
    void load_radios();
    void save_config();
    void load_config();
    void start_random_rotations();
    void start_radio_simulation();

private:
//    bool load_antennas_file(QString &filename);
    bool load_radios_file(QString &filename);

    QAction* const about_action;
    QAction* const save_action;
    QAction* const load_action;
    QAction* const save_config_action;
    QAction* const load_config_action;
    QAction* const quit_action;
    QAction* const start_test;
    QAction* const stop_test;
    QAction* const reset_test;
    QAction* const step_antenna;
    QAction* const reset_rotation;
    QAction* const fast_mode;
    QAction* const start_rotations;
    QAction* const start_simulation;

    QAction* const solid_visible;
    QAction* const transparent_visible;
    QAction* const visibility_visible;
    QAction* const rad_pattern_visible;
    QAction* const effective_visible;
    QAction* const show_editor_action;

    Canvas*        canvas;
    Radios*        radios;
    TestPattern*   test_pattern;
    DataProcessor* data_processor;
    RadPatternData* rad_patterns;
    RadioSimulation* rad_sim;

    QMenu* const visibility;
    QActionGroup* const objects_visibility;

    QStringList pending_radpattern_loads;
};

#endif // WINDOW_H
