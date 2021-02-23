#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), textures_(), faces_()
{
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail())
        return;
    std::string line;
    bool has_vn = false;
    while (!in.eof())
    {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;

        if (!line.compare(0, 2, "v "))
        {
            iss >> trash;
            Vec3f v;
            for (int i = 0; i < 3; i++)
                iss >> v.raw[i];
            verts_.push_back(v);
            continue;
        }

        if (!line.compare(0, 2, "vt"))
        {
            iss >> trash >> trash;
            Vec2f v;
            for (int i = 0; i < 2; i++)
                iss >> v.raw[i];
            textures_.push_back(v);
            continue;
        }

        if (!line.compare(0, 2, "vn"))
        {
            has_vn = true;
            iss >> trash >> trash;
            Vec3f vn;
            for (int i = 0; i < 3; i++)
            {
                iss >> vn[i];
            }
            normals_.push_back(vn);
            continue;
        }

        if (!line.compare(0, 2, "f "))
        {
            std::vector<face_propertes> f;
            int itrash, v_idx, vt_idx, vn_idx;
            iss >> trash;
            if (has_vn)
            {
                while (iss >> v_idx >> trash >> vt_idx >> trash >> vn_idx)
                {
                    v_idx--; // in wavefront obj all indices start at 1, not zero
                    vt_idx--;
                    vn_idx--;
                    f.push_back(face_propertes(v_idx, vt_idx, vn_idx));
                }
            }
            else
            {
                while (iss >> v_idx >> trash >> vt_idx)
                {
                    v_idx--; // in wavefront obj all indices start at 1, not zero
                    vt_idx--;
                    f.push_back(face_propertes(v_idx, vt_idx));
                }
            }

            faces_.push_back(f);
            continue;
        }
    }
    std::cerr << "# v# " << verts_.size() << " vt# " << textures_.size() << " vn# " << normals_.size() << " f# " << faces_.size() << std::endl;
}

Model::~Model()
{
}

int Model::nverts() const
{
    return (int)verts_.size();
}

int Model::nfaces() const
{
    return (int)faces_.size();
}

int Model::ntex_coords() const
{
    return (int)textures_.size();
}

int Model::nnormal_coords() const
{
    return (int)normals_.size();
}

std::vector<face_propertes> Model::face(int idx)
{
    return faces_[idx];
}

Vec3f Model::vert(int i) const
{
    return verts_[i];
}

Vec2f Model::texture(int i) const
{
    return textures_[i];
}

Vec3f Model::normal(int i) const
{
    return normals_[i];
}