#include <adiar/bdd.h>
#include <adiar/bdd/bdd_policy.h>

#include <utility>

#include <adiar/bool_op.h>
#include <adiar/internal/assert.h>
#include <adiar/internal/algorithms/quantify.h>
#include <adiar/internal/data_types/arc.h>
#include <adiar/internal/data_types/node.h>
#include <adiar/internal/data_types/tuple.h>
#include <adiar/internal/io/file_stream.h>
#include <adiar/internal/io/levelized_file_writer.h>
#include <adiar/internal/io/levelized_file_stream.h>

namespace adiar
{
  class bdd_quantify_policy : public bdd_policy
  {
  public:
    static inline bdd::ptr_t
    resolve_root(const bdd::node_t &r, const bool_op &op)
    {
      // TODO: should all but the last case not have a 'suppression taint'?

      // Return shortcutting terminal (including its tainting flag).
      if (r.low().is_terminal() && can_left_shortcut(op, r.low())) {
        return r.low();
      }
      if (r.high().is_terminal() && can_right_shortcut(op, r.high())) {
        return r.high();
      }

      // Return other child (including its tainting flag) for irrelevant terminals.
      if (r.low().is_terminal() && is_left_irrelevant(op, r.low())) {
        return r.high();
      }
      if (r.high().is_terminal() && is_right_irrelevant(op, r.high())) {
        return r.low();
      }

      // Otherwise return 'nothing'
      return r.uid();
    }

  public:
    static inline bool
    keep_terminal(const bool_op &op, const bdd::ptr_t &p)
    {
      // `op` is commutative, so we can check irrelevancy from either side.
      return !is_right_irrelevant(op, p);
    }

    static inline bool
    collapse_to_terminal(const bool_op &op, const bdd::ptr_t &p)
    {
      // `op` is commutative, so we can check shortcutting from either side.
      return can_right_shortcut(op, p);
    }

  public:
    static inline internal::cut_type
    cut_with_terminals(const bool_op &op)
    {
      const bool incl_false = !can_right_shortcut(op, bdd::ptr_t(false));
      const bool incl_true  = !can_right_shortcut(op, bdd::ptr_t(true));

      return internal::cut_type_with(incl_false, incl_true);
    }

  public:
    static constexpr bool quantify_onset = true;
  };

  //////////////////////////////////////////////////////////////////////////////
  __bdd bdd_exists(const bdd &f, bdd::label_t var)
  {
    return internal::quantify<bdd_quantify_policy>(f, var, or_op);
  }

  __bdd bdd_exists(const bdd &f, const std::function<bool(bdd::label_t)> &vars)
  {
    return internal::quantify<bdd_quantify_policy>(f, vars, or_op);
  }

  __bdd bdd_exists(bdd &&f, const std::function<bool(bdd::label_t)> &vars)
  {
    return internal::quantify<bdd_quantify_policy>(std::forward<bdd>(f), vars, or_op);
  }

  __bdd bdd_exists(const bdd &f, const std::function<bdd::label_t()> &gen)
  {
    return internal::quantify<bdd_quantify_policy>(f, gen, or_op);
  }

  __bdd bdd_exists(bdd &&f, const std::function<bdd::label_t()> &gen)
  {
    return internal::quantify<bdd_quantify_policy>(std::forward<bdd>(f), gen, or_op);
  }
  //////////////////////////////////////////////////////////////////////////////

  __bdd bdd_forall(const bdd &f, bdd::label_t var)
  {
    return internal::quantify<bdd_quantify_policy>(f, var, and_op);
  }

  __bdd bdd_forall(const bdd &f, const std::function<bool(bdd::label_t)> &vars)
  {
    return internal::quantify<bdd_quantify_policy>(f, vars, and_op);
  }

  __bdd bdd_forall(bdd &&f, const std::function<bool(bdd::label_t)> &vars)
  {
    return internal::quantify<bdd_quantify_policy>(std::forward<bdd>(f), vars, and_op);
  }

  __bdd bdd_forall(const bdd &f, const std::function<bdd::label_t()> &gen)
  {
    return internal::quantify<bdd_quantify_policy>(f, gen, and_op);
  }

  __bdd bdd_forall(bdd &&f, const std::function<bdd::label_t()> &gen)
  {
    return internal::quantify<bdd_quantify_policy>(std::forward<bdd>(f), gen, and_op);
  }
}
