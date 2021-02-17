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

const int width = 512;
const int height = 512;

int main(int argc, char **argv)
{

    Model *model = NULL;
    Rasterizer *rst = new Rasterizer(width, height);

    Vec3f light_dir = Vec3f(0, 0, 1);

    light_dir.normalize();

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
    std::cout << model->nfaces() << std::endl;
    while (true)
    {

        start_time_d = omp_get_wtime();
        meshs.clear();
        if (dirty)
        {

            for (int i = 0; i < model->nfaces(); i++)
            {
                auto cur_face = model->face(i);
                Vec3f screen_coords[3];
                Vec3f world_coords[3];
                Vec2f uv_coords[3];
                for (int j = 0; j < 3; j++)
                {
                    model->vert(cur_face[j].v_ind_);
                    world_coords[j] = model->vert(cur_face[j].v_ind_);
                    uv_coords[j] = model->texture(cur_face[j].vt_ind_);
                    screen_coords[j] = Vec3f(int((world_coords[j].x + 1.) * width / 100. + .5), int((world_coords[j].y + 1.) * height / 100. + .5), world_coords[j].z);
                }

                Vec3f n = (world_coords[1] - world_coords[0]) ^ (world_coords[2] - world_coords[0]);
                n.normalize();
                float light_intensity = n * light_dir;
                if (light_intensity > 0)
                {
                    auto t = Triangle3D(screen_coords[0], screen_coords[1], screen_coords[2]);
                    for (int ind = 0; ind < 3; ind++)
                        t.SetUV(uv_coords[ind], ind);
                    meshs.push_back(t);
                }
            }
            std::cout << meshs.size() << std::endl;
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
            dirty = true, light_dir[1] += 10;

        if (key == 's')
            dirty = true, light_dir[1] -= 10;

        if (key == 'q')
        {
            rst->Output();
            break;
        }
    }

    return 0;
}