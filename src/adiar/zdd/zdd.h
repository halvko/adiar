#ifndef ADIAR_ZDD_ZDD_H
#define ADIAR_ZDD_ZDD_H

#include <string_view>

#include <adiar/internal/cut.h>
#include <adiar/internal/dd.h>

namespace adiar
{
  // Class declarations to be able to reference it
  class zdd;

  //////////////////////////////////////////////////////////////////////////////
  /// \ingroup module__zdd
  ///
  /// \brief A (possibly unreduced) Zero-suppressed Decision Diagram.
  ///
  /// \relates zdd
  ///
  /// \copydetails adiar::internal::__dd
  //////////////////////////////////////////////////////////////////////////////
  class __zdd : public internal::__dd {
  public:
    ////////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor with an empty result.
    ////////////////////////////////////////////////////////////////////////////
    __zdd();

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Wrapper for an algorithm's already reduced output.
    ////////////////////////////////////////////////////////////////////////////
    __zdd(const shared_node_file_type &f);

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Wrapper for an algorithm's unreduced output.
    ////////////////////////////////////////////////////////////////////////////
    __zdd(const shared_arc_file_type &f, const exec_policy &ep);

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Conversion constructor from a `bdd` to pass along a prior value.
    ////////////////////////////////////////////////////////////////////////////
    __zdd(const zdd &zdd);

    // NOTE:
    //   '__dd' class includes 'max_1level_cut' and 'max_2level_cut' operations.
    //   This does not take the potential extra 'false' arc into account.
    //
    //   For now, we do not care to add this, since we are not using `__zdd` in
    //   a context where it is necessary such as 'zdd_union'.
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \ingroup module__zdd
  ///
  /// \brief Reduced Ordered Zero-suppressed Decision Diagram.
  ///
  /// \copydetails adiar::internal::dd
  //////////////////////////////////////////////////////////////////////////////
  class zdd : public internal::dd
  {
    ////////////////////////////////////////////////////////////////////////////
    // Friends
    // |- classes
    friend __zdd;

    // |- functions
    friend size_t zdd_nodecount(const zdd&);
    friend zdd::label_type zdd_varcount(const zdd&);

    friend bool zdd_subseteq(const exec_policy &ep, const zdd&, const zdd&);
    friend bool zdd_disjoint(const exec_policy &ep, const zdd &, const zdd &);

  public:
    /// \cond
    ////////////////////////////////////////////////////////////////////////////
    /// \brief Text to pretty-print in '.dot' output.
    ////////////////////////////////////////////////////////////////////////////
    static constexpr std::string_view false_print = "Ø";

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Text to pretty-print in '.dot' output.
    ////////////////////////////////////////////////////////////////////////////
    static constexpr std::string_view true_print = "{Ø}";
    /// \endcond

    ////////////////////////////////////////////////////////////////////////////
    // Constructors
  public:
    ////////////////////////////////////////////////////////////////////////////
    /// \brief Default construction, creating the empty set Ø.
    ///
    /// \see zdd_empty
    ////////////////////////////////////////////////////////////////////////////
    zdd();

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Implicit conversion from a terminal value to respectively
    ///        construct Ø and {Ø} from respectively 0 and 1.
    ///
    /// \see zdd_terminal, zdd_empty, zdd_null
    ////////////////////////////////////////////////////////////////////////////
    zdd(zdd::terminal_type t);

    /// \cond
    ////////////////////////////////////////////////////////////////////////////
    /// \brief Constructor to wrap the node-based result of an algorithm.
    ////////////////////////////////////////////////////////////////////////////
    zdd(const zdd::shared_node_file_type &A, bool negate = false);
    /// \endcond

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Copy construction, incrementing thre reference count on the file
    ///        underneath.
    ////////////////////////////////////////////////////////////////////////////
    zdd(const zdd &oA);

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Move construction, taking over ownership of the files underneath.
    ////////////////////////////////////////////////////////////////////////////
    zdd(zdd &&A);

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Implicit move conversion from a possibly to-be reduced result
    ///        from an algorithm to a `zdd`.
    ///
    /// \details Since the `adiar::internal::reduce` algorithm is run as part of
    ///          this constructor, the scoping rules ensure we garbage collect
    ///          irrelevant files as early as possible.
    ///
    /// \remark  Since the value `o` is forced to be moved, we force the content
    ///          of `o` to be destructed after finishing the *Reduce* algorithm.
    ////////////////////////////////////////////////////////////////////////////
    zdd(__zdd &&A);

