#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"

/* obj loader, code from https://github.com/ssloy/tinyrenderer*/

struct face_propertes
{
    int v_ind_;
    int vt_ind_;
    face_propertes(int v_index = 0, int vt_index = 0) : v_ind_(v_index), vt_ind_(vt_index) {}
};

class Model
{
private:
    std::vector<Vec3f> verts_;
    std::vector<Vec2f> textures_;
    std::vector<std::vector<face_propertes>> faces_;

public:
    Model(const char *filename);
    ~Model();
    int nverts() const;
    int nfaces() const;
    int ntex_coords() const;
    Vec3f vert(int i) const;
    Vec2f texture(int i) const;
    std::vector<face_propertes> face(int idx);
};

#endif //__MODEL_H__