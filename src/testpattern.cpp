#include "testpattern.h"

TestPattern::TestPattern(QObject *parent) : QObject(parent)
  , pattern_running(false)
  , m_rotation(0.0, 0.0, 0.0)
{
    reset_pattern();
}

void TestPattern::center_color(QColor color, QVector3D rotation)
{
    if(pattern_running){
        float yaw = m_rotation.z() + 10.0;
        if(yaw > 360){
            float pitch = m_rotation.x();
            yaw = 0.0;
            pitch += 10.0;
            if(pitch > 90){
                pitch = -90.0;
            }
            m_rotation.setX(pitch);
        }
        m_rotation.setZ(yaw);
        emit set_rotation(m_rotation);
    }
}

void TestPattern::reset_pattern(void)
{
    m_rotation = QVector3D(0.0, 0.0, 0.0);
}

void TestPattern::start_pattern(void)
{
    if(!pattern_running){
        emit set_rotation(m_rotation);
    }
    pattern_running = true;
}

void TestPattern::stop_pattern(void)
{
    pattern_running = false;
}
