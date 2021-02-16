#ifndef RASTERIZER_H
#define RASTERIZER_H
#include "tgaimage.h"
#include "Triangle.h"
#include <iostream>
#include <vector>
#include <omp.h>

class Rasterizer
{
private:
    TGAImage img_;
    TGAImage texture_;
    std::vector<float> z_buffer;
    int w_;
    int h_;

public:
    Rasterizer(int w, int h) : w_(w), h_(h)
    {
        img_ = TGAImage(w, h, TGAImage::RGB);
        z_buffer.resize(w_ * h_);
        std::fill(z_buffer.begin(), z_buffer.end(), -9999999);
    }
    void Render(std::vector<Triangle3D> meshs);

    bool LoadTexture(std::string texture_filename);

    inline uint8_t *GetBuffer() { return img_.buffer(); }

private:
    inline int get_index(int x, int y)
    {
        if (x < 0 || x >= w_ || y < 0 || y >= h_)
            return 0;
        return x + y * w_;
    }

    Vec3f ComputeBarycentric(const Triangle3D &t, Vec3f p);

    template <class T>
    T BarycentricInterpolation(const Vec3f &barycentric_coords, const T &vert1, const T &vert2, const T &vert3);

    void DrawLine(int x0, int y0, int x1, int y1, const TGAColor &color);

    void DrawTriangle(const Triangle3D &t, const TGAColor &color);
};
#endif