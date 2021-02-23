#pragma once
#ifndef SHADER_H
#define SHADER_H
#include "geometry.h"
#include "tgaimage.h"
#include <functional>

struct fragment_shader_payload
{
    fragment_shader_payload()
    {
    }

    fragment_shader_payload(const Vec3f &col, const Vec3f &nor, const Vec2f &tc, Texture *tex) : color(col), normal(nor), tex_coords(tc), texture(tex) {}

    Vec3f view_pos;
    Vec3f color;
    Vec3f normal;
    Vec2f tex_coords;
    TGAImage *texture;
};

typedef std::function<TGAColor(fragment_shader_payload)> fragment_shader;

// struct light
// {
//     Vec3f pos_;
//     Vec3f intensity_;
//     void SetPos(const Vec3f &pos)
//     {
//         pos_ = pos;
//     }

//     void SetIntensity(const Vec3f &intensity)
//     {
//         intensity_ = intensity;
//     }
// };

// inline static Vec3f getLightIntensity(const Vec3f &light_intensity, const Vec3f &cof)
// {
//     Vec3f res;
//     for (int i = 0; i < 3; i++)
//     {
//         res[i] = light_intensity[i] * cof[i];
//     }
//     return res;
// }

// Vec3f normal_fragment_shader(fragment_shader_payload payload)
// {
//     Vec3f return_color = (payload.normal.normalize() + Vec3f(1.0f, 1.0f, 1.0f)) / 2.f;
//     return return_color * 255.f;
// }

// Vec3f phong_fragment_shader(fragment_shader_payload payload)
// {
//     // TODO: process on texture

//     Vec3f ka = Vec3f(0.005, 0.005, 0.005);
//     Vec3f kd = payload.color;
//     Vec3f ks = Vec3f(0.7937, 0.7937, 0.7937);

//     light l1;
//     l1.SetPos(Vec3f(-20, 20, 0));
//     l1.SetIntensity(Vec3f(500, 500, 500));

//     // light l2;
//     // l2.SetPos(Vec3f(-20, 20, 0));
//     // l2.SetIntensity(Vec3f(500, 500, 500));

//     std::vector<light> lights = {l1};
//     Vec3f amb_light_intensity(10, 10, 10);
//     Vec3f eye_pos(0, -0.5, 1.7);

//     float p = 150;

//     Vec3f color = payload.color;
//     Vec3f point = payload.view_pos;
//     Vec3f normal = payload.normal;

//     Vec3f result_color(0, 0, 0);
//     Vec3f ambient = getLightIntensity(ka, amb_light_intensity);
//     result_color = result_color + ambient;

//     Vec3f view_vec = eye_pos - point;
//     view_vec = view_vec.normalize();
//     for (auto &light : lights)
//     {
//         float r_square = (light.pos_ - point) * (light.pos_ - point);
//         Vec3f light_vec = light.pos_ - point;
//         light_vec = light_vec.normalize();
//         Vec3f diffuse = getLightIntensity(kd, light.intensity_) / r_square * std::max(0.f, DotProduct(normal, light_vec));
//         Vec3f h = (view_vec + light_vec).normalize();
//         Vec3f specular = getLightIntensity(ks, light.intensity_) / r_square * powf(std::max(0.f, DotProduct(h, normal)), p);
//         result_color = result_color + diffuse + specular;
//     }
//     return result_color;
// }

// Vec3f texture_fragment_shader(fragment_shader_payload payload)
// {
//     float uv_u = std::round(payload.tex_coords[0] * payload.texture.get_width());
//     float uv_v = std::round(payload.tex_coords[1] * payload.texture.get_height());
//     auto tem = payload.texture.get(uv_u, uv_v);
//     Vec3f texture_color = Vec3f(tem[0], tem[1], tem[2]);

//     Vec3f ka = Vec3f(0.005, 0.005, 0.005);
//     Vec3f kd = texture_color / 255.f;
//     Vec3f ks = Vec3f(0.7937, 0.7937, 0.7937);

//     light l1;
//     l1.SetPos(Vec3f(0, 0, 0));
//     l1.SetIntensity(Vec3f(500, 500, 500));

//     std::vector<light> lights = {l1};
//     Vec3f amb_light_intensity(10, 10, 10);
//     Vec3f eye_pos(0, -0.5, 1.7);

//     float p = 150;

//     Vec3f color = texture_color;
//     Vec3f point = payload.view_pos;
//     Vec3f normal = payload.normal;

//     Vec3f result_color(0, 0, 0);
//     Vec3f ambient = getLightIntensity(ka, amb_light_intensity);
//     result_color = result_color + ambient;

//     Vec3f view_vec = eye_pos - point;
//     view_vec = view_vec.normalize();
//     for (auto &light : lights)
//     {
//         float r_square = (light.pos_ - point) * (light.pos_ - point);
//         Vec3f light_vec = light.pos_ - point;
//         light_vec = light_vec.normalize();
//         Vec3f diffuse = getLightIntensity(kd, light.intensity_) / r_square * std::max(0.f, DotProduct(normal, light_vec));
//         Vec3f h = (view_vec + light_vec).normalize();
//         Vec3f specular = getLightIntensity(ks, light.intensity_) / r_square * powf(std::max(0.f, DotProduct(h, normal)), p);
//         result_color = result_color + diffuse + specular;
//     }
//     return result_color;
// }

// Vec3f no_shader(fragment_shader_payload payload)
// {
//     float uv_u = std::round(payload.tex_coords[0] * payload.texture.get_width());
//     float uv_v = std::round(payload.tex_coords[1] * payload.texture.get_height());
//     auto tem = payload.texture.get(uv_u, uv_v);
//     Vec3f texture_color = Vec3f(tem[0], tem[1], tem[2]);
//     return texture_color;
// }
#endif