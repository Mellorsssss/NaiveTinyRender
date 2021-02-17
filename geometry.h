#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <assert.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* math lib , code from https://github.com/ssloy/tinyrenderer*/
template <class t>
struct Vec2
{
    union
    {
        struct
        {
            t u, v;
        };
        struct
        {
            t x, y;
        };
        t raw[2];
    };
    Vec2() : u(0), v(0) {}
    Vec2(t _u, t _v) : u(_u), v(_v) {}
    inline Vec2<t> operator+(const Vec2<t> &V) const { return Vec2<t>(u + V.u, v + V.v); }
    inline Vec2<t> operator-(const Vec2<t> &V) const { return Vec2<t>(u - V.u, v - V.v); }
    inline Vec2<t> operator*(float f) const { return Vec2<t>(u * f, v * f); }
    template <class>
    friend std::ostream &operator<<(std::ostream &s, Vec2<t> &v);
};

template <class t>
struct Vec3
{
    union
    {
        struct
        {
            t x, y, z;
        };
        struct
        {
            t ivert, iuv, inorm;
        };
        t raw[3];
    };
    Vec3() : x(0), y(0), z(0) {}
    Vec3(t _x, t _y, t _z) : x(_x), y(_y), z(_z) {}
    inline Vec3<t> operator^(const Vec3<t> &v) const { return Vec3<t>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
    inline Vec3<t> operator+(const Vec3<t> &v) const { return Vec3<t>(x + v.x, y + v.y, z + v.z); }
    inline Vec3<t> operator-(const Vec3<t> &v) const { return Vec3<t>(x - v.x, y - v.y, z - v.z); }
    inline Vec3<t> operator*(float f) const { return Vec3<t>(x * f, y * f, z * f); }
    inline t operator*(const Vec3<t> &v) const { return x * v.x + y * v.y + z * v.z; }
    inline t &operator[](int index) { return raw[index]; }
    inline t operator[](int index) const { return raw[index]; }
    float norm() const { return std::sqrt(x * x + y * y + z * z); }
    Vec3<t> &normalize(t l = 1)
    {
        *this = (*this) * (l / norm());
        return *this;
    }
    template <class>
    friend std::ostream &operator<<(std::ostream &s, Vec3<t> &v);
};

typedef Vec2<float> Vec2f;
typedef Vec2<int> Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;

/*** Vector ***/
template <class T, int l>
class Vec
{
private:
    std::vector<T> arr_;

public:
    Vec()
    {
        arr_.resize(l);
    }

    T operator[](int index) const
    {
        assert(index < l);
        return arr_[index];
    }

    T &operator[](int index)
    {
        assert(index < l);
        return arr_[index];
    }

    friend Vec<T, l> operator+(const Vec<T, l> &v1, const Vec<T, l> &v2);
    friend Vec<T, l> operator-(const Vec<T, l> &v1, const Vec<T, l> &v2);
    friend Vec<T, l> operator*(const Vec<T, l> &v1, const Vec<T, l> &v2);
};

template <class T, int l>
Vec<T, l> operator+(const Vec<T, l> &v1, const Vec<T, l> &v2)
{
    Vec<T, l> res;
    for (int i = 0; i < l; i++)
        res[i] = v1[i] + v2[i];
    return res;
}

template <class T, int l>
Vec<T, l> operator-(const Vec<T, l> &v1, const Vec<T, l> &v2)
{
    Vec<T, l> res;
    for (int i = 0; i < l; i++)
        res[i] = v1[i] - v2[i];
    return res;
}

template <class T, int l>
Vec<T, l> operator*(const Vec<T, l> &v1, const Vec<T, l> &v2)
{
    Vec<T, l> res;
    for (int i = 0; i < l; i++)
        res[i] = v1[i] * v2[i];
    return res;
}

/*** Matrix ***/
template <class T>
class Matrix
{
private:
    std::vector<std::vector<T>> m_;
    int row_num_, col_num_;

public:
    Matrix(int row_num, int col_num) : row_num_(row_num), col_num_(col_num)
    {
        m_.resize(row_num);
        for (auto row : m_)
        {
            row.resize(col_num);
        }
    }

    int GetRowNumber() const { return row_num_; }

    int GetColNumber() const { return col_num_; }

    std::vector<T> operator[](int index) const
    {
        assert(index < row_num_);
        return m_[index];
    }

    std::vector<T> &operator[](int index)
    {
        assert(index < row_num_);
        return m_[index];
    }

    Matrix<T> operator*(const Matrix &M) const
    {
        assert(M.GetRowNumber() == col_num_);
        Matrix<T> res(row_num_, M.col_num_);
        for (int i = 0; i < row_num_; i++)
        {
            for (int j = 0; j < M.col_num_; j++)
            {
                res[i][j] = 0.f;
                for (int k = 0; k < col_num_; k++)
                    res[i][j] += m_[i][k] * M.m_[k][j];
            }
        }
        return res;
    }

    Matrix<T> Identity() const
    {
        Matrix<T> res(row_num_, col_num_);
        for (int i = 0; i < row_num_; i++)
        {
            for (int j = 0; j < col_num_; j++)
            {
                res[i][j] = (i == j);
            }
        }
        return res;
    }

    std::vector<T> GetCol(int col_num) const
    {
        assert(col_num < col_num_);
        std::vector<T> res;
        res.reserve(row_num_);
        for (int i = 0; i < row_num_; i++)
            res.push_back(m_[i][col_num]);
        return res;
    }

    std::vector<T> GetRow(int row_num) const
    {
        assert(row_num < row_num_);
        return m_[row_num_];
    }

    Matrix<T> &SetRow(std::vector<T> row, int row_ind)
    {
        assert(row.size() == col_num_);
        m_[row_ind].clear();
        m_[row_ind] = row;
        return *this;
    }

    Matrix<T> &SetCol(std::vector<T> col, int col_ind)
    {
        assert(col.size() == row_num_);
        for (int i = 0; i < row_num_; i++)
        {
            m_[i][col_ind] = col[i];
        }
        return *this;
    }

    Matrix<T> Transpose() const
    {
        Matrix<T> res(col_num_, row_num_);
        for (int i = 0; i < col_num_; i++)
            res[i] = GetCol(i);
        return res;
    }
};
template <class t>
std::ostream &operator<<(std::ostream &s, Vec2<t> &v)
{
    s << "(" << v.x << ", " << v.y << ")\n";
    return s;
}

template <class t>
std::ostream &operator<<(std::ostream &s, Vec3<t> &v)
{
    s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
    return s;
}
template <class t>
inline Vec3<t> CrossProduct(const Vec3<t> &a, const Vec3<t> &b) { return Vec3<t>(a.y * b.z - b.y * a.z,
                                                                                 b.x * a.z - a.x * b.z, a.x * b.y - a.y * b.x); };

template <class t>
inline t DotProduct(const Vec3<t> &a, const Vec3<t> &b) { return a.x * b.x + a.y * b.y + a.y * b.y; }
#endif //__GEOMETRY_H__