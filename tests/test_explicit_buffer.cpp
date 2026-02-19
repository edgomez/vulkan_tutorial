/* --------------------------------------------------------------------------
 * Vulkan Tutorial Tests
 *
 * SPDX-FileCopyrightText: 2026 Edouard Gomez
 * SPDX-License-Identifier: MIT
 * ----------------------------------------------------------------------- */

#include <gtest/gtest.h>

#include <egomez/vulkan_tutorial/explicit_buffer.h>

#include <algorithm>
#include <cstddef>

using egomez::vulkan_tutorial::ExplicitBuffer;

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

TEST(ExplicitBuffer, DefaultConstructedIsEmpty)
{
    ExplicitBuffer<int> buf;
    EXPECT_TRUE(buf.empty());
    EXPECT_EQ(buf.size(), 0u);
    EXPECT_EQ(buf.data(), nullptr);
}

// ---------------------------------------------------------------------------
// reserve
// ---------------------------------------------------------------------------

TEST(ExplicitBuffer, ReserveZeroSucceeds)
{
    ExplicitBuffer<int> buf;
    EXPECT_TRUE(buf.reserve(0));
    EXPECT_TRUE(buf.empty());
}

TEST(ExplicitBuffer, ReserveAllocatesStorage)
{
    ExplicitBuffer<int> buf;
    EXPECT_TRUE(buf.reserve(8));
    EXPECT_EQ(buf.size(), 0u);
    EXPECT_NE(buf.data(), nullptr);
}

TEST(ExplicitBuffer, ReserveSmallerThanCapacityIsNoop)
{
    ExplicitBuffer<int> buf;
    EXPECT_TRUE(buf.reserve(16));
    const int* old_data = buf.data();
    EXPECT_TRUE(buf.reserve(4));
    EXPECT_EQ(buf.data(), old_data);
}

TEST(ExplicitBuffer, ReservePreservesExistingElements)
{
    ExplicitBuffer<int> buf;
    EXPECT_TRUE(buf.push_back(10));
    EXPECT_TRUE(buf.push_back(20));
    EXPECT_TRUE(buf.push_back(30));
    EXPECT_TRUE(buf.reserve(64));
    ASSERT_EQ(buf.size(), 3u);
    EXPECT_EQ(buf[0], 10);
    EXPECT_EQ(buf[1], 20);
    EXPECT_EQ(buf[2], 30);
}

// ---------------------------------------------------------------------------
// resize
// ---------------------------------------------------------------------------

TEST(ExplicitBuffer, ResizeSetsSize)
{
    ExplicitBuffer<int> buf;
    EXPECT_TRUE(buf.resize(5));
    EXPECT_EQ(buf.size(), 5u);
    EXPECT_FALSE(buf.empty());
}

TEST(ExplicitBuffer, ResizeToZero)
{
    ExplicitBuffer<int> buf;
    EXPECT_TRUE(buf.resize(10));
    EXPECT_TRUE(buf.resize(0));
    EXPECT_EQ(buf.size(), 0u);
    EXPECT_TRUE(buf.empty());
}

// ---------------------------------------------------------------------------
// push_back
// ---------------------------------------------------------------------------

TEST(ExplicitBuffer, PushBackSingleElement)
{
    ExplicitBuffer<int> buf;
    EXPECT_TRUE(buf.push_back(42));
    ASSERT_EQ(buf.size(), 1u);
    EXPECT_EQ(buf[0], 42);
}

TEST(ExplicitBuffer, PushBackMultipleElements)
{
    ExplicitBuffer<int> buf;
    for (int i = 0; i < 20; ++i)
    {
        ASSERT_TRUE(buf.push_back(i * 3));
    }
    ASSERT_EQ(buf.size(), 20u);
    for (int i = 0; i < 20; ++i)
    {
        EXPECT_EQ(buf[static_cast<std::size_t>(i)], i * 3);
    }
}

TEST(ExplicitBuffer, PushBackGrowsCapacity)
{
    ExplicitBuffer<int> buf;
    // Default first allocation is 4, so pushing 5 elements forces a growth.
    for (int i = 0; i < 5; ++i)
    {
        EXPECT_TRUE(buf.push_back(i));
    }
    ASSERT_EQ(buf.size(), 5u);
    for (int i = 0; i < 5; ++i)
    {
        EXPECT_EQ(buf[static_cast<std::size_t>(i)], i);
    }
}

// ---------------------------------------------------------------------------
// clear
// ---------------------------------------------------------------------------

TEST(ExplicitBuffer, ClearResetsSizeToZero)
{
    ExplicitBuffer<int> buf;
    EXPECT_TRUE(buf.push_back(1));
    EXPECT_TRUE(buf.push_back(2));
    buf.clear();
    EXPECT_EQ(buf.size(), 0u);
    EXPECT_TRUE(buf.empty());
}

TEST(ExplicitBuffer, ClearOnEmptyBufferIsHarmless)
{
    ExplicitBuffer<int> buf;
    buf.clear();
    EXPECT_TRUE(buf.empty());
}

// ---------------------------------------------------------------------------
// Element access
// ---------------------------------------------------------------------------

