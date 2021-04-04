#include <vector>
#include <cmath>
#include <iostream>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "Triangle.h"
#include "Rasterizer.h"
#include "TransFormation.h"
#include "test.hpp"
#include "utils.h"
#include <opencv2/opencv.hpp>
#include <time.h>
#include <stdio.h>
#include <string>

const int width = 512;
const int height = 512;
const int depth = 255;

Matrix4x4f ModelMatrix; //Scale(1, 1, 1);
Matrix4x4f View;
Matrix4x4f Projection = GetPerpectiveMatrix(3.14 * 0.5f, 1, 1.f, 500.0f);
Matrix4x4f ViewPort = GetViewport(width, height); // viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
Matrix4x4f MVP;
Matrix4x4f NormalInvert;
Matrix4x4f ViewTransformation;

Vec3f light_dir = Vec3f(1, 1, 0.85);
float angle = 0.f;

Vec3f eye_pos = Vec3f(0, -0.5, 1.7);
Vec3f eye_at = Vec3f(0, 0, 0);
Vec3f eye_up = Vec3f(0, 1, 0);

struct light
{
    Vec3f pos_;
    Vec3f intensity_;
    void SetPos(const Vec3f &pos)
    {
        pos_ = pos;
    }

    void SetIntensity(const Vec3f &intensity)
    {
        intensity_ = intensity;
    }
};

inline static Vec3f getLightIntensity(const Vec3f &light_intensity, const Vec3f &cof)
{
    Vec3f res;
    for (int i = 0; i < 3; i++)
    {
        res[i] = light_intensity[i] * cof[i];
    }
    return res;
}

Vec3f normal_fragment_shader(fragment_shader_payload payload)
{
    Vec3f return_color = (payload.normal.normalize() + Vec3f(1.0f, 1.0f, 1.0f)) / 2.f;
    return return_color * 255.f;
}

Vec3f phong_fragment_shader(fragment_shader_payload payload)
{
    // TODO: process on texture

    Vec3f ka = Vec3f(0.005, 0.005, 0.005);
    Vec3f kd = payload.color;
    Vec3f ks = Vec3f(0.7937, 0.7937, 0.7937);

    light l1;
    l1.SetPos(Vec3f(-20, 20, 0));
    l1.SetIntensity(Vec3f(500, 500, 500));

    // light l2;
    // l2.SetPos(Vec3f(-20, 20, 0));
    // l2.SetIntensity(Vec3f(500, 500, 500));

    std::vector<light> lights = {l1};
    Vec3f amb_light_intensity(10, 10, 10);

    float p = 150;

    Vec3f color = payload.color;
    Vec3f point = payload.view_pos;
    Vec3f normal = payload.normal;

    Vec3f result_color(0, 0, 0);
    Vec3f ambient = getLightIntensity(ka, amb_light_intensity);
    result_color = result_color + ambient;

    Vec3f view_vec = eye_pos - point;
    view_vec = view_vec.normalize();
    for (auto &light : lights)
    {
        float r_square = (light.pos_ - point) * (light.pos_ - point);
        Vec3f light_vec = light.pos_ - point;
        light_vec = light_vec.normalize();
        Vec3f diffuse = getLightIntensity(kd, light.intensity_) / r_square * std::max(0.f, DotProduct(normal, light_vec));
        Vec3f h = (view_vec + light_vec).normalize();
        Vec3f specular = getLightIntensity(ks, light.intensity_) / r_square * powf(std::max(0.f, DotProduct(h, normal)), p);
        result_color = result_color + diffuse + specular;
    }
    return result_color;
}

TGAColor phong_fragment_shader_tga(fragment_shader_payload payload)
{
    // TODO: process on texture

    Vec3f ka = Vec3f(0.005, 0.005, 0.005);
    Vec3f kd = payload.color;
    Vec3f ks = Vec3f(0.7937, 0.7937, 0.7937);

    light l1;
    l1.SetPos(Vec3f(-20, 20, 0));
    l1.SetIntensity(Vec3f(500, 500, 500));

    // light l2;
    // l2.SetPos(Vec3f(-20, 20, 0));
    // l2.SetIntensity(Vec3f(500, 500, 500));

    std::vector<light> lights = {l1};
    Vec3f amb_light_intensity(10, 10, 10);

    float p = 150;

    Vec3f color = payload.color;
    Vec3f point = payload.view_pos;
    Vec3f normal = payload.normal;

    Vec3f result_color(0, 0, 0);
    Vec3f ambient = getLightIntensity(ka, amb_light_intensity);
    result_color = result_color + ambient;

    Vec3f view_vec = eye_pos - point;
    view_vec = view_vec.normalize();
    for (auto &light : lights)
    {
        float r_square = (light.pos_ - point) * (light.pos_ - point);
        Vec3f light_vec = light.pos_ - point;
        light_vec = light_vec.normalize();
        Vec3f diffuse = getLightIntensity(kd, light.intensity_) / r_square * std::max(0.f, DotProduct(normal, light_vec));
        Vec3f h = (view_vec + light_vec).normalize();
        Vec3f specular = getLightIntensity(ks, light.intensity_) / r_square * powf(std::max(0.f, DotProduct(h, normal)), p);
        result_color = result_color + diffuse + specular;
    }
    return TGAColor(result_color[0], result_color[1], result_color[2]);
}

