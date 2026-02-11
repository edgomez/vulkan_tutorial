/* --------------------------------------------------------------------------
 * Vulkan Tutorial Common Library
 *
 * Lean and simple scope_guard implementation for basic RAII needs
 *
 * SPDX-FileCopyrightText: 2022-2026 Edouard Gomez
 * SPDX-License-Identifier: MIT
 * ----------------------------------------------------------------------- */

#pragma once

#include <utility>

namespace egomez
{
namespace vulkan_tutorial
{

/** Executes a function when getting out of scope */
template <typename F> class scope_guard
{
  public:
    scope_guard(F&& f) : m_guard(std::forward<F>(f))
    {
    }

    ~scope_guard()
    {
        if (!m_dismissed)
        {
            m_guard();
        }
    }

    void dismiss()
    {
        m_dismissed = true;
    }

  private:
    bool m_dismissed = false;
    F    m_guard;
};

template <typename F> static scope_guard<F> makeScopeGuard(F&& f)
{
    return scope_guard<F>(std::forward<F>(f));
}

#define SCOPE_GUARD_NAME_CONCAT_IMPL(a, b) a##b
#define SCOPE_GUARD_NAME_CONCAT(a, b) SCOPE_GUARD_NAME_CONCAT_IMPL(a, b)

#define SCOPE_GUARD_NAMED(varname, f) auto varname = ::egomez::vulkan_tutorial::makeScopeGuard(f)

#define SCOPE_GUARD(f) SCOPE_GUARD_NAMED(SCOPE_GUARD_NAME_CONCAT(a_scope_guard_L, __LINE__), f)

} // namespace vulkan_tutorial
} // namespace egomez