#include <iostream>
#include "geometry.h"

template <int ROW, int COL, class T>
void output_matrix(const Matrix<ROW, COL, T> &m)
{
    std::cout << m << std::endl;
}

template <class T, int l>
void output_vector(const Vec<T, l> &vec)
{
    std::cout << vec << std::endl;
}

void MatrixTest()
{
    Matrix<4, 5, float> a = Matrix<4, 5, float>::Identity();
    output_matrix(a);

    /* Basic Properties*/
    std::cout << "row :" << a.GetRowNumber() << " col:" << a.GetColNumber() << std::endl;

    Matrix<5, 5, float> b = Matrix<5, 5, float>::Identity();
    Matrix<5, 5, float> c;
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            c[i][j] = 1;
        }
    }

    output_matrix(a * b);
    output_matrix(b + c);
    output_matrix(b - c);
    output_matrix(b / 1);
    output_matrix(1.f / c);
    output_matrix(a.GetMinor(1, 1));
    output_matrix(a.Transpose());
    output_matrix(3.f * a);
    output_matrix(a * 3);
    std::cout << (b == c) << std::endl;
    std::cout << (b != c) << std::endl;
}

void VectorTest()
{
    Vec<float, 3> v1;
    v1[0] = 1.f, v1[1] = 2.f, v1[2] = 3.f;
    auto v2 = v1;
    output_vector(v1 + v2);
    output_vector(v1 - v2);
    // output_vector(v1 * v2);
    // output_vector(v1.normalize());
    for (int i = 0; i < 3; i++)
        std::cout << v1[i] << std::endl;
}