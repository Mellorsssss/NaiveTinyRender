#ifndef RASTERIZER_H
#define RASTERIZER_H
#include "tgaimage.h"
#include "Triangle.h"
#include "geometry.h"
#include "Shader.h"
#include <iostream>
#include <vector>
#include <omp.h>

enum class MODE
{
    WIREFRAME,
    COLOR,
    GRAY
};

class Rasterizer
{
private:
    TGAImage img_;
    TGAImage texture_;
    std::vector<float> z_buffer;
    int w_;
    int h_;
    MODE draw_mode_;
    Matrix4x4f model_, view_, projection_;
    fragment_shader frag_shader_;

public:
    Rasterizer(int w, int h) : w_(w), h_(h), draw_mode_(MODE::COLOR)
    {
        img_ = TGAImage(w, h, TGAImage::RGB);
        z_buffer.resize(w_ * h_);
        std::fill(z_buffer.begin(), z_buffer.end(), -9999999);

        model_ = Matrix4x4f::Identity();
        view_ = Matrix4x4f::Identity();
        projection_ = Matrix4x4f::Identity();
    }
    void Render(std::vector<Triangle3D> meshs);

    bool LoadTexture(std::string texture_filename);

    inline uint8_t *GetBuffer() { return img_.buffer(); }

    void Output() const;

    void SetMode(const MODE &m) { draw_mode_ = m; }

    void SetModel(const Matrix4x4f &model)
    {
        model_ = model;
    }

    void SetView(const Matrix4x4f &view)
    {
        view_ = view;
    }

    void SetProjection(const Matrix4x4f &proj)
    {
        projection_ = proj;
    }

    void SetFragmentShader(fragment_shader fs)
    {
        frag_shader_ = fs;
    }

    Matrix4x4f GetMVP() const
    {
        return projection_ * view_ * model_;
    }

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