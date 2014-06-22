#include "Mesh.h"

void Mesh::update(float diff) {
    passTime += diff;
    kmVec3 axis;
    kmVec3Fill(&axis, 0, 1, 0);
    kmQuaternion ry;
    kmQuaternionRotationAxis(&ry, &axis, passTime*3.14/2);

    kmQuaternion rx;
    kmVec3Fill(&axis, 1, 0, 0);
    kmQuaternionRotationAxis(&rx, &axis, passTime*3.14/2);

    kmQuaternionMultiply(&rotation, &rx, &ry);
}