Vec3f texture_fragment_shader(fragment_shader_payload payload)
{
    float uv_u = std::round(payload.tex_coords[0] * payload.texture->get_width());
    float uv_v = std::round(payload.tex_coords[1] * payload.texture->get_height());
    auto tem = payload.texture->get(uv_u, uv_v);
    std::cout << tem[0] << " " << tem[1] << " " << tem[2];
    Vec3f texture_color = Vec3f(tem[0], tem[1], tem[2]);

    Vec3f ka = Vec3f(0.005, 0.005, 0.005);
    Vec3f kd = texture_color / 255.f;
    Vec3f ks = Vec3f(0.7937, 0.7937, 0.7937);

    light l1;
    l1.SetPos(Vec3f(0, 0, 0));
    l1.SetIntensity(Vec3f(500, 500, 500));

    std::vector<light> lights = {l1};
    Vec3f amb_light_intensity(10, 10, 10);
    Vec3f eye_pos(0, -0.5, 1.7);

    float p = 150;

    Vec3f color = texture_color;
    Vec3f point = payload.view_pos;
    Vec3f normal = payload.normal;

    Vec3f result_color(0, 0, 0);
    Vec3f ambient = getLightIntensity(ka, amb_light_intensity);
    result_color = result_color + ambient;

    Vec3f view_vec = eye_pos - point;
    view_vec = view_vec.normalize();
    for (auto &light : lights)
    {
        float r_square = (light.pos_ - point) * (light.pos_ - point);
        Vec3f light_vec = light.pos_ - point;
        light_vec = light_vec.normalize();
        Vec3f diffuse = getLightIntensity(kd, light.intensity_) / r_square * std::max(0.f, DotProduct(normal, light_vec));
        Vec3f h = (view_vec + light_vec).normalize();
        Vec3f specular = getLightIntensity(ks, light.intensity_) / r_square * powf(std::max(0.f, DotProduct(h, normal)), p);
        result_color = result_color + diffuse + specular;
    }
    return result_color;
}

float light_in = 10.f;
float specular_ = 0.7;
TGAColor texture_fragment_shader_tga(fragment_shader_payload payload)
{
    float uv_u = std::round(payload.tex_coords[0] * payload.texture->get_width());
    float uv_v = std::round(payload.tex_coords[1] * payload.texture->get_height());
    auto tem = payload.texture->get(uv_u, uv_v);
    Vec3f texture_color = Vec3f(tem[0], tem[1], tem[2]);

    Vec3f ka = Vec3f(0.005, 0.005, 0.005);
    Vec3f kd = texture_color / 255.f;
    // Vec3f kd = payload.color;
    Vec3f ks = Vec3f(specular_, specular_, specular_);

    light l1;
    l1.SetPos(Vec3f(-1, 1, 0));
    l1.SetIntensity(Vec3f(light_in, light_in, light_in));

    // light l2;
    // l1.SetPos(Vec3f(20, 20, 20));
    // l1.SetIntensity(Vec3f(1000, 1000, 1000));

    std::vector<light> lights = {l1};
    Vec3f amb_light_intensity(10, 10, 10);

    float p = 150;

    Vec3f color = payload.color;
    Vec3f point = payload.view_pos;
    Vec3f normal = payload.normal;

    Vec3f result_color(0, 0, 0);
    Vec3f ambient = getLightIntensity(ka, amb_light_intensity);
    // result_color = result_color + ambient;

    Vec3f view_vec = eye_pos - point;
    view_vec = view_vec.normalize();
    for (auto &light : lights)
    {
        float r_square = (light.pos_ - point) * (light.pos_ - point);
        Vec3f light_vec = light.pos_ - point;
        light_vec = light_vec.normalize();
        Vec3f diffuse = getLightIntensity(kd, light.intensity_) / r_square * std::max(0.f, DotProduct(normal, light_vec));
        //std::cout << "diff: " << diffuse;
        Vec3f h = (view_vec + light_vec).normalize();
        Vec3f specular = getLightIntensity(ks, light.intensity_) / r_square * powf(std::max(0.f, DotProduct(h, normal)), p);
        // std::cout << " specular: " << specular << std::endl;
        result_color = result_color + diffuse + specular;
    }
    result_color = result_color * 255.f;
    //std::cout << result_color << std::endl;
    return TGAColor(std::min(255.f, result_color[0]), std::min(255.f, result_color[1]), std::min(255.f, result_color[2]));
}

Vec3f no_shader(fragment_shader_payload payload)
{
    float uv_u = std::round(payload.tex_coords[0] * payload.texture->get_width());
    float uv_v = std::round(payload.tex_coords[1] * payload.texture->get_height());
    auto tem = payload.texture->get(uv_u, uv_v);
    Vec3f texture_color = Vec3f(tem[0], tem[1], tem[2]);
    return texture_color;
}

