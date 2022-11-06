/* --------------------------------------------------------------------------
 * Vulkan Tutorial Common Library
 *
 * Lean and simple scope_guard implemenation for basic RAII needs
 *
 * SPDX-FileCopyrightText: 2022 Edouard Gomez
 * SPDX-License-Identifier: MIT
 * ----------------------------------------------------------------------- */

#pragma once

namespace egomez
{
namespace vulkan_tutorial
{

/** Executes a function when getting out of scope */
template <typename F> class scope_guard
{
  public:
    scope_guard(const F& f) : m_guard(f)
    {
    }

    ~scope_guard()
    {
        if (!m_commit)
        {
            m_guard();
        }
    }

    void commit()
    {
        m_commit = true;
    }

  private:
    bool m_commit = false;
    F m_guard;
};

template <typename aF> static const scope_guard<aF> makeScopeGuard(const aF& f)
{
    return scope_guard<aF>(f);
}

#define SCOPE_GUARD_NAME_CONCAT_IMPL(a, b) a##b
#define SCOPE_GUARD_NAME_CONCAT(a, b) SCOPE_GUARD_NAME_CONCAT_IMPL(a, b)

#define SCOPE_GUARD_NAMED(varname, f) auto varname = ::egomez::vulkan_tutorial::makeScopeGuard(f)

#define SCOPE_GUARD(f) SCOPE_GUARD_NAMED(SCOPE_GUARD_NAME_CONCAT(a_scope_guard_L, __LINE__), f)

} // namespace vulkan_tutorial
} // namespace egomez