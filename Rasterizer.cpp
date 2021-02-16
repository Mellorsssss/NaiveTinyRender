#include "Rasterizer.h"
#include <time.h>
void Rasterizer::Render(std::vector<Triangle3D> meshs)
{
    std::fill(z_buffer.begin(), z_buffer.end(), -9999999.f);
    img_.clear();
    for (auto t : meshs)
    {
        DrawTriangle(t, white);
    }
    img_.flip_vertically();
}

bool Rasterizer::LoadTexture(std::string texture_filename)
{
    bool is_success = texture_.read_tga_file(texture_filename);
    if (is_success)
    {
        texture_.flip_vertically();
    }

    return is_success;
}

void Rasterizer::DrawLine(int x0, int y0, int x1, int y1, const TGAColor &color)
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
            img_.set(cur_x, cur_y, color);
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
            img_.set(cur_x, cur_y, color);
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

void Rasterizer::DrawTriangle(const Triangle3D &t, const TGAColor &color)
{
    Vec2i bbox_min(img_.get_width(), img_.get_height());
    Vec2i bbox_max(-1, -1);
    for (auto point : t.points())
    {
        bbox_min.x = std::min((int)point.x, bbox_min.x);
        bbox_min.y = std::min((int)point.y, bbox_min.y);
        bbox_max.x = std::max((int)point.x, bbox_max.x);
        bbox_max.y = std::max((int)point.y, bbox_max.y);
    }

    // #pragma omp parallel for shared(z_buffer) num_threads(8)
    for (int x = bbox_min.x; x <= bbox_max.x; x++)
    {
        for (int y = bbox_min.y; y <= bbox_max.y; y++)
        {
            Vec3f P(x, y, 0);
            auto barycentric_coords = ComputeBarycentric(t, P);
            if (barycentric_coords.x < 0 || barycentric_coords.y < 0 || barycentric_coords.z < 0)
                continue;

            P.z = BarycentricInterpolation(barycentric_coords, t.a.z, t.b.z, t.c.z);

            Vec2f P_uv(0, 0);
            P_uv = BarycentricInterpolation(barycentric_coords, t.uv_[0], t.uv_[1], t.uv_[2]);
            if (z_buffer[get_index(x, y)] < P.z)
            {
                z_buffer[get_index(x, y)] = P.z;
                float uv_u = std::round(P_uv.x * texture_.get_width());
                float uv_v = std::round(P_uv.y * texture_.get_height());
                img_.set(x, y, texture_.get(uv_u, uv_v));
            }
        }
    }
}
Vec3f Rasterizer::ComputeBarycentric(const Triangle3D &t, Vec3f p)
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
        return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
    }
    return Vec3f(-1, 1, 1);
}

template <class T>
T Rasterizer::BarycentricInterpolation(const Vec3f &barycentric_coords, const T &vert1, const T &vert2, const T &vert3)
{
    return T(vert1 * barycentric_coords[0] + vert2 * barycentric_coords[1] + vert3 * barycentric_coords[2]);
}