#include <vector>
#include <cmath>
#include <iostream>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "Triangle.h"
#include "Rasterizer.h"
#include <opencv2/opencv.hpp>
#include <time.h>
#include <stdio.h>
#include <string>
#include "test.hpp"

const int width = 512;
const int height = 512;
const int depth = 255;
Vec3f camera(0, 0, -2);

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

Matrix4x4f viewport(int x, int y, int w, int h)
{
    auto m = Matrix4x4f::Identity();
    m[0][3] = x + w / 2.f;
    m[1][3] = y + h / 2.f;
    m[2][3] = depth / 2.f;

    m[0][0] = w / 2.f;
    m[1][1] = h / 2.f;
    m[2][2] = depth / 2.f;
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

Matrix4x4f GetViewMatrix(Vec3f eye_pos, Vec3f eye_at, Vec3f eye_up)
{
    Matrix4x4f trans = Translation(-eye_pos[0], -eye_pos[1], -eye_pos[2]);
    Vec3f x_dir = CrossProduct(eye_at, eye_up);
    Vec3f neg_eye_at = eye_at * (-1);
    Matrix4x4f rotate = Matrix4x4f::Identity();
    rotate.SetRow(x_dir.ToVec4(0.f), 0);
    rotate.SetRow(eye_up.ToVec4(0.f), 1);
    rotate.SetRow(neg_eye_at.ToVec4(0.f), 2);
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

int main(int argc, char **argv)
{

    Model *model = NULL;
    Rasterizer *rst = new Rasterizer(width, height);

    Vec3f light_dir = Vec3f(0, 0, 1);

    light_dir.normalize();
    std::cout << light_dir << std::endl;

    Vec3f eye_pos = Vec3f(0, 0, 1.7);
    Vec3f eye_at = Vec3f(0, 0, -1);
    Vec3f eye_up = Vec3f(0, 1, 0);
    Matrix4x4f ModelMatrix = Scale(0.5, 0.5, 0.5);
    Matrix4x4f View = GetViewMatrix(eye_pos, eye_at, eye_up);
    Matrix4x4f Projection = GetPerpectiveMatrix(3.14 * 0.5f, 6 / 8.0, -1.0, -500.0f);
    Matrix4x4f ViewPort = GetViewport(width, height); // viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
    Matrix4x4f MVP = ViewPort * Projection * View * ModelMatrix;

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

    bool dirty = true;
    double start_time_d, end_time_d;
    while (true)
    {

        start_time_d = omp_get_wtime();
        meshs.clear();
        if (dirty)
        {
            View = GetViewMatrix(eye_pos, eye_at, eye_up);
            MVP = ViewPort * Projection * View * ModelMatrix;
            for (int i = 0; i < model->nfaces(); i++)
            {
                auto cur_face = model->face(i);
                Vec3f screen_coords[3];
                Vec3f world_coords[3];
                Vec2f uv_coords[3];
                for (int j = 0; j < 3; j++)
                {
                    world_coords[j] = model->vert(cur_face[j].v_ind_);
                    uv_coords[j] = model->texture(cur_face[j].vt_ind_);
                    screen_coords[j] = Vec4fToVec3f(MVP * world_coords[j].ToVec4(1.f));
                }

                Vec3f n = (world_coords[1] - world_coords[0]) ^ (world_coords[2] - world_coords[0]);
                n.normalize();
                float light_intensity = n * light_dir;
                if (light_intensity > 1e-2)
                {
                    auto t = Triangle3D(screen_coords[0], screen_coords[1], screen_coords[2]);
                    for (int ind = 0; ind < 3; ind++)
                        t.SetUV(uv_coords[ind], ind);
                    meshs.push_back(t);
                }
            }
            rst->Render(meshs);

            cv::Mat image(width, height, CV_8UC3, rst->GetBuffer());
            cv::imshow("image", image);

            /* caculatate the frame per second */
            end_time_d = omp_get_wtime();
            double abs_fps = 1.f / (end_time_d - start_time_d);
            std::cout << " fps :" << abs_fps << "\r" << std::flush;
        }
        int key = cv::waitKey(10);
        dirty = true;
        if (key == 'w')
        {
            dirty = true, eye_pos[2] += 10;
        }

        if (key == 's')
        {
            dirty = true, eye_pos[2] -= 10;
        }

        if (key == 'a')
        {
            dirty = true, eye_pos[0] += 10;
        }

        if (key == 'd')
        {
            dirty = true, eye_pos[0] -= 10;
        }

        if (key == 'q')
        {
            rst->Output();
            break;
        }
    }

    return 0;
}