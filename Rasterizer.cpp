#include "Rasterizer.h"
#include "utils.h"
#include <time.h>

void Rasterizer::Render(std::vector<Triangle3D> meshs)
{
    int count = 0;
    std::fill(z_buffer.begin(), z_buffer.end(), -9999999.f);
    img_.clear();
#pragma omp parallel for num_threads(4)
    for (int i = 0; i < meshs.size(); i++)
    {
#pragma omp critical
        {
            count++;
            // UpdateProgress(1.0 * count / meshs.size());
        }
        DrawTriangle(meshs[i], white);
    }
    img_.flip_vertically();
}

void Rasterizer::Output() const
{
    img_.write_tga_file("out.tga");
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

    if (draw_mode_ == MODE::WIREFRAME)
    {
        for (int i = 0; i < 3; i++)
        {
            DrawLine(t[i].x, t[i].y, t[(i + 1) % 3].x, t[(i + 1) % 3].y, white);
        }
        return;
    }

    if (draw_mode_ == MODE::COLOR)
    {
        for (int x = bbox_min.x; x <= bbox_max.x; x++)
        {
            for (int y = bbox_min.y; y <= bbox_max.y; y++)
            {
                Vec3f P(x, y, 0);
                auto barycentric_coords = ComputeBarycentric(t, P);
                if (barycentric_coords.x >= 0 && barycentric_coords.y >= 0 && barycentric_coords.z >= 0)
                {
                    P.z = BarycentricInterpolation(barycentric_coords, t.a.z, t.b.z, t.c.z);
                    float w_reciprocal = 1.0 / BarycentricInterpolation(barycentric_coords, t.w_[0], t.w_[1], t.w_[2]);
                    float z_interpolated = BarycentricInterpolation(barycentric_coords, t.a[2] / t.w_[0], t.b[2] / t.w_[1], t.c[2] / t.w_[2]);
                    z_interpolated *= w_reciprocal;
                    if (z_buffer[get_index(x, y)] < z_interpolated)
                    {
                        Vec2f interpolated_uv(0, 0);
                        Vec3f interpolated_normal;
                        Vec3f interpolated_pos;
                        Vec3f interplolated_color;
                        interpolated_uv = BarycentricInterpolation(barycentric_coords, t.uv_[0], t.uv_[1], t.uv_[2]);
                        interpolated_normal = BarycentricInterpolation(barycentric_coords, t.normals_[0], t.normals_[1], t.normals_[2]);
                        interpolated_pos = BarycentricInterpolation(barycentric_coords, t.view_coords_[0], t.view_coords_[1], t.view_coords_[2]);
                        interplolated_color = BarycentricInterpolation(barycentric_coords, t.color_[0], t.color_[1], t.color_[2]);

                        fragment_shader_payload payload(interplolated_color, interpolated_normal.normalize(), interpolated_uv, &texture_);
                        payload.view_pos = interpolated_pos;
                        auto pixel_color = frag_shader_(payload);
                        z_buffer[get_index(x, y)] = z_interpolated;
                        // float uv_u = std::round(interpolated_uv.x * texture_.get_width());
                        // float uv_v = std::round(interpolated_uv.y * texture_.get_height());
                        // img_.set(x, y, texture_.get(uv_u, uv_v));

                        img_.set(x, y, pixel_color);
                    }
                }
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