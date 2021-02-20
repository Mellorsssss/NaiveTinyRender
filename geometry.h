#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <assert.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* math lib , code from https://github.com/ssloy/tinyrenderer*/
// template <class t>
// struct Vec2
// {
//     union
//     {
//         struct
//         {
//             t u, v;
//         };
//         struct
//         {
//             t x, y;
//         };
//         t raw[2];
//     };
//     Vec2() : u(0), v(0) {}
//     Vec2(t _u, t _v) : u(_u), v(_v) {}
//     inline Vec2<t> operator+(const Vec2<t> &V) const { return Vec2<t>(u + V.u, v + V.v); }
//     inline Vec2<t> operator-(const Vec2<t> &V) const { return Vec2<t>(u - V.u, v - V.v); }
//     inline Vec2<t> operator*(float f) const { return Vec2<t>(u * f, v * f); }
//     template <class>
//     friend std::ostream &operator<<(std::ostream &s, Vec2<t> &v);
// };

// template <class t>
// struct Vec3
// {
//     union
//     {
//         struct
//         {
//             t x, y, z;
//         };
//         struct
//         {
//             t ivert, iuv, inorm;
//         };
//         t raw[3];
//     };
//     Vec3() : x(0), y(0), z(0) {}
//     Vec3(t _x, t _y, t _z) : x(_x), y(_y), z(_z) {}
//     inline Vec3<t> operator^(const Vec3<t> &v) const { return Vec3<t>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
//     inline Vec3<t> operator+(const Vec3<t> &v) const { return Vec3<t>(x + v.x, y + v.y, z + v.z); }
//     inline Vec3<t> operator-(const Vec3<t> &v) const { return Vec3<t>(x - v.x, y - v.y, z - v.z); }
//     inline Vec3<t> operator*(float f) const { return Vec3<t>(x * f, y * f, z * f); }
//     inline t operator*(const Vec3<t> &v) const { return x * v.x + y * v.y + z * v.z; }
//     inline t &operator[](int index) { return raw[index]; }
//     inline t operator[](int index) const { return raw[index]; }
//     float norm() const { return std::sqrt(x * x + y * y + z * z); }
//     Vec3<t> &normalize(t l = 1)
//     {
//         *this = (*this) * (l / norm());
//         return *this;
//     }
//     template <class>
//     friend std::ostream &operator<<(std::ostream &s, Vec3<t> &v);
// };

// typedef Vec2<float> Vec2f;
// typedef Vec2<int> Vec2i;
// typedef Vec3<float> Vec3f;
// typedef Vec3<int> Vec3i;

/*** Vector ***/
/* forward declarations */
// template <class T, int l>
// class Vec;

// template <class T, int l>
// Vec<T, l> operator+(const Vec<T, l> &v1, const Vec<T, l> &v2);

// template <class T, int l>
// Vec<T, l> operator-(const Vec<T, l> &v1, const Vec<T, l> &v2);

// template <class T, int l>
// Vec<T, l> operator*(const Vec<T, l> &v1, const Vec<T, l> &v2);

// template <class T, int l>
// std::ostream &operator<<(std::ostream &s, const Vec<T, l> &v);
template <class T, int l>
class Vec
{
private:
    T arr_[l];

public:
    T operator[](int index) const
    {
        assert(index >= 0 && index < l);
        return arr_[index];
    }

    T &operator[](int index)
    {
        assert(index >= 0 && index < l);
        return arr_[index];
    }

    T norm() const
    {
        T sum = 0;
        for (int i = 0; i < l; i++)
        {
            sum += arr_[i] * arr_[i];
        }
        return std::sqrt(sum);
    }

