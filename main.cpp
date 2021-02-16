#include <vector>
#include <cmath>
#include <iostream>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "Triangle.h"
#include "Rasterizer.h"
#include <opencv2/opencv.hpp>

const int width = 800;
const int height = 800;
int main(int argc, char **argv)
{

    Model *model = NULL;
    Rasterizer *rst = new Rasterizer(width, height);
    rst->LoadTexture("../obj/african_head_diffuse.tga");
    Vec3f light_dir = Vec3f(100, 100, 100);

    light_dir.normalize();

    if (2 == argc)
    {
        model = new Model(argv[1]);
    }
    else
    {
        model = new Model("../obj/african_head.obj");
    }

    std::vector<Triangle3D> meshs;
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
            screen_coords[j] = Vec3f(int((world_coords[j].x + 1.) * width / 2. + .5), int((-world_coords[j].y + 1.) * height / 2. + .5), world_coords[j].z);
        }
        Vec3f n = (world_coords[1] - world_coords[0]) ^ (world_coords[2] - world_coords[0]);
        n.normalize();
        float light_intensity = n * light_dir;
        if (light_intensity > 0)
        {
            auto t = Triangle3D(screen_coords[0], screen_coords[1], screen_coords[2]);
            for (int i = 0; i < 3; i++)
                t.SetUV(uv_coords[i], i);
            meshs.push_back(t);
        }
    }
    rst->Render(meshs);

    // cv::Mat img(image.get_height(), image.get_width(), CV_8UC4);
    // memcpy(img.data, image.buffer(), image.get_height() * image.get_width() * sizeof(uint8_t));
    // cv::namedWindow("input", CV_WINDOW_AUTOSIZE); //构建一个窗口，自适应图片大小
    // cv::imshow("input", img);                     //显示

    // while (true)
    // {
    //     int key = cv::waitKey(10);
    //     if (key == 'a')
    //         light_dir.z += 20;
    // }
    return 0;
}