TGAColor no_shader_tga(fragment_shader_payload payload)
{
    float uv_u = std::round(payload.tex_coords[0] * payload.texture->get_width());
    float uv_v = std::round(payload.tex_coords[1] * payload.texture->get_height());
    return payload.texture->get(uv_u, uv_v);
}

void UpdateTransFormation()
{
    View = GetViewMatrix(eye_pos, eye_at, eye_up);
    ModelMatrix = Scale(1, 1, 1);
    ViewTransformation = View * ModelMatrix;
    MVP = Projection * ViewTransformation;
    NormalInvert = InvertMatrix(ViewTransformation).Transpose();
}

void HandleInput()
{
    int key = cv::waitKey(10);
    if (key == 'w')
    {
        eye_pos[2] += 0.2;
    }

    if (key == 's')
    {
        eye_pos[2] -= 0.2;
    }

    if (key == 'a')
    {
        eye_pos[1] += 0.2;
    }

    if (key == 'd')
    {
        eye_pos[1] -= 0.2;
    }

    if (key == 'q')
    {
        eye_pos[0] += 0.2;
    }

    if (key == 'e')
    {
        eye_pos[0] -= 0.2;
    }

    if (key == 'r')
    {
        angle -= 1;
    }

    if (key == 't')
    {
        angle += 1;
    }

    if (key == 'u')
    {
        light_in += 0.5;
        std::cout << light_in << std::endl;
    }

    if (key == 'i')
    {
        light_in -= 0.5;
        std::cout << light_in << std::endl;
    }

    if (key == 'o')
    {
        specular_ += 0.05;
        std::cout << specular_ << std::endl;
    }

    if (key == 'p')
    {
        specular_ -= 0.05;
        std::cout << specular_ << std::endl;
    }
}

int main(int argc, char **argv)
{

    Model *model = NULL;
    Rasterizer *rst = new Rasterizer(width, height);

    if (3 == argc)
    {
        model = new Model(argv[1]);
        if (rst->LoadTexture(argv[2]))
        {
            std::cout << "Load success!\n";
        }
        else
        {
            std::cout << "Load failure\n";
        }
    }
    else
    {
        model = new Model("../obj/african_head.obj");
        rst->LoadTexture("../obj/african_head_diffuse.tga");
    }

    std::vector<Triangle3D> meshs;
    rst->SetFragmentShader(no_shader_tga); //(texture_fragment_shader); //normal_fragment_shader);
    bool dirty = true;
    double start_time_d, end_time_d;
    while (true)
    {
        UpdateTransFormation();
        start_time_d = omp_get_wtime();
        meshs.clear();
        for (int i = 0; i < model->nfaces(); i++)
        {
            auto cur_face = model->face(i);
            Vec3f screen_coords[3];
            Vec3f viewspace_w;
            Vec3f world_coords[3];
            Vec2f uv_coords[3];
            Vec3f normal_coords[3];
            Vec3f view_coords[3];

            bool discard = false;
            for (int j = 0; j < 3; j++)
            {
                world_coords[j] = model->vert(cur_face[j].v_ind_);
                uv_coords[j] = model->texture(cur_face[j].vt_ind_);
                view_coords[j] = Vec4fToVec3f(ViewTransformation * world_coords[j].ToVec4(1.f));
                normal_coords[j] = Vec4fToVec3f(NormalInvert * model->normal(cur_face[j].vn_ind_).ToVec4(0.f));

                auto tem = MVP * world_coords[j].ToVec4(1.f);
                // std::cout << tem << std::endl;
                if (tem[2] < -tem[3] || tem[2] > tem[3])
                    discard = true;
                if (tem[0] < -tem[3] || tem[0] > tem[3])
                    discard = true;
                if (tem[1] < -tem[3] || tem[1] > tem[3])
                    discard = true;
                if (discard)
                    break;

                auto screen_val = ViewPort * MVP * world_coords[j].ToVec4(1.f);
                viewspace_w[j] = screen_val[3];
                screen_coords[j] = Vec4fToVec3f(screen_val);
            }

            if (discard)
                continue;
            Vec3f n = (world_coords[1] - world_coords[0]) ^ (world_coords[2] - world_coords[0]);
            n.normalize();
            float light_intensity = n * light_dir;
            // if (light_intensity < 0)
            //     continue;
            auto t = Triangle3D(screen_coords[0], screen_coords[1], screen_coords[2], viewspace_w);
            for (int ind = 0; ind < 3; ind++)
                t.SetUV(uv_coords[ind], ind),
                    t.SetNormal(normal_coords[ind], ind),
                    t.SetViewSpace(view_coords[ind], ind),
                    t.SetColor(Vec3f(148, 121.0, 92.0), ind);
            meshs.push_back(t);
        }
        rst->Render(meshs);

        cv::Mat image(width, height, CV_8UC3, rst->GetBuffer());
        cv::imshow("image", image);

        /* caculatate the frame per second */
        end_time_d = omp_get_wtime();
        double abs_fps = 1.f / (end_time_d - start_time_d);
        std::cout << " fps :" << abs_fps << "\r" << std::flush;

        HandleInput();
    }

    return 0;
}