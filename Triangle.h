#ifndef TRIANGLE_H
#define TRIANGLE_H
#include "geometry.h"
#include <vector>

class Triangle3D
{
public:
    Vec3f a, b, c;
    Vec2f uv_[3];

    Triangle3D() : a(), b(), c(), uv_() {}
    Triangle3D(Vec3f x, Vec3f y, Vec3f z) : a(x), b(y), c(z), uv_() {}
    void SetUV(Vec2f uv_coords, int i)
    {
        uv_[i] = uv_coords;
    }
    std::vector<Vec3f> points() const { return std::vector<Vec3f>{a, b, c}; }
    Vec3f operator[](int index) const
    {
        if (index == 0)
            return a;
        if (index == 1)
            return b;
        if (index == 2)
            return c;
        return Vec3f();
    }
};
#endif