    Vec<T, l> &normalize(T length = 1)
    {
        *this = (*this) * (length / norm());
        return *this;
    }
    Vec<T, l> operator*(T x) const
    {
        Vec<T, l> res;
        for (int i = 0; i < l; i++)
            res[i] = arr_[i] * x;
        return res;
    }
    template <class, int>
    friend Vec<T, l> operator+(const Vec<T, l> &v1, const Vec<T, l> &v2);
    template <class, int>
    friend Vec<T, l> operator-(const Vec<T, l> &v1, const Vec<T, l> &v2);
    template <class, int>
    friend Vec<T, l> operator*(const Vec<T, l> &v1, const Vec<T, l> &v2);
    template <class, int>
    friend Vec<T, l> operator*(T val, const Vec<T, l> &v);
    template <class, int>
    friend std::ostream &operator<<(std::ostream &s, const Vec<T, l> &v);
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

template <class T, int l>
Vec<T, l> operator*(T val, const Vec<T, l> &v)
{
    return (v * val);
}

template <class T, int l>
std::ostream &operator<<(std::ostream &s, const Vec<T, l> &v)
{
    s << "(";
    for (int i = 0; i < l; i++)
    {
        if (i)
            s << ", ";
        s << v[i];
    }
    s << ")\n";
    return s;
}

template <class T>
class Vec<T, 3>
{
public:
    union
    {
        struct
        {
            T x, y, z;
        };
        struct
        {
            T ivert, iuv, inorm;
        };
        T raw[3];
    };
    Vec<T, 3>() : x(0), y(0), z(0) {}
    Vec<T, 3>(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
    inline Vec<T, 3> operator^(const Vec<T, 3> &v) const { return Vec<T, 3>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
    inline Vec<T, 3> operator+(const Vec<T, 3> &v) const { return Vec<T, 3>(x + v.x, y + v.y, z + v.z); }
    inline Vec<T, 3> operator-(const Vec<T, 3> &v) const { return Vec<T, 3>(x - v.x, y - v.y, z - v.z); }
    inline Vec<T, 3> operator*(float f) const { return Vec<T, 3>(x * f, y * f, z * f); }
    inline T operator*(const Vec<T, 3> &v) const { return x * v.x + y * v.y + z * v.z; }
    inline T &operator[](int index) { return raw[index]; }
    inline T operator[](int index) const { return raw[index]; }
    T norm() const
    {
        std::sqrt(x * x + y * y + z * z);
    }

    Vec<T, 3> &normalize(T length = 1)
    {
        *this = (*this) * (length / norm());
        return *this;
    }

    Vec<T, 4> ToVec4(T fill = 1)
    {
        Vec<T, 4> res;
        for (int i = 0; i < 3; i++)
            res[i] = raw[i];
        res[3] = fill;
        return res;
    }

    template <class>
    friend Vec<T, 3> operator*(const Vec<T, 3> &v1, const Vec<T, 3> &v2);
    template <class>
    friend Vec<T, 3> operator*(T val, const Vec<T, 3> &v);
    template <class>
    friend std::ostream &operator<<(std::ostream &s, const Vec<T, 3> &v);
};

template <class T>
class Vec<T, 2>
{
public:
    union
    {
        struct
        {
            T u, v;
        };
        struct
        {
            T x, y;
        };
        T raw[2];
    };
    Vec<T, 2>() : u(0), v(0) {}
    Vec<T, 2>(T _u, T _v) : u(_u), v(_v) {}
    inline Vec<T, 2> operator+(const Vec<T, 2> &V) const { return Vec<T, 2>(u + V.u, v + V.v); }
    inline Vec<T, 2> operator-(const Vec<T, 2> &V) const { return Vec<T, 2>(u - V.u, v - V.v); }
    inline Vec<T, 2> operator*(float f) const { return Vec<T, 2>(u * f, v * f); }
    inline T &operator[](int index) { return raw[index]; }
    inline T operator[](int index) const { return raw[index]; }
    T norm() const
    {
        std::sqrt(x * x + y * y);
    }

    Vec<T, 3> &normalize(T length = 1)
    {
        *this = (*this) * (length / norm());
        return *this;
    }
    template <class>
    friend Vec<T, 2> operator*(const Vec<T, 2> &v1, const Vec<T, 2> &v2);
    template <class>
    friend Vec<T, 2> operator*(T val, const Vec<T, 2> &v);
    template <class>
    friend std::ostream &operator<<(std::ostream &s, const Vec<T, 2> &v);
};

typedef Vec<int, 2> Vec2i;
typedef Vec<int, 3> Vec3i;
typedef Vec<int, 4> Vec4i;
typedef Vec<float, 2> Vec2f;
typedef Vec<float, 3> Vec3f;
typedef Vec<float, 4> Vec4f;
/*** Matrix ***/
template <int ROW, int COL, class T>
class Matrix
{
private:
    T m_[ROW][COL];

public:
    Matrix()
    {
        for (int i = 0; i < ROW; i++)
        {
            for (int j = 0; j < COL; j++)
            {
                m_[i][j] = 0;
            }
        }
    }

    Matrix(const Matrix<ROW, COL, T> &m)
    {
        for (int i = 0; i < ROW; i++)
        {
            for (int j = 0; j < COL; j++)
            {
                m_[i][j] = m[i][j];
            }
        }
    }

    int GetRowNumber() const { return ROW; }

    int GetColNumber() const { return COL; }

    const T *operator[](int index) const
    {
        assert(0 <= index < ROW);
        return m_[index];
    }

    T *operator[](int index)
    {
        assert(0 <= index < ROW);
        return m_[index];
    }

    void operator=(const Matrix<ROW, COL, T> &m)
    {

        for (int i = 0; i < ROW; i++)
        {
            for (int j = 0; j < COL; j++)
            {
                m_[i][j] = m[i][j];
            }
        }
    }

    template <int NEWCOL>
    Matrix<ROW, COL, T> operator*(const Matrix<COL, NEWCOL, T> &M) const
    {
        Matrix<ROW, NEWCOL, T> res;
        for (int i = 0; i < ROW; i++)
        {
            for (int j = 0; j < NEWCOL; j++)
            {
                res[i][j] = 0.f;
                for (int k = 0; k < COL; k++)
                    res[i][j] += m_[i][k] * M[k][j];
            }
        }
        return res;
    }

    Matrix<ROW, COL, T> operator+(const Matrix<ROW, COL, T> &M) const
    {
        Matrix<ROW, COL, T> res;
        for (int i = 0; i < ROW; i++)
        {
            for (int j = 0; j < COL; j++)
            {
                res[i][j] = m_[i][j] + M.m_[i][j];
            }
        }
        return res;
    }

    Matrix<ROW, COL, T> operator-(const Matrix<ROW, COL, T> &M) const
    {
        Matrix<ROW, COL, T> res;
        for (int i = 0; i < ROW; i++)
        {
            for (int j = 0; j < COL; j++)
            {
                res[i][j] = m_[i][j] - M.m_[i][j];
            }
        }
        return res;
    }

    bool operator==(const Matrix<ROW, COL, T> &M)
    {
        for (int i = 0; i < ROW; i++)
        {
            for (int j = 0; j < COL; j++)
            {
                if (m_[i][j] != M[i][j])
                    return false;
            }
        }
        return true;
    }

    bool operator!=(const Matrix<ROW, COL, T> &M)
    {
        return !(*this == M);
    }

    Vec<T, ROW> GetCol(int col_num) const
    {
        assert(0 <= col_num < COL);
        Vec<T, ROW> res;
        for (int i = 0; i < ROW; i++)
            res[i] = m_[i][col_num];
        return res;
    }

    Vec<T, COL> GetRow(int row_num) const
    {
        assert(0 <= row_num < ROW);
        Vec<T, COL> res;
        for (int i = 0; i < COL; i++)
            res[i] = m_[row_num][i];
        return res;
    }

    Matrix<ROW, COL, T> &SetRow(Vec<T, COL> row, int row_ind)
    {
        assert(0 <= row_ind < ROW);
        for (int i = 0; i < COL; i++)
        {
            m_[row_ind][i] = row[i];
        }
        return *this;
    }

    Matrix<ROW, COL, T> &SetCol(Vec<T, ROW> col, int col_ind)
    {
        assert(0 <= col_ind < COL);
        for (int i = 0; i < ROW; i++)
        {
            m_[i][col_ind] = col[i];
        }
        return *this;
    }

    Matrix<COL, ROW, T> Transpose() const
    {
        Matrix<COL, ROW, T> res;
        for (int i = 0; i < COL; i++)
            res.SetRow(this->GetCol(i), i);
        return res;
    }

    template <int, int, class>
    friend std::ostream &operator<<(std::ostream &s, const Matrix<ROW, COL, T> &m);

    Matrix<ROW, COL, T> operator*(T x) const
    {
        Matrix<ROW, COL, T> res;
        for (int i = 0; i < ROW; i++)
        {
            for (int j = 0; j < COL; j++)
            {
                res[i][j] = m_[i][j] * x;
            }
        }
        return res;
    }

    friend Matrix<ROW, COL, T> operator*<>(T x, const Matrix<ROW, COL, T> &M);

    Matrix<ROW, COL, T> operator/(T x) const
    {
        Matrix<ROW, COL, T> res;
        for (int i = 0; i < ROW; i++)
        {
            for (int j = 0; j < COL; j++)
            {
                res[i][j] = m_[i][j] / x;
            }
        }
        return res;
    }

    template <int, int, class>
    friend Matrix<ROW, COL, T> operator/(T x, const Matrix<ROW, COL, T> &M);

    Vec<T, ROW> operator*(const Vec<T, COL> &v)
    {
        Vec<T, ROW> res;
        for (int i = 0; i < ROW; i++)
        {
            Vec<T, COL> cur_row = this->GetRow(i);
            res[i] = DotProduct(cur_row, v);
        }
        return res;
    }

    static Matrix<ROW, COL, T> Identity()
    {
        Matrix<ROW, COL, T> res;
        for (int i = 0; i < ROW; i++)
        {
            for (int j = 0; j < COL; j++)
            {
                res.m_[i][j] = (i == j);
            }
        }
        return res;
    }

    static Matrix<ROW, COL, T> Zero()
    {
        Matrix<ROW, COL, T> res;
        for (int i = 0; i < ROW; i++)
        {
            for (int j = 0; j < COL; j++)
            {
                res.m_[i][j] = (T)0;
            }
        }
        return res;
    }

    inline Matrix<ROW - 1, COL - 1, T> GetMinor(int row_ind, int col_ind) const
    {
        assert(row_ind >= 0 && col_ind >= 0 && row_ind < ROW && col_ind < COL);

        Matrix<ROW - 1, COL - 1, T> res;
        for (int i = 0; i < ROW - 1; i++)
        {
            for (int j = 0; j < COL - 1; j++)
            {
                int row_val = (i < row_ind) ? i : i + 1;
                int col_val = (j < col_ind) ? j : j + 1;
                res[i][j] = m_[row_val][col_val];
            }
        }
        return res;
    }
};

template <int ROW, int COL, class T>
std::ostream &operator<<(std::ostream &s, const Matrix<ROW, COL, T> &m)
{
    for (int i = 0; i < m.GetRowNumber(); i++)
    {
        s << "[ ";
        for (int j = 0; j < m.GetColNumber(); j++)
        {
            s << m[i][j] << " ";
        }
        s << "]\n";
    }
    return s;
}

template <int ROW, int COL, class T>
Matrix<ROW, COL, T> operator*(T x, const Matrix<ROW, COL, T> &M)
{
    return (M * x);
}

template <int ROW, int COL, class T>
Matrix<ROW, COL, T> operator/(T x, const Matrix<ROW, COL, T> &M)
{
    Matrix<ROW, COL, T> res;
    for (int i = 0; i < ROW; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            res[i][j] = x / M[i][j];
        }
    }
    return res;
}

typedef Matrix<4, 4, float> Matrix4x4f;
// template <class t>
// std::ostream &operator<<(std::ostream &s, Vec2<t> &v)
// {
//     s << "(" << v.x << ", " << v.y << ")\n";
//     return s;
// }

// template <class t>
// std::ostream &operator<<(std::ostream &s, Vec3<t> &v)
// {
//     s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
//     return s;
// }
template <class T>
inline Vec<T, 3> CrossProduct(const Vec<T, 3> &a, const Vec<T, 3> &b) { return Vec<T, 3>(a.y * b.z - b.y * a.z,
                                                                                         b.x * a.z - a.x * b.z, a.x * b.y - a.y * b.x); };

// template <class T>
// inline T DotProduct(const Vec<T, 3> &a, const Vec<T, 3> &b) { return a.x * b.x + a.y * b.y + a.y * b.y; }

template <class T, int l>
inline T DotProduct(const Vec<T, l> &a, const Vec<T, l> &b)
{
    T res = 0;
    for (int i = 0; i < l; i++)
    {
        res += a[i] * b[i];
    }
    return res;
}

// Transformations/ MVP

#endif //__GEOMETRY_H__