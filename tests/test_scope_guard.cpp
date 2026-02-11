/* --------------------------------------------------------------------------
 * Vulkan Tutorial Tests
 *
 * SPDX-FileCopyrightText: 2026 Edouard Gomez
 * SPDX-License-Identifier: MIT
 * ----------------------------------------------------------------------- */

#include <gtest/gtest.h>

#include <egomez/vulkan_tutorial/scope_guard.h>

#include <stdexcept>

TEST(ScopeGuard, NormalOutOfScopeLambda)
{
    bool called = false;
    {
        SCOPE_GUARD_NAMED(guard, [&called] { called = true; });
        ASSERT_FALSE(called);
    }
    ASSERT_TRUE(called);
}

TEST(ScopeGuard, NormalOutOfScopeFunctor)
{
    struct Functor
    {
        bool called{false};
        void operator()()
        {
            called = true;
        }
    } f;

    {
        SCOPE_GUARD_NAMED(guard, f);
        ASSERT_FALSE(f.called);
    }
    ASSERT_TRUE(f.called);
}

TEST(ScopeGuard, NormalOutOfScopeStdFunction)
{
    bool called;

    std::function<void()> f = [&called]() { called = true; };
    {
        SCOPE_GUARD_NAMED(guard, f);
        ASSERT_FALSE(called);
    }
    ASSERT_TRUE(called);
}

TEST(ScopeGuard, ExceptionOutOfScope)
{
    bool called = false;
    bool exception_caught = false;
    try
    {
        SCOPE_GUARD_NAMED(guard, [&called] { called = true; });
        ASSERT_FALSE(called);
        throw std::runtime_error("out of scope by exception");
    }
    catch (std::runtime_error& e)
    {
        ASSERT_TRUE(called);
        exception_caught = true;
    }
    ASSERT_TRUE(called);
    ASSERT_TRUE(exception_caught);
}

TEST(ScopeGuard, DismissOutOfScope)
{
    bool called = false;
    {
        SCOPE_GUARD_NAMED(guard, [&called] { called = true; });
        ASSERT_FALSE(called);
        guard.dismiss();
    }
    ASSERT_FALSE(called);
}

TEST(ScopeGuard, DismissExceptionOutOfScope)
{
    bool called = false;
    bool exception_caught = false;
    try
    {
        SCOPE_GUARD_NAMED(guard, [&called] { called = true; });
        ASSERT_FALSE(called);
        guard.dismiss();
        throw std::runtime_error("out of scope by exception");
    }
    catch (std::runtime_error& e)
    {
        ASSERT_FALSE(called);
        exception_caught = true;
    }
    ASSERT_TRUE(exception_caught);
}