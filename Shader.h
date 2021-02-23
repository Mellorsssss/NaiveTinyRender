#ifndef SHADER_H
#define SHADER_H
#include "geometry.h"
#include "tgaimage.h"
#include <functional>

struct fragment_shader_payload
{
    fragment_shader_payload()
    {
        texture = nullptr;
    }

    fragment_shader_payload(const Vec3f &col, const Vec3f &nor, const Vec3f &tc, Texture *tex) : color(col), normal(nor), tex_coords(tc), texture(tex) {}

    Vec3f view_pos;
    Vec3f color;
    Vec3f normal;
    Vec3f tex_coords;
    Texture *texture;
};

typedef std::function<Vec3f(fragment_shader_payload)> fragment_shader;
#endif