#include <vector>
#include <cmath>
#include <iostream>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include <opencv2/opencv.hpp>

Model *model = NULL;
const int width = 800;
const int height = 800;

struct Triangle3D
{

    Vec3f a, b, c;

    Triangle3D(Vec3f x, Vec3f y, Vec3f z) : a(x), b(y), c(z) {}
    std::vector<Vec3f> points() const { return std::vector<Vec3f>{a, b, c}; }
    Vec3f operator[](int index) const
    {
        if (index == 0)
            return a;
        if (index == 1)
            return b;
        if (index == 2)
            return c;
    }
};

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

void triangle(Vec2i a, Vec2i b, Vec2i c, TGAImage &img, TGAColor color)
{
    if (a.y > b.y)
        std::swap(a, b);
    if (a.y > c.y)
        std::swap(a, c);
    if (b.y > c.y)
        std::swap(b, c);

    int total_height = c.y - a.y;
    int bottom_seg_height = b.y - a.y;
    int top_seg_height = c.y - b.y;
    for (int y = a.y; y <= c.y; y++)
    {
        bool is_draw_bottom = (y <= b.y);
        float alpha = (float)1.0 / total_height;
        float beta = is_draw_bottom ? (float)1.0 / bottom_seg_height : (float)1.0 / top_seg_height;
        int x1 = a.x + alpha * (y - a.y) * (c.x - a.x);
        int x2 = is_draw_bottom ? (int)a.x + beta * (y - a.y) * (b.x - a.x) : (int)b.x + beta * (y - b.y) * (c.x - b.x);
        if (x1 > x2)
            std::swap(x1, x2);
        for (int i = x1; i <= x2; i++)
        {
            img.set(i, y, color);
        }
    }
}

void base_triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color)
{
    if (t0.y == t1.y && t0.y == t2.y)
        return; // I dont care about degenerate triangles
    // sort the vertices, t0, t1, t2 lower−to−upper (bubblesort yay!)
    if (t0.y > t1.y)
        std::swap(t0, t1);
    if (t0.y > t2.y)
        std::swap(t0, t2);
    if (t1.y > t2.y)
        std::swap(t1, t2);
    int total_height = t2.y - t0.y;
    for (int i = 0; i < total_height; i++)
    {
        bool second_half = i > t1.y - t0.y || t1.y == t0.y;
        int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
        float alpha = (float)i / total_height;
        float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height; // be careful: with above conditions no division by zero here
        Vec2i A = t0 + (t2 - t0) * alpha;
        Vec2i B = second_half ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;
        if (A.x > B.x)
            std::swap(A, B);
        for (int j = A.x; j <= B.x; j++)
        {
            image.set(j, t0.y + i, color); // attention, due to int casts t0.y+i != A.y
        }
    }
}

Vec3f ComputeBarycentric(const Triangle3D &t, Vec3f p)
{
    Vec3f edge_vec[2];
    for (int i = 0; i < 2; i++)
    {
        edge_vec[i][0] = t.c[i] - t.a[i];
        edge_vec[i][1] = t.b[i] - t.a[i];
        edge_vec[i][2] = t.a[i] - p[i];
    }

    Vec3f u = edge_vec[0] ^ edge_vec[1];
    if (std::abs(u.z) > 1e-2)
    {
        return Vec3f(1 - (u.x + u.y) / u.z, u.x / u.z, u.y / u.z);
    }
    return Vec3f(-1, 1, 1);
}

int get_index(const int &x, const int &y, const int &scene_width)
{
    return x + y * scene_width;
}

void adv_triangle(const Triangle3D &t, float *z_buffer, TGAImage &img, const TGAColor &color)
{
    Vec2i bbox_min(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
    Vec2i bbox_max(-1, -1);
    for (auto point : t.points())
    {
        bbox_min.x = std::min((int)point.x, bbox_min.x);
        bbox_min.y = std::min((int)point.y, bbox_min.y);
        bbox_max.x = std::max((int)point.x, bbox_max.x);
        bbox_max.y = std::max((int)point.y, bbox_max.y);
    }

    std::cout << "max" << bbox_max << std::endl;
    std::cout << "min" << bbox_min << std::endl;
    for (int x = bbox_min.x; x <= bbox_max.x; x++)
    {
        for (int y = bbox_min.y; y <= bbox_max.y; y++)
        {
            Vec3f P(x, y, 0);
            auto barycentric_coords = ComputeBarycentric(t, P);
            if (barycentric_coords.x < 0 || barycentric_coords.y < 0 || barycentric_coords.z < 0)
                continue;
            for (int i = 0; i < 3; i++)
                P.z += barycentric_coords[i] * t[i].z;
            if (z_buffer[get_index(x, y, width)] < P.z)
            {
                z_buffer[get_index(x, y, width)] = P.z;
                img.set(x, y, color);
            }
        }
    }
}

Vec2i GetRandomPoint()
{
    return Vec2i(rand() % width, rand() % height);
}

int main(int argc, char **argv)
{
    Vec3f light_dir = Vec3f(0, 0, 100);
    light_dir.normalize();
    if (2 == argc)
    {
        model = new Model(argv[1]);
    }
    else
    {
        model = new Model("../obj/african_head.obj");
    }

    TGAImage image(width, height, TGAImage::RGB);

    float z_buffer[width * height];
    for (int i = 0; i < width * height; i++)
    {
        z_buffer[i] = -std::numeric_limits<float>::max();
    }

    for (int i = 0; i < model->nfaces(); i++)
    {
        std::cout << i << std::endl;
        auto cur_face = model->face(i);
        Vec3f screen_coords[3];
        Vec3f world_coords[3];
        for (int j = 0; j < 3; j++)
        {
            world_coords[j] = model->vert(cur_face[j]);
            screen_coords[j] = Vec3f(int((world_coords[j].x + 1.) * width / 2. + .5), int((-world_coords[j].y + 1.) * height / 2. + .5), world_coords[j].z);
        }
        Vec3f n = (world_coords[1] - world_coords[0]) ^ (world_coords[2] - world_coords[0]);
        n.normalize();
        float intensity = n * light_dir;
        if (intensity > 0)
        {
            adv_triangle(Triangle3D(screen_coords[0], screen_coords[1], screen_coords[2]), z_buffer, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
        }
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");

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