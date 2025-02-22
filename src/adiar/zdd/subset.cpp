#include <adiar/zdd.h>
#include <adiar/zdd/zdd_policy.h>

#include <adiar/internal/assert.h>
#include <adiar/internal/unreachable.h>
#include <adiar/internal/algorithms/select.h>
#include <adiar/internal/io/file_stream.h>

namespace adiar
{
  template<assignment FIX_VALUE>
  class zdd_subset_labels
  {
    const generator<zdd::label_type> &gen;

    /// \brief The current level (including the current algorithm level)
    optional<zdd::label_type> l_incl = make_optional<zdd::label_type>();

    /// \brief The next level (definitely excluding the current level)
    optional<zdd::label_type> l_excl = make_optional<zdd::label_type>();

    /// We will rememeber how far the algorithm in substitution.h has got
    zdd::label_type alg_level = 0;

  public:
    /// We will remember whether any level of the input actually matched.
    bool l_match = false;

  public:
    zdd_subset_labels(const generator<zdd::label_type> &g)
      : gen(g)
    {
      l_incl = gen();
      if (l_incl) { l_excl = gen(); }
    }

  private:
    /// \brief Forwards through the input to the given level
    inline void forward_to_level(const zdd::label_type new_level)
    {
      adiar_assert(alg_level <= new_level,
                   "The algorithm should ask for the levels in increasing order.");

      alg_level = new_level;

      while (l_incl && l_incl.value() < new_level) {
        l_incl = std::move(l_excl);
        if (l_incl) { l_excl = gen(); };
      }
    }

  public:
    /// \brief Obtain the assignment for the current level
    assignment assignment_for_level(const zdd::label_type new_level)
    {
      forward_to_level(new_level);

      const bool level_matches = l_incl && l_incl.value() == new_level;
      l_match |= level_matches;

      return level_matches ? FIX_VALUE : assignment::None;
    }

  public:
    /// \brief Whether the manager has a next level (including the current)
    bool has_level_incl()
    {
      return l_incl && alg_level <= l_incl.value();
    }

    /// \brief Get the current level (including the current algorithm level)
    zdd::label_type level_incl()
    {
      adiar_assert(has_level_incl());
      return l_incl.value();
    }

    /// \brief Whether the manager has a level ahead of the current
    bool has_level_excl()
    {
      return (l_incl && alg_level < l_incl.value()) || l_excl;
    }

    /// \brief Get the next level (excluding the current one)
    zdd::label_type level_excl()
    {
      adiar_assert(has_level_excl());
      if (alg_level < l_incl.value()) { return l_incl.value(); }
      return l_excl.value();
    }
  };

  //////////////////////////////////////////////////////////////////////////////
  template<typename assignment_mgr>
  class zdd_offset_policy : public zdd_policy
  {
  public:
    static internal::select_rec keep_node(const zdd::node_type &n, assignment_mgr &/*amgr*/)
    { return n; }

    static internal::select_rec fix_false(const zdd::node_type &n, assignment_mgr &/*amgr*/)
    { return n.low(); }

    // LCOV_EXCL_START
    static internal::select_rec fix_true(const zdd::node_type &/*n*/, assignment_mgr &/*amgr*/)
    { adiar_unreachable(); }
    // LCOV_EXCL_STOP

  public:
    static inline zdd terminal(bool terminal_val, assignment_mgr& /*amgr*/)
    { return zdd_terminal(terminal_val); }
  };

  __zdd zdd_offset(const exec_policy &ep, const zdd &A, const generator<zdd::label_type> &vars)
  {
    // Both { Ø }, and Ø cannot have more variables removed
    if (zdd_isterminal(A)) { return A; }

    zdd_subset_labels<assignment::False> amgr(vars);

    // Empty set of variables in `xs`?
    if (!amgr.has_level_incl()) { return A; }

    // Run select sweep
    __zdd res = internal::select<zdd_offset_policy<zdd_subset_labels<assignment::False>>>
      (ep, A, amgr);

    // Skip Reduce if no level of `xs` matched with any in `A`.
    if (!amgr.l_match) {
      return A;
    }
    return res;
  }

  __zdd zdd_offset(const zdd &A, const generator<zdd::label_type> &vars)
  {
    return zdd_offset(exec_policy(), A, vars);
  }

  __zdd zdd_offset(const exec_policy &ep, const zdd &A, zdd::label_type var)
  {
    return zdd_offset(ep, A, make_generator(var));
  }

  __zdd zdd_offset(const zdd &A, zdd::label_type var)
  {
    return zdd_offset(exec_policy(), A, var);
  }

  __zdd zdd_offset(const exec_policy &ep, const zdd &A)
  {
    return zdd_offset(ep, A, zdd_topvar(A));
  }

  __zdd zdd_offset(const zdd &A)
  {
    return zdd_offset(exec_policy(), A);
  }

  //////////////////////////////////////////////////////////////////////////////
  template<typename assignment_mgr>
  class zdd_onset_policy : public zdd_policy
  {
  public:
    static internal::select_rec keep_node(const zdd::node_type &n, assignment_mgr &amgr)
    {
      if (amgr.has_level_incl()) {
        // If recursion goes past the intended level, then it is replaced with
        // the false terminal.
        const zdd::pointer_type low  = n.low().is_terminal() || n.low().label() > amgr.level_incl()
          ? zdd::pointer_type(false)
          : n.low();

        // If this applies to high, then the node should be skipped entirely.
        if (n.high().is_terminal() || n.high().label() > amgr.level_incl()) {
          return low;
        }
        return zdd::node_type(n.uid(), low, n.high());
      }
      return n;
    }

    // LCOV_EXCL_START
    static internal::select_rec fix_false(const zdd::node_type &/*n*/, assignment_mgr &/*amgr*/)
    { adiar_unreachable(); }
    // LCOV_EXCL_STOP

    static internal::select_rec fix_true(const zdd::node_type &n, assignment_mgr &amgr)
    {
      if (amgr.has_level_excl()) {
        if (n.high().is_terminal() || n.high().label() > amgr.level_excl()) {
          return zdd::pointer_type(false);
        }
      }

      return zdd::node_type(n.uid(), zdd::pointer_type(false), n.high());
    }

  public:
    static inline zdd terminal(bool terminal_val, assignment_mgr &amgr)
    {
      return zdd_terminal(!amgr.has_level_excl() && terminal_val);
    }
  };

  __zdd zdd_onset(const exec_policy &ep, const zdd &A, const generator<zdd::label_type> &xs)
  {
    if (zdd_isfalse(A)) { return A; }

    zdd_subset_labels<assignment::True> amgr(xs);

    // Empty set of variables in `xs`?
    if (!amgr.has_level_incl()) {
      return A;
    }

    // If `A` is { Ø } and `xs` is non-empty, then it trivially collapses to Ø.
    if (zdd_istrue(A)) {
      return zdd_empty();
    }

    // Run select sweep
    __zdd res = internal::select<zdd_onset_policy<zdd_subset_labels<assignment::True>>>
      (ep, A, amgr);

    // Skip Reduce no levels of `xs` matched with one from `A`.
    if (!amgr.l_match) {
      return zdd_empty();
    }
    return res;
  }

  __zdd zdd_onset(const zdd &A, const generator<zdd::label_type> &xs)
  {
    return zdd_onset(exec_policy(), A, xs);
  }

  __zdd zdd_onset(const exec_policy &ep, const zdd &A, zdd::label_type var)
  {
    return zdd_onset(ep, A, make_generator(var));
  }

  __zdd zdd_onset(const zdd &A, zdd::label_type var)
  {
    return zdd_onset(exec_policy(), A, var);
  }

  __zdd zdd_onset(const exec_policy &ep, const zdd &A)
  {
    return zdd_onset(ep, A, zdd_topvar(A));
  }

  __zdd zdd_onset(const zdd &A)
  {
    return zdd_onset(exec_policy(), A);
  }
}