TEST(ExplicitBuffer, SubscriptOperator)
{
    ExplicitBuffer<int> buf;
    EXPECT_TRUE(buf.resize(3));
    buf[0] = 100;
    buf[1] = 200;
    buf[2] = 300;
    EXPECT_EQ(buf[0], 100);
    EXPECT_EQ(buf[1], 200);
    EXPECT_EQ(buf[2], 300);
}

TEST(ExplicitBuffer, ConstSubscriptOperator)
{
    ExplicitBuffer<int> buf;
    EXPECT_TRUE(buf.push_back(7));
    const ExplicitBuffer<int>& cbuf = buf;
    EXPECT_EQ(cbuf[0], 7);
}

TEST(ExplicitBuffer, DataPointerAccess)
{
    ExplicitBuffer<int> buf;
    EXPECT_TRUE(buf.push_back(11));
    EXPECT_TRUE(buf.push_back(22));
    EXPECT_EQ(buf.data()[0], 11);
    EXPECT_EQ(buf.data()[1], 22);

    const ExplicitBuffer<int>& cbuf = buf;
    EXPECT_EQ(cbuf.data()[0], 11);
}

// ---------------------------------------------------------------------------
// Iterators
// ---------------------------------------------------------------------------

TEST(ExplicitBuffer, BeginEndRangeIteration)
{
    ExplicitBuffer<int> buf;
    EXPECT_TRUE(buf.push_back(1));
    EXPECT_TRUE(buf.push_back(2));
    EXPECT_TRUE(buf.push_back(3));

    int sum = 0;
    for (int v : buf)
    {
        sum += v;
    }
    EXPECT_EQ(sum, 6);
}

TEST(ExplicitBuffer, ConstBeginEnd)
{
    ExplicitBuffer<int> buf;
    EXPECT_TRUE(buf.push_back(4));
    EXPECT_TRUE(buf.push_back(5));

    const ExplicitBuffer<int>& cbuf = buf;
    int                        sum = 0;
    for (const int& v : cbuf)
    {
        sum += v;
    }
    EXPECT_EQ(sum, 9);
}

TEST(ExplicitBuffer, EmptyBufferBeginEqualsEnd)
{
    ExplicitBuffer<int> buf;
    EXPECT_EQ(buf.begin(), buf.end());
}

TEST(ExplicitBuffer, StdAlgorithmCompatibility)
{
    ExplicitBuffer<int> buf;
    EXPECT_TRUE(buf.push_back(3));
    EXPECT_TRUE(buf.push_back(1));
    EXPECT_TRUE(buf.push_back(2));
    std::sort(buf.begin(), buf.end());
    EXPECT_EQ(buf[0], 1);
    EXPECT_EQ(buf[1], 2);
    EXPECT_EQ(buf[2], 3);
}

// ---------------------------------------------------------------------------
// Move semantics
// ---------------------------------------------------------------------------

TEST(ExplicitBuffer, MoveConstruction)
{
    ExplicitBuffer<int> a;
    EXPECT_TRUE(a.push_back(10));
    EXPECT_TRUE(a.push_back(20));

    ExplicitBuffer<int> b(std::move(a));

    // b has a's old contents
    ASSERT_EQ(b.size(), 2u);
    EXPECT_EQ(b[0], 10);
    EXPECT_EQ(b[1], 20);

    // a is in a moved-from state
    EXPECT_EQ(a.size(), 0u);
    EXPECT_TRUE(a.empty());
}

TEST(ExplicitBuffer, MoveAssignment)
{
    ExplicitBuffer<int> a;
    EXPECT_TRUE(a.push_back(1));
    EXPECT_TRUE(a.push_back(2));

    ExplicitBuffer<int> b;
    EXPECT_TRUE(b.push_back(99));

    b = std::move(a);

    ASSERT_EQ(b.size(), 2u);
    EXPECT_EQ(b[0], 1);
    EXPECT_EQ(b[1], 2);

    EXPECT_EQ(a.size(), 0u);
    EXPECT_TRUE(a.empty());
}

TEST(ExplicitBuffer, SelfMoveAssignmentIsSafe)
{
    ExplicitBuffer<int> buf;
    EXPECT_TRUE(buf.push_back(42));

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
#endif
    buf = std::move(buf);
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

    // Buffer should still be usable
    ASSERT_EQ(buf.size(), 1u);
    EXPECT_EQ(buf[0], 42);
}

// ---------------------------------------------------------------------------
// Non-trivial element type
// ---------------------------------------------------------------------------

TEST(ExplicitBuffer, WorksWithNonTrivialType)
{
    struct Point
    {
        int x;
        int y;
    };

    ExplicitBuffer<Point> buf;
    Point                 p1{1, 2};
    Point                 p2{3, 4};
    EXPECT_TRUE(buf.push_back(p1));
    EXPECT_TRUE(buf.push_back(p2));
    ASSERT_EQ(buf.size(), 2u);
    EXPECT_EQ(buf[0].x, 1);
    EXPECT_EQ(buf[0].y, 2);
    EXPECT_EQ(buf[1].x, 3);
    EXPECT_EQ(buf[1].y, 4);
}