    ////////////////////////////////////////////////////////////////////////////
    // Accessors overwrite
  public:
    /// \cond
    ////////////////////////////////////////////////////////////////////////////
    /// \brief Obtain the 1-level cut of the desired type, i.e. of the sub-graph
    ///        including the desired type of arcs.
    ///
    /// \param ct The type of the cut to obtain
    ////////////////////////////////////////////////////////////////////////////
    internal::cut::size_type max_1level_cut(const internal::cut ct) const
    {
      return add_false_cofactor(ct, file->max_1level_cut);
    }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Obtain the 2-level cut of the desired type, i.e. of the sub-graph
    ///        including the desired type of arcs.
    ///
    /// \param ct The type of the cut to obtain
    ////////////////////////////////////////////////////////////////////////////
    internal::cut::size_type max_2level_cut(const internal::cut ct) const
    {
      return add_false_cofactor(ct, file->max_2level_cut);
    }
    /// \endcond

  private:
    /// \cond
    ////////////////////////////////////////////////////////////////////////////
    /// \brief Accounts for the false arc added due to using a co-factor.
    ////////////////////////////////////////////////////////////////////////////
    internal::cut::size_type add_false_cofactor(const internal::cut ct, const internal::cuts_t &ilevel_cuts) const
    {
      const internal::safe_size_t cut_size = ilevel_cuts[ct];

      // Bit-mask (allowing implicit conversion to size_t with bit-operators) to
      // get the cut-type WITHOUT the false arcs.
      constexpr size_t bit_mask = internal::cut::Internal_True;
      const internal::cut ct_excl_false = static_cast<internal::cut>(ct & bit_mask);

      // In product construction algorithms we need to take into account the
      // (single) suppressed false arc, which may suddenly become visible (e.g.
      // 'zdd_union'). Here, the DAG gets stuck inside of a copy of only one of
      // the input ZDDs. To get there, one followed an arc of the input but
      // afterwards one pairs with an invisible false arc that spans all levels.
      //
      // We do not need to account for this invisible false arc in the following
      // two cases
      //
      // - If the requested cut does not include false arcs.
      //
      // - If the cut size is strictly larger than the corresponding cut
      //   excluding false. In this case, we already have a false arc to pair
      //   with.
      const size_t add_suppressed = !ct.includes(false) && cut_size == ilevel_cuts[ct_excl_false];

      return internal::to_size(cut_size + add_suppressed);
    }
    /// \endcond

    ////////////////////////////////////////////////////////////////////////////
    // Assignment operator overloadings
  public:
    ////////////////////////////////////////////////////////////////////////////
    /// \brief Assigns new `zdd`.
    ////////////////////////////////////////////////////////////////////////////
    zdd& operator= (const zdd &other);

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Assigns new `zdd` to a variable; the content is derefenced before
    ///        the given `__zdd` is reduced into a `zdd`.
    ////////////////////////////////////////////////////////////////////////////
    zdd& operator= (__zdd &&other);

    ////////////////////////////////////////////////////////////////////////////
    /// \see zdd_intsec
    ////////////////////////////////////////////////////////////////////////////
    zdd& operator&= (const zdd &other);

    /// \cond
    zdd& operator&= (zdd &&other);
    /// \endcond

    ////////////////////////////////////////////////////////////////////////////
    /// \see zdd_union
    ////////////////////////////////////////////////////////////////////////////
    zdd& operator|= (const zdd &other);

    /// \cond
    zdd& operator|= (zdd &&other);
    /// \endcond

    ////////////////////////////////////////////////////////////////////////////
    /// \see zdd_diff
    ////////////////////////////////////////////////////////////////////////////
    zdd& operator-= (const zdd &other);

    /// \cond
    zdd& operator-= (zdd &&other);
    /// \endcond
  };
}

#endif // ADIAR_ZDD_ZDD_H
