#include "testpattern.h"

TestPattern::TestPattern(QObject *parent) : QObject(parent)
  , pattern_running(false)
{

}

void TestPattern::center_color(QColor color, QVector3D rotation)
{
    float yaw = rotation.z() + 10.0;
    if(yaw > 360){
        float pitch = rotation.x();
        yaw = 0.0;
        pitch += 10.0;
        if(pitch > 90){
            pitch = -90.0;
        }
        rotation.setX(pitch);
    }
    rotation.setZ(yaw);
    emit set_rotation(rotation);
}
