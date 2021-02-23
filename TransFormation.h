#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H
#include "geometry.h"

Vec3f Vec4fToVec3f(Vec4f origin)
{
    return Vec3f(origin[0] / origin[3], origin[1] / origin[3], origin[2] / origin[3]);
}

Vec4f Vec3fToVec4f(Vec3f v)
{
    Vec4f m;
    m[0] = v.x;
    m[1] = v.y;
    m[2] = v.z;
    m[3] = 1.f;
    return m;
}

Matrix4x4f Scale(float x, float y, float z)
{
    Matrix4x4f res = Matrix4x4f::Zero();
    res[0][0] = x;
    res[1][1] = y;
    res[2][2] = z;
    res[3][3] = 1.f;
    return res;
}

Matrix4x4f Translation(float dx, float dy, float dz)
{
    Matrix4x4f res = Matrix4x4f::Identity();
    res[0][3] = dx;
    res[1][3] = dy;
    res[2][3] = dz;
    return res;
}

Matrix4x4f RotationByY(float angle)
{
    Matrix4x4f res = Matrix4x4f::Identity();
    res[0][0] = std::cos(angle);
    res[0][2] = std::sin(angle);
    res[2][0] = -res[0][2];
    res[2][2] = res[0][0];
    return res;
}

Matrix4x4f GetViewMatrix(Vec3f eye_pos, Vec3f eye_at, Vec3f eye_up)
{
    Vec3f w = (eye_pos - eye_at).normalize();
    Vec3f u = CrossProduct(eye_up, w).normalize();
    Vec3f v = CrossProduct(w, u);

    Matrix4x4f trans = Translation(-DotProduct(eye_pos, u), -DotProduct(eye_pos, v), -DotProduct(eye_pos, w)); // translate the camera to the origin
    Matrix4x4f rotate = Matrix4x4f::Identity();
    rotate.SetRow(u.ToVec4(0.f), 0);
    rotate.SetRow(v.ToVec4(0.f), 1);
    rotate.SetRow(w.ToVec4(0.f), 2);
    return rotate * trans;
}

Matrix4x4f GetPerpectiveMatrix(float fovY, float aspect, float zNear, float zFar)
{
    // zNear and zFar here should be negtive
    float t = abs(zNear) * tanf(fovY / 2.);
    float r = aspect * t;
    Matrix4x4f pers_to_orth = Matrix4x4f::Zero();
    pers_to_orth[0][0] = zNear;
    pers_to_orth[1][1] = zNear;
    pers_to_orth[2][2] = zNear + zFar;
    pers_to_orth[2][3] = -zNear * zFar;
    pers_to_orth[3][2] = 1.f;

    Matrix4x4f orth = Scale(1. / r, 1. / t, 2. / (zNear - zFar)) * Translation(0, 0, -(zNear + zFar) / 2.);
    return orth * pers_to_orth;
}

Matrix4x4f GetViewport(int width, int height)
{
    Matrix4x4f viewport = Matrix4x4f::Identity();
    viewport[0][0] = 1.f * width / 2;
    viewport[0][3] = 1.f * width / 2;
    viewport[1][1] = 1.f * height / 2;
    viewport[1][3] = 1.f * height / 2;
    return viewport;
}

#endif