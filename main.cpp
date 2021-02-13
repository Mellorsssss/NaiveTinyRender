#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
Model *model = NULL;
const int width = 800;
const int height = 800;

void line(int x0, int y0, int x1, int y1, TGAImage &img, TGAColor color)
{
    int absDx = abs(x1 - x0),
        absDy = abs(y1 - y0);
    int px = 2 * absDy - absDx,
        py = 2 * absDx - absDy;
    int dyMinusdx = 2 * absDy - 2 * absDx,
        dxMinusdy = 2 * absDx - 2 * absDy;
    int doubleDy = 2 * absDy;
    int doubleDx = 2 * absDx;
    if (absDy <= absDx)
    {
        int cur_x, cur_y;
        int end_x;
        if (x1 >= x0)
        {
            cur_x = x0,
            cur_y = y0;
            end_x = x1;
        }
        else
        {
            cur_x = x1;
            cur_y = y1;
            end_x = x0;
        }
        while (cur_x <= end_x)
        {
            img.set(cur_y, cur_x, color);
            if (px < 0)
            {
                px += doubleDy;
            }
            else
            {
                px += dyMinusdx;
                if ((x1 < x0 && y1 < y0) || (x1 > x0 && y1 > y0))
                {
                    cur_y++;
                }
                else
                {
                    cur_y--;
                }
            }
            cur_x++;
        }
    }
    else
    {
        int cur_x, cur_y;
        int end_y;
        if (y1 >= y0)
        {
            cur_x = x0,
            cur_y = y0;
            end_y = y1;
        }
        else
        {
            cur_x = x1;
            cur_y = y1;
            end_y = y0;
        }
        while (cur_y <= end_y)
        {
            img.set(cur_y, cur_x, color);
            if (py < 0)
            {
                py += doubleDx;
            }
            else
            {
                py += dxMinusdy;
                if ((x1 < x0 && y1 < y0) || (x1 > x0 && y1 > y0))
                {
                    cur_x++;
                }
                else
                {
                    cur_x--;
                }
            }
            cur_y++;
        }
    }
}
int main(int argc, char **argv)
{
    if (2 == argc)
    {
        model = new Model(argv[1]);
    }
    else
    {
        model = new Model("obj/african_head.obj");
    }

    TGAImage image(width, height, TGAImage::RGB);
    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        for (int j = 0; j < 3; j++)
        {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j + 1) % 3]);
            int x0 = (v0.x + 1.) * width / 2.;
            int y0 = (v0.y + 1.) * height / 2.;
            int x1 = (v1.x + 1.) * width / 2.;
            int y1 = (v1.y + 1.) * height / 2.;
            line(x0, y0, x1, y1, image, white);
        }
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}