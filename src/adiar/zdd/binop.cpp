#include <limits>

#include <adiar/zdd.h>
#include <adiar/zdd/zdd_policy.h>

#include <adiar/internal/assert.h>
#include <adiar/internal/dd_func.h>
#include <adiar/internal/algorithms/prod2.h>
#include <adiar/internal/data_types/tuple.h>

namespace adiar
{
  bool can_left_shortcut_zdd(const bool_op &op, const zdd::pointer_type &terminal)
  {
    zdd::pointer_type terminal_F = zdd::pointer_type(false);
    zdd::pointer_type terminal_T = zdd::pointer_type(true);

    return // Does it shortcut on this level?
      op(terminal, terminal_F) == terminal_F && op(terminal, terminal_T) == terminal_F
      // Does it shortcut on all other levels below?
      && op(terminal_F, terminal_F) == terminal_F && op(terminal_F,  terminal_T) == terminal_F;
  }

  bool can_right_shortcut_zdd(const bool_op &op, const zdd::pointer_type &terminal)
  {
    zdd::pointer_type terminal_F = zdd::pointer_type(false);
    zdd::pointer_type terminal_T = zdd::pointer_type(true);

    return // Does it shortcut on this level?
      op(terminal_F, terminal) == terminal_F && op(terminal_T,  terminal) == terminal_F
      // Does it shortcut on all other levels below?
      && op(terminal_F, terminal_F) == terminal_F && op(terminal_T,  terminal_F) == terminal_F;
  }

  bool zdd_skippable(const bool_op &op, const zdd::node_type::children_type &r_high)
  {
    return (r_high[0].is_terminal() && r_high[1].is_terminal()
            && op(r_high[0], r_high[1]) == zdd::pointer_type(false))
      || (r_high[0].is_terminal() && can_left_shortcut_zdd(op, r_high[0]))
      || (r_high[1].is_terminal() && can_right_shortcut_zdd(op, r_high[1]));
 }

  //////////////////////////////////////////////////////////////////////////////
  // ZDD product construction policy
  class zdd_prod2_policy
    : public zdd_policy
    , public internal::prod2_mixed_level_merger<zdd_policy>
  {
  public:
    static __zdd resolve_same_file(const zdd &zdd_1, const zdd &/* zdd_2 */,
                                   const bool_op &op)
    {
      // Compute the results on all children.
      zdd::pointer_type op_F = op(zdd::pointer_type(false), zdd::pointer_type(false));
      zdd::pointer_type op_T = op(zdd::pointer_type(true), zdd::pointer_type(true));

      // Does it collapse to a terminal?
      if (op_F == op_T) {
        return zdd_terminal(op_F.value());
      }

      return zdd_1;
    }

  public:
    static __zdd resolve_terminal_root(const zdd& zdd_1,
                                       const zdd& zdd_2,
                                       const bool_op &op)
    {
      adiar_assert(zdd_isterminal(zdd_1) || zdd_isterminal(zdd_2));

      const zdd::pointer_type terminal_F = zdd::pointer_type(false);

      if (zdd_isterminal(zdd_1) && zdd_isterminal(zdd_2)) {
        const zdd::pointer_type p1 = dd_valueof(zdd_1);
        const zdd::pointer_type p2 = dd_valueof(zdd_2);

        return zdd_terminal(op(p1, p2).value());
      } else if (zdd_isterminal(zdd_1)) {
        const zdd::pointer_type p1 = dd_valueof(zdd_1);

        if (can_left_shortcut_zdd(op, p1))  {
          // Shortcuts the left-most path to {Ø} and all others to Ø
          return zdd_terminal(false);
        } else if (is_left_irrelevant(op, p1) && is_left_irrelevant(op, terminal_F)) {
          // Has no change to left-most path to {Ø} and neither any others
          return zdd_2;
        }
      } else { // if (is_terminal(zdd_2)) {
        const zdd::pointer_type p2 = dd_valueof(zdd_2);

        if (can_right_shortcut_zdd(op, p2)) {
          // Shortcuts the left-most path to {Ø} and all others to Ø
          return zdd_terminal(false);
        } else if (is_right_irrelevant(op, p2) && is_right_irrelevant(op, terminal_F)) {
          // Has no change to left-most path to {Ø} and neither any others
          return zdd_1;
        }
      }
      return __zdd(); // return with no_file
    }

  public:
    static internal::cut left_cut(const bool_op &op)
    {
      const bool incl_false = !can_left_shortcut_zdd(op, zdd::pointer_type(false));
      const bool incl_true = !can_left_shortcut_zdd(op, zdd::pointer_type(true));

      return internal::cut(incl_false, incl_true);
    }

    static internal::cut right_cut(const bool_op &op)
    {
      const bool incl_false = !can_right_shortcut_zdd(op, zdd::pointer_type(false));
      const bool incl_true = !can_right_shortcut_zdd(op, zdd::pointer_type(true));

      return internal::cut(incl_false, incl_true);
    }

  private:
    static internal::tuple<zdd::pointer_type>
    __resolve_request(const bool_op &op,
                      const internal::tuple<zdd::pointer_type> &r)
    {
      if (r[0].is_terminal() && can_left_shortcut_zdd(op, r[0])) {
        return { r[0], zdd::pointer_type(true) };
      }
      if (r[1].is_terminal() && can_right_shortcut_zdd(op, r[1])) {
        return { zdd::pointer_type(true), r[1] };
      }
      return r;
    }

  public:
    static internal::prod2_rec
    resolve_request(const bool_op &op,
                    const internal::tuple<zdd::pointer_type>  &r_low,
                    const internal::tuple<zdd::pointer_type > &r_high)
    {
      // If r_high surely suppresses the node during the ZDD Reduce, then skip
      // creating it in the first place and just forward the edge to r_low.
      if (zdd_skippable(op, r_high)) {
        // TODO: 'r_low' => '__resolve_request(op, r_low)' ?
        return internal::prod2_rec_skipto(r_low);
      }

      // Otherwise, create a node with children r_low and r_high
      return internal::prod2_rec_output {
        __resolve_request(op, r_low),
        __resolve_request(op, r_high)
      };
    }

    static constexpr bool no_skip = false;
  };

  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_binop(const exec_policy &ep,
                  const zdd &A,
                  const zdd &B,
                  const bool_op &op)
  {
    return internal::prod2<zdd_prod2_policy>(ep, A, B, op);
  }

  __zdd zdd_binop(const zdd &A, const zdd &B, const bool_op &op)
  {
    return zdd_binop(exec_policy(), A, B, op);
  }

  __zdd zdd_union(const exec_policy &ep, const zdd &A, const zdd &B)
  {
    return zdd_binop(ep, A, B, or_op);
  }

  __zdd zdd_union(const zdd &A, const zdd &B)
  {
    return zdd_union(exec_policy(), A, B);
  }

  __zdd zdd_intsec(const exec_policy &ep, const zdd &A, const zdd &B)
  {
    return zdd_binop(ep, A, B, and_op);
  }

  __zdd zdd_intsec(const zdd &A, const zdd &B)
  {
    return zdd_intsec(exec_policy(), A, B);
  }

  __zdd zdd_diff(const exec_policy &ep, const zdd &A, const zdd &B)
  {
    return zdd_binop(ep, A, B, diff_op);
  }

  __zdd zdd_diff(const zdd &A, const zdd &B)
  {
    return zdd_diff(exec_policy(), A, B);
  }
}
