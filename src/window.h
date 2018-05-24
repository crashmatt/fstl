#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QActionGroup>
#include <QFileSystemWatcher>
#include <QVector3D>

class Canvas;
class TestPattern;

class Window : public QMainWindow
{
    Q_OBJECT
public:
    explicit Window(QWidget* parent=0);
    bool load_stl(const QString& filename, const QString& name, const QString& shader_name, const QColor& base_color, int order);

protected:

public slots:
    void on_about();
    void on_bad_stl();
    void on_empty_mesh();
    void on_missing_file();
    void on_confusing_stl();

private slots:


private:
    QAction* const about_action;
    QAction* const quit_action;
    QAction* const start_test;
    QAction* const pause_test;
    QAction* const reset_test;

    Canvas*        canvas;
    TestPattern*   test_pattern;
};

#endif // WINDOW_H
