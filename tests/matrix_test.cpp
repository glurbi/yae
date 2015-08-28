#include <gtest/gtest.h>

#include <matrix.hpp>

using namespace std;

TEST(matrix, add_two_vectors)
{
    auto v1 = yae::vector3f{ 1.0f, 2.0f, 3.0f };
    auto v2 = yae::vector3f{ 2.0f, 3.0f, 4.0f };
    auto v3 = v1 + v2;
    ASSERT_FLOAT_EQ(3.0f, v3.x());
    ASSERT_FLOAT_EQ(5.0f, v3.y());
    ASSERT_FLOAT_EQ(7.0f, v3.z());
}

TEST(matrix, midpoint)
{
    auto v1 = yae::vector3f{ 1.0f, 2.0f, 3.0f };
    auto v2 = yae::vector3f{ 2.0f, 3.0f, 4.0f };
    auto v3 = yae::midpoint(v1, v2);
    ASSERT_FLOAT_EQ(1.5f, v3.x());
    ASSERT_FLOAT_EQ(2.5f, v3.y());
    ASSERT_FLOAT_EQ(3.5f, v3.z());
}
