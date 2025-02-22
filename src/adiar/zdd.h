#ifndef ADIAR_ZDD_H
#define ADIAR_ZDD_H

////////////////////////////////////////////////////////////////////////////////
/// \defgroup module__zdd Zero-suppressed Decision Diagrams
///
/// \brief A Zero-suppressed Decision Diagram (ZDD) represents a family of a set
/// of \f$ n \f$ numbers, i.e. an \f$ S \subseteq 2^{\{ 0, 1, \dots, n-1 \}} \f$.
///
/// The \ref zdd class takes care of reference counting and optimal garbage
/// collection of the underlying files. To ensure the most disk-space is
/// available, try to garbage collect the \ref zdd objects as quickly as
/// possible and/or minimise the number of lvalues of said type.
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>

#include <adiar/bool_op.h>
#include <adiar/exec_policy.h>
#include <adiar/functional.h>

#include <adiar/zdd/zdd.h>
#include <adiar/bdd/bdd.h>

namespace adiar
{
  //////////////////////////////////////////////////////////////////////////////
  /// \addtogroup module__zdd
  ///
  /// \{

  //////////////////////////////////////////////////////////////////////////////
  /// \name Basic ZDD Constructors
  ///
  /// To construct a more complex but well-structured \ref zdd by hand, please
  /// use the \ref zdd_builder (see \ref builder) instead.
  ///
  /// \{

  //////////////////////////////////////////////////////////////////////////////
  /// \brief       The ZDD of only a single terminal.
  ///
  /// \param value The constant terminal value.
  ///
  /// \see         zdd_empty zdd_null
  //////////////////////////////////////////////////////////////////////////////
  zdd zdd_terminal(bool value);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The empty family, i.e. Ø .
  //////////////////////////////////////////////////////////////////////////////
  zdd zdd_empty();

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The family only with the empty set, i.e. { Ø } .
  //////////////////////////////////////////////////////////////////////////////
  zdd zdd_null();

  //////////////////////////////////////////////////////////////////////////////
  /// \brief     The set of bitvectors over a given domain where *var* is set to
  ///            true.
  ///
  /// \details   This function is (given the same domain of variables)
  ///            semantically equivalent to `bdd_ithvar` even though the ZDD DAG
  ///            does not at all look like the BDD DAG.
  ///
  /// \param var The variable to be forced to true.
  ///
  /// \param dom Generator function of the domain in \em descending order. These
  ///            variables should be smaller than or equals to `zdd::max_label`.
  ///
  /// \pre       The variable `var` should occur in `dom`.
  ///
  /// \throws invalid_argument If `dom` is not in \em descending order.
  //////////////////////////////////////////////////////////////////////////////
  zdd zdd_ithvar(zdd::label_type var, const generator<zdd::label_type> &dom);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief       The set of bitvectors over a given domain where *var* is set
  ///              to true.
  ///
  /// \param var   The variable to be forced to true.
  ///
  /// \param begin Single-pass forward iterator that provides the domain's
  ///              variables in \em descending order. The variables may not
  ///              exceed `zdd::max_label`.
  ///
  /// \param end   Marks the end for `begin`.
  ///
  /// \pre         The variable `var` should occur in `dom`.
  ///
  /// \throws invalid_argument If the iterator does not provide values in
  ///                          \em descending order.
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  zdd zdd_ithvar(zdd::label_type var, ForwardIt begin, ForwardIt end)
  { return zdd_ithvar(var, make_generator(begin, end)); }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief     The set of bitvectors over the globally set domain where *var*
  ///            is set to true.
  ///
  /// \param var The variable to be forced to true.
  ///
  /// \pre       `domain_isset() == true` and the variable `var` should
  ///            occur in the global domain.
  //////////////////////////////////////////////////////////////////////////////
  zdd zdd_ithvar(zdd::label_type var);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief     The set of bitvectors over a given domain where *var* is set to
  ///            false.
  ///
  /// \details   Creates a ZDD with a don't care chain of nodes to the true
  ///            child except for the node for `var`; this one instead is forced
  ///            to be true.
  ///
  /// \param var The variable to be forced to false.
  ///
  /// \param dom Generator function of the domain in \em descending order. The
  ///            variables should be smaller than or equals to `zdd::max_label`.
  ///
  /// \pre       The variable `var` should occur in `dom`.
  ///
  /// \throws invalid_argument If `dom` is not in \em descending order.
  //////////////////////////////////////////////////////////////////////////////
  zdd zdd_nithvar(zdd::label_type var, const generator<zdd::label_type> &dom);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief       The set of bitvectors over a given domain where *var* is set
  ///              to false.
  ///
  /// \param var   The variable to be forced to false.
  ///
  /// \param begin Single-pass forward iterator that provides the domain's
  ///              variables in \em descending order. The variables may not
  ///              exceed `zdd::max_label`.
  ///
  /// \param end   Marks the end for `begin`.
  ///
  /// \pre         The variable `var` should occur in `dom`.
  ///
  /// \throws invalid_argument If the iterator does not provide values in
  ///                          \em descending order.
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  zdd zdd_nithvar(zdd::label_type var, ForwardIt begin, ForwardIt end)
  { return zdd_nithvar(var, make_generator(begin, end)); }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief     The set of bitvectors over the globally set domain where *var*
  ///            is set to false.
  ///
  /// \param var The variable to be forced to false.
  ///
  /// \pre       `domain_isset() == true` and the variable `var` should
  ///            occur in the global domain.
  //////////////////////////////////////////////////////////////////////////////
  zdd zdd_nithvar(zdd::label_type var);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief     The family { { 1, 2, ..., k } }.
  ///
  /// \details   Creates a ZDD with a chain of nodes on the 'high' arc to the
  ///            true child, and false otherwise.
  ///
  /// \param vars Generator function of the variables in \em descending order.
  ///             The variables may not exceed `zdd::max_label`.
  ///
  /// \throws invalid_argument If `vars` are not in \em descending order.
  //////////////////////////////////////////////////////////////////////////////
  zdd zdd_vars(const generator<zdd::label_type> &vars);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief       The family { { 1, 2, ..., k } }.
  ///
  /// \param begin Single-pass forward iterator that provides the variables
  ///              in \em descending order. The variables may not exceed
  ///              `zdd::max_label`.
  ///
  /// \param end   Marks the end for `begin`.
  ///
  /// \throws invalid_argument If the iterator does not provide values in
  ///                          \em descending order.
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  zdd zdd_vars(ForwardIt begin, ForwardIt end)
  { return zdd_vars(make_generator(begin, end)); }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief     The family { { 1, 2, ..., k } } with a single bit-vector.
  ///
  /// \details   Creates the ZDD for a set with a single bit-vector, i.e. a
  ///            *point*.
  ///
  /// \param vars Generator function of the variables in \em descending order.
  ///             The variables may not exceed `zdd::max_label`.
  ///
  /// \throws invalid_argument If `vars` are not in \em descending order.
  //////////////////////////////////////////////////////////////////////////////
  zdd zdd_point(const generator<zdd::label_type> &vars);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief       The family { { 1, 2, ..., k } } with a single bit-vector.
  ///
  /// \param begin Single-pass forward iterator that provides the variables
  ///              in \em descending order. The variables may not exceed
  ///              `zdd::max_label`.
  ///
  /// \param end   Marks the end for `begin`.
  ///
  /// \throws invalid_argument If the iterator does not provide values in
  ///                          \em descending order.
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  zdd zdd_point(ForwardIt begin, ForwardIt end)
  { return zdd_point(make_generator(begin, end)); }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief     The family { {i} } .
  ///
  /// \details   Creates a ZDD of a single node with label `var` and the
  ///            children false and true. The given label must be smaller than
  ///            `zdd::max_label`.
  ///
  /// \param var The label of the desired variable to include
  ///
  /// \throws invalid_argument If `var` is a too large value.
  //////////////////////////////////////////////////////////////////////////////
  zdd zdd_singleton(zdd::label_type var);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief     The family { {1}, {2}, ..., {k} }.
  ///
  /// \details   Creates a ZDD with a chain of nodes on the 'low' arc to the
  ///            true child, and false otherwise.
  ///
  /// \param vars Generator function of the variables in \em descending order.
  ///             The variables may not exceed `zdd::max_label`.
  ///
  /// \throws invalid_argument If `vars` are not in \em descending order.
  //////////////////////////////////////////////////////////////////////////////
  zdd zdd_singletons(const generator<zdd::label_type> &vars);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief       The family { {1}, {2}, ..., {k} }.
  ///
  /// \param begin Single-pass forward iterator that provides the variables
  ///              in \em descending order. The variables may not exceed
  ///              `zdd::max_label`.
  ///
  /// \param end   Marks the end for `begin`.
  ///
  /// \throws invalid_argument If the iterator does not provide values in
  ///                          \em descending order.
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  zdd zdd_singletons(ForwardIt begin, ForwardIt end)
  { return zdd_singletons(make_generator(begin, end)); }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief     The powerset of all given variables.
  ///
  /// \details   Creates a ZDD with a don't care chain of nodes to the true
  ///            child.
  ///
  /// \param vars Generator function of the variables in \em descending order.
  ///             These values may not exceed `zdd::max_label`.
  ///
  /// \throws invalid_argument If `vars` are not in \em ascending order.
  //////////////////////////////////////////////////////////////////////////////
  zdd zdd_powerset(const generator<zdd::label_type> &vars);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief       The powerset of all given variables.
  ///
  /// \param begin Single-pass forward iterator that provides the variables
  ///              in \em descending order. The variables may not exceed
  ///              `zdd::max_label`
  ///
  /// \param end   Marks the end for `begin`.
  ///
  /// \throws invalid_argument If the iterator does not provide values in
  ///                          \em descending order.
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  zdd zdd_powerset(ForwardIt begin, ForwardIt end)
  { return zdd_powerset(make_generator(begin, end)); }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief     Bottom of the powerset lattice.
  ///
  /// \param dom Generator function of the variables in \em descending order.
  ///            These values may not exceed `zdd::max_label`.
  ///
  /// \see zdd_empty
  //////////////////////////////////////////////////////////////////////////////
  zdd zdd_bot(const generator<zdd::label_type> &dom);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief       Bottom of the powerset lattice.
  ///
  /// \param begin Single-pass forward iterator that provides the domain's
  ///              variables in \em descending order. The variables may not
  ///              exceed `zdd::max_label`.
  ///
  /// \param end   Marks the end for `begin`.
  ///
  /// \see zdd_empty
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  inline zdd zdd_bot(ForwardIt begin, ForwardIt end)
  { return zdd_bot(make_generator(begin, end)); }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Bottom of the powerset lattice.
  ///
  /// \see zdd_empty
  //////////////////////////////////////////////////////////////////////////////
  zdd zdd_bot();

  //////////////////////////////////////////////////////////////////////////////
  /// \brief     Top of the powerset lattice.
  ///
  /// \param dom Generator of the variables in \em descending order. These
  ///            values may not exceed `zdd::max_label`.
  ///
  /// \see zdd_powerset, zdd_null
  //////////////////////////////////////////////////////////////////////////////
  zdd zdd_top(const generator<zdd::label_type> &dom);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief       Top of the powerset lattice.
  ///
  /// \param begin Single-pass forward iterator that provides the domain's
  ///              variables in \em descending order. These values may not
  ///              exceed `zdd::max_label`.
  ///
  /// \param end   Marks the end for `begin`.
  ///
  /// \see zdd_empty
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  inline zdd zdd_top(ForwardIt begin, ForwardIt end)
  { return zdd_top(make_generator(begin, end)); }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief   Top of the powerset lattice.
  ///
  /// \details Since no set of variables is given, the global \ref
  ///          module__domain is used (if available).
  ///
  /// \see zdd_powerset, zdd_null
  //////////////////////////////////////////////////////////////////////////////
  zdd zdd_top();

  /// \}
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  /// \name Basic ZDD Manipulation
  ///
  /// \{

  //////////////////////////////////////////////////////////////////////////////
  /// \brief     Apply a binary operator between the sets of two families.
  ///
  ///
  /// \param A   ZDD for the left-hand-side of the operator
  /// \param B   ZDD for the right-hand-side of the operator
  /// \param op  Binary boolean operator to be applied. See 'adiar/data.h'
  ///
  /// \return Product construction of the two that represents the boolean
  ///         operator applied to the two family of sets.
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_binop(const zdd &A, const zdd &B, const bool_op &op);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Apply a binary operator between the sets of two families.
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_binop(const exec_policy &ep,
                  const zdd &A,
                  const zdd &B,
                  const bool_op &op);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The union of two families of sets.
  ///
  /// \returns
  /// \f$ A \cup B \f$
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_union(const zdd &A, const zdd &B);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The union of two families of sets.
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_union(const exec_policy &ep, const zdd &A, const zdd &B);

  //////////////////////////////////////////////////////////////////////////////
  /// \see zdd_union
  //////////////////////////////////////////////////////////////////////////////
  __zdd operator| (const zdd &lhs, const zdd &rhs);

  /// \cond
  __zdd operator| (__zdd &&, __zdd &&);
  __zdd operator| (const zdd &, __zdd &&);
  __zdd operator| (__zdd &&, const zdd &);
  /// \endcond

  //////////////////////////////////////////////////////////////////////////////
  /// \brief   The intersection of two families of sets.
  ///
  /// \returns
  /// \f$ A \cap B \f$
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_intsec(const zdd &A, const zdd &B);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The intersection of two families of sets.
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_intsec(const exec_policy &ep, const zdd &A, const zdd &B);

  //////////////////////////////////////////////////////////////////////////////
  /// \see zdd_intsec
  //////////////////////////////////////////////////////////////////////////////
  __zdd operator& (const zdd &lhs, const zdd &rhs);

  /// \cond
  __zdd operator& (__zdd &&, __zdd &&);
  __zdd operator& (const zdd &, __zdd &&);
  __zdd operator& (__zdd &&, const __zdd &);
  /// \endcond

  //////////////////////////////////////////////////////////////////////////////
  /// \brief   The set difference of two families of sets.
  ///
  /// \returns
  /// \f$ A \setminus B \f$
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_diff(const zdd &A, const zdd &B);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The set difference of two families of sets.
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_diff(const exec_policy &ep, const zdd &A, const zdd &B);

  //////////////////////////////////////////////////////////////////////////////
  /// \see zdd_diff
  //////////////////////////////////////////////////////////////////////////////
  __zdd operator- (const zdd &lhs, const zdd &rhs);

  /// \cond
  __zdd operator- (__zdd &&, __zdd &&);
  __zdd operator- (const zdd &, __zdd &&);
  __zdd operator- (__zdd &&, const zdd &);
  /// \endcond

  //////////////////////////////////////////////////////////////////////////////
  /// \brief      The symmetric difference between each set in the family and
  ///             the given set of variables.
  ///
  /// \param A    ZDD to apply with the other.
  ///
  /// \param vars Generator function of labels to flip in \em ascending order.
  ///             These values may not exceed `zdd::max_label`.
  ///
  /// \returns
  /// \f$ \{ \mathit{vars} \Delta a \mid a \in A \} \f$
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_change(const zdd &A, const generator<zdd::label_type> &vars);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The symmetric difference between each set in the family and the
  ///        given set of variables.
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_change(const exec_policy &ep,
                   const zdd &A,
                   const generator<zdd::label_type> &vars);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief      The symmetric difference between each set in the family and
  ///             the given set of variables.
  ///
  /// \param A    ZDD to apply with the other.
  ///
  /// \param begin Single-pass forward iterator that provides the to-be
  ///              flipped variables in \em ascending order. These values may
  ///              not exceed `zdd::max_label`.
  ///
  /// \param end   Marks the end for `begin`.
  ///
  /// \returns
  /// \f$ \{ \mathit{vars} \Delta a \mid a \in A \} \f$
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  __zdd zdd_change(const zdd &A, ForwardIt begin, ForwardIt end)
  { return zdd_change(A, make_generator(begin, end)); }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The symmetric difference between each set in the family and the
  ///        given set of variables.
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  __zdd zdd_change(const exec_policy &ep,
                   const zdd &A,
                   ForwardIt begin,
                   ForwardIt end)
  { return zdd_change(ep, A, make_generator(begin, end)); }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief     Complement of A within the given domain.
  ///
  /// \param A   family of sets to complement
  ///
  /// \param dom Labels of the domain in \em ascending order
  ///
  /// \returns    \f$ 2^{\mathit{dom}} \setminus A \f$
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_complement(const zdd &A, const generator<zdd::label_type> &dom);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Complement of A within the given domain.
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_complement(const exec_policy& ep,
                       const zdd &A,
                       const generator<zdd::label_type> &dom);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief       Complement of A within the given domain.
  ///
  /// \param A     family of sets to complement
  ///
  /// \param begin Single-pass forward iterator that provides the domain's
  ///              variables in \em ascending order. These values may not exceed
  ///              `zdd::max_label`.
  ///
  /// \param end   Marks the end for `begin`.
  ///
  /// \returns     \f$ 2^{\mathit{dom}} \setminus A \f$
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  __zdd zdd_complement(const zdd &A, ForwardIt begin, ForwardIt end)
  { return zdd_complement(A, make_generator(begin, end)); }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Complement of A within the given domain.
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  __zdd zdd_complement(const exec_policy &ep,
                       const zdd &A,
                       ForwardIt begin,
                       ForwardIt end)
  { return zdd_complement(ep, A, make_generator(begin, end)); }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief     Complement of A within the global \ref module__domain
  ///
  /// \param A   family of sets to complement
  ///
  /// \returns   \f$ 2^{\mathit{dom}} \setminus A \f$
  ///
  /// \see domain_set domain_isset
  ///
  /// \pre       The global \ref module__domain is set to a set of variables
  ///            that is equals to or a superset of the variables in `A`.
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_complement(const zdd &A);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief     Complement of A within the global \ref module__domain
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_complement(const exec_policy& ep, const zdd &A);

  //////////////////////////////////////////////////////////////////////////////
  /// \see zdd_complement
  //////////////////////////////////////////////////////////////////////////////
  __zdd operator~ (const zdd& A);

  /// \cond
  __zdd operator~ (__zdd&& A);
  /// \endcond

  //////////////////////////////////////////////////////////////////////////////
  /// \brief      Expands the domain of the given ZDD to also include the given
  ///             set of labels.
  ///
  /// \details    Adds don't care nodes on each levels in `vars`. That is, this
  ///             essentially is the inverse of the `zdd_project` and lifts the
  ///             set of sets unprojects to a larger domain.
  ///
  /// \param A    Family of set to expand.
  ///
  /// \param vars Generator function of labels to unproject in \em ascending
  ///             order. No variables it generates may already exist in `A` or
  ///             exceed `zdd::max_label`.
  ///
  /// \returns
  /// \f$ \bigcup_{a \in A, i \in 2^{vars}} (a \cup i) \f$
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_expand(const zdd &A, const generator<zdd::label_type> &vars);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief   Expands the domain of the given ZDD to also include the given set
  ///          of labels.
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_expand(const exec_policy &ep,
                   const zdd &A,
                   const generator<zdd::label_type> &vars);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief       Expands the domain of the given ZDD to also include the given
  ///              set of labels.
  ///
  /// \copydetails
  ///
  /// \param A     Family of set to expand.
  ///
  /// \param begin Single-pass forward iterator that provides the to-be
  ///              unprojected variables in \em ascending order. These may \em not
  ///              be present in `A` or exceed `zdd::max_label`.
  ///
  /// \param end   Marks the end for `begin`.
  ///
  /// \returns
  /// \f$ \bigcup_{a \in A, i \in 2^{vars}} (a \cup i) \f$
  ///
  /// \see zdd_project
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  __zdd zdd_expand(const zdd &A, ForwardIt begin, ForwardIt end)
  { return zdd_expand(A, make_generator(begin, end)); }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief   Expands the domain of the given ZDD to also include the given set
  ///          of labels.
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  __zdd zdd_expand(const exec_policy &ep,
                   const zdd &A,
                   ForwardIt begin, ForwardIt end)
  { return zdd_expand(ep, A, make_generator(begin, end)); }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief     Subset that do \em not include the given element.
  ///
  /// \param A   Family of set
  ///
  /// \param var Variable to include.
  ///
  /// \returns
  /// \f$ \{ a \in A \mid \mathit{var} \not\in a \} \f$
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_offset(const zdd &A, zdd::label_type var);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Subset that do \em not include the given element.
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_offset(const exec_policy &ep, const zdd &A, zdd::label_type var);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief  Subset that do \em not include the top variable.
  ///
  /// \param A Family of set
  ///
  /// \remark  In other BDD packages, this function is good for traversing a ZDD.
  ///          But, here this is not a constant-time operation but constructs
  ///          an entire new ZDD of up-to linear size.
  ///
  /// \throws invalid_argument If `A` is a terminal.
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_offset(const zdd &A);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Subset that do \em not include the top variable.
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_offset(const exec_policy &ep, const zdd &A);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief      Subset that do \em not include the given set of variables.
  ///
  /// \param A    Family of set
  ///
  /// \param vars Generator function of the variable labels to filter on in
  ///             \em ascending order. The values generated should not exceed
  ///             `zdd::max_label`.
  ///
  /// \returns
  /// \f$ \{ a \in A \mid \forall i \in \mathit{vars} : i \not\in a \} \f$
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_offset(const zdd &A, const generator<zdd::label_type> &vars);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Subset that do \em not include the given set of variables.
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_offset(const exec_policy &ep,
                   const zdd &A,
                   const generator<zdd::label_type> &vars);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief      Subset that do \em not include the given set of variables.
  ///
  /// \param A    Family of set
  ///
  /// \param begin Single-pass forward iterator that provides the variables to
  ///              filter out in \em ascending order. These values may not
  ///              exceed `zdd::max_label`.
  ///
  /// \param end   Marks the end for `begin`.
  ///
  /// \returns
  /// \f$ \{ a \in A \mid \forall i \in \mathit{vars} : i \not\in a \} \f$
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  __zdd zdd_offset(const zdd &A, ForwardIt begin, ForwardIt end)
  { return zdd_offset(A, make_generator(begin, end)); }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Subset that do \em not include the given set of variables.
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  __zdd zdd_offset(const exec_policy &ep,
                   const zdd &A,
                   ForwardIt begin,
                   ForwardIt end)
  { return zdd_offset(ep, A, make_generator(begin, end)); }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief     Subset that \em do include the given element.
  ///
  /// \param A   Family of set
  ///
  /// \param var Variable to include.
  /// \returns
  /// \f$ \{ a \in A \mid \mathit{var} \in a \} \f$
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_onset(const zdd &A, zdd::label_type var);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Subset that \em do include the given element.
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_onset(const exec_policy &ep, const zdd &A, zdd::label_type var);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief   Subset that \em do include the top variable.
  ///
  /// \param A Family of set
  ///
  /// \remark  In other BDD packages, this function is good for traversing a ZDD.
  ///          But, here this is not a constant-time operation but constructs
  ///          an entire new ZDD of up-to linear size.
  ///
  /// \throws invalid_argument If `A` is a terminal.
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_onset(const zdd &A);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Subset that \em do include the top variable.
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_onset(const exec_policy &ep, const zdd &A);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief      Subset that \em do include the given set of variables.
  ///
  /// \param A    Family of set
  ///
  /// \param vars Generator function of the variable labels to filter on in
  ///             \em ascending order. The values generated may not exceed
  ///             `zdd::max_label`
  ///
  /// \returns
  /// \f$ \{ a \in A \mid \forall i \in \mathit{vars} : i \in a \} \f$
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_onset(const zdd &A, const generator<zdd::label_type> &vars);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief      Subset that \em do include the given set of variables.
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_onset(const exec_policy &ep,
                  const zdd &A,
                  const generator<zdd::label_type> &vars);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief      Subset that \em do include the given set of variables.
  ///
  /// \param A    Family of set
  ///
  /// \param begin Single-pass forward iterator that provides the variables to
  ///              filter out in \em ascending order. These values may not
  ///              exceed `zdd::max_label`
  ///
  /// \param end   Marks the end for `begin`.
  ///
  /// \returns
  /// \f$ \{ a \in A \mid \forall i \in \mathit{vars} : i \in a \} \f$
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  __zdd zdd_onset(const zdd &A, ForwardIt begin, ForwardIt end)
  { return zdd_onset(A, make_generator(begin, end)); }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief      Subset that \em do include the given set of variables.
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  __zdd zdd_onset(const exec_policy &ep,
                  const zdd &A,
                  ForwardIt begin,
                  ForwardIt end)
  { return zdd_onset(ep, A, make_generator(begin, end)); }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief     Project family of sets onto a domain, i.e. remove from every
  ///            set all variables not within the domain.
  ///
  /// \param A   Family of sets to project
  ///
  /// \param dom Predicate function that defines the domain.
  ///
  /// \returns
  /// \f$ \prod_{\mathit{dom}}(A) = \{ a \setminus \mathit{dom}^c \mid a \in A \} \f$
  ///
  /// \see zdd_expand
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_project(const zdd &A, const predicate<zdd::label_type> &dom);

  /// \cond
  __zdd zdd_project(zdd &&A, const predicate<zdd::label_type> &dom);
  /// \endcond

  //////////////////////////////////////////////////////////////////////////////
  /// \brief       Project family of sets onto a domain, i.e. remove from every
  ///              set all variables not within the domain.
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_project(const exec_policy &ep,
                    const zdd &A,
                    const predicate<zdd::label_type> &dom);

  /// \cond
  __zdd zdd_project(const exec_policy &ep,
                    zdd &&A,
                    const predicate<zdd::label_type> &dom);
  /// \endcond

  //////////////////////////////////////////////////////////////////////////////
  /// \brief     Project family of sets onto a domain, i.e. remove from every
  ///            set all variables not within the domain.
  ///
  /// \param A   Family of sets to project
  ///
  /// \param dom Generator function, that produces the variables of the domain in
  ///            \em descending order. They should not exceed `zdd::max_label`.
  ///
  /// \returns
  /// \f$ \prod_{\mathit{dom}}(A) = \{ a \setminus \mathit{dom}^c \mid a \in A \} \f$
  ///
  /// \see zdd_expand
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_project(const zdd &A, const generator<zdd::label_type> &dom);

  /// \cond
  __zdd zdd_project(zdd &&A, const generator<zdd::label_type> &dom);
  /// \endcond

  //////////////////////////////////////////////////////////////////////////////
  /// \brief       Project family of sets onto a domain, i.e. remove from every
  ///              set all variables not within the domain.
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_project(const exec_policy &ep,
                    const zdd &A,
                    const generator<zdd::label_type> &dom);

  /// \cond
  __zdd zdd_project(const exec_policy &ep,
                    zdd &&A,
                    const generator<zdd::label_type> &dom);
  /// \endcond

  //////////////////////////////////////////////////////////////////////////////
  /// \brief       Project family of sets onto a domain, i.e. remove from every
  ///              set all variables not within the domain.
  ///
  /// \param A     Family of sets to project
  ///
  /// \param begin Single-pass forward iterator that provides the domain in
  ///              \em descending order. Its values should not exceed
  ///              `zdd::max_label`
  ///
  /// \param end   Marks the end for `begin`.
  ///
  /// \returns
  /// \f$ \prod_{\mathit{dom}}(A) = \{ a \setminus \mathit{dom}^c \mid a \in A \} \f$
  ///
  /// \see zdd_expand
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  __zdd zdd_project(const zdd &A, ForwardIt begin, ForwardIt end)
  { return zdd_project(A, make_generator(begin, end)); }

  /// \cond
  template<typename ForwardIt>
  __zdd zdd_project(zdd &&A, ForwardIt begin, ForwardIt end)
  { return zdd_project(std::forward<zdd>(A), make_generator(begin, end)); }
  /// \endcond

  //////////////////////////////////////////////////////////////////////////////
  /// \brief       Project family of sets onto a domain, i.e. remove from every
  ///              set all variables not within the domain.
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  __zdd zdd_project(const exec_policy &ep,
                    const zdd &A,
                    ForwardIt begin,
                    ForwardIt end)
  { return zdd_project(ep, A, make_generator(begin, end)); }

  /// \cond
  template<typename ForwardIt>
  __zdd zdd_project(const exec_policy &ep,
                    zdd &&A,
                    ForwardIt begin,
                    ForwardIt end)
  { return zdd_project(ep, std::forward<zdd>(A), make_generator(begin, end)); }
  /// \endcond

  /// \}
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  /// \name ZDD Predicates
  ///
  /// \{

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Check whether a given ZDD is canonical.
  ///
  /// \copydetails adiar::internal::dd_iscanonical
  //////////////////////////////////////////////////////////////////////////////
  bool zdd_iscanonical(const zdd& A);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Whether this ZDD represents a terminal.
  //////////////////////////////////////////////////////////////////////////////
  bool zdd_isterminal(const zdd& A);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Whether this ZDD represents false terminal.
  //////////////////////////////////////////////////////////////////////////////
  bool zdd_isfalse(const zdd& A);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Whether it is the empty family, i.e. Ø .
  //////////////////////////////////////////////////////////////////////////////
  bool zdd_isempty(const zdd &A);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Whether this BDD represents true terminal.
  //////////////////////////////////////////////////////////////////////////////
  bool zdd_istrue(const zdd& A);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Whether it is the null family, i.e. { Ø } .
  //////////////////////////////////////////////////////////////////////////////
  bool zdd_isnull(const zdd &A);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Whether it contains a single bit-vector `a`, i.e. A = { a }.
  //////////////////////////////////////////////////////////////////////////////
  bool zdd_ispoint(const zdd &A);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Whether they represent the same family.
  //////////////////////////////////////////////////////////////////////////////
  bool zdd_equal(const zdd &A, const zdd &B);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Whether they represent the same family.
  //////////////////////////////////////////////////////////////////////////////
  bool zdd_equal(const exec_policy &ep, const zdd &A, const zdd &B);

  //////////////////////////////////////////////////////////////////////////////
  /// \see zdd_equal
  //////////////////////////////////////////////////////////////////////////////
  bool operator== (const zdd& lhs, const zdd& rhs);

  /// \cond
  bool operator== (__zdd &&, __zdd &&);
  bool operator== (const zdd &, __zdd &&);
  bool operator== (__zdd &&, const zdd &);
  /// \endcond

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Whether they represent two different families.
  //////////////////////////////////////////////////////////////////////////////
  bool zdd_unequal(const zdd &A, const zdd &B);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Whether they represent two different families.
  //////////////////////////////////////////////////////////////////////////////
  bool zdd_unequal(const exec_policy &ep, const zdd &A, const zdd &B);

  //////////////////////////////////////////////////////////////////////////////
  /// \see zdd_unequal
  //////////////////////////////////////////////////////////////////////////////
  bool operator!= (const zdd& lhs, const zdd& rhs);

  /// \cond
  bool operator!= (__zdd &&, __zdd &&);
  bool operator!= (const zdd &, __zdd &&);
  bool operator!= (__zdd &&, const zdd &);
  /// \endcond

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Whether one family is a subset or equal to the other.
  //////////////////////////////////////////////////////////////////////////////
  bool zdd_subseteq(const zdd &A, const zdd &B);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Whether one family is a subset or equal to the other.
  //////////////////////////////////////////////////////////////////////////////
  bool zdd_subseteq(const exec_policy &ep, const zdd &A, const zdd &B);

  //////////////////////////////////////////////////////////////////////////////
  /// \see zdd_subseteq
  //////////////////////////////////////////////////////////////////////////////
  bool operator<= (const zdd& lhs, const zdd& rhs);

  /// \cond
  bool operator<= (__zdd &&, __zdd &&);
  bool operator<= (const zdd &, __zdd &&);
  bool operator<= (__zdd &&, const zdd &);
  /// \endcond

  //////////////////////////////////////////////////////////////////////////////
  /// \see zdd_subseteq
  //////////////////////////////////////////////////////////////////////////////
  bool operator>= (const zdd& lhs, const zdd& rhs);

  /// \cond
  bool operator>= (__zdd &&, __zdd &&);
  bool operator>= (const zdd &, __zdd &&);
  bool operator>= (__zdd &&, const zdd &);
  /// \endcond

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Whether one family is a strict subset of the other.
  //////////////////////////////////////////////////////////////////////////////
  bool zdd_subset(const zdd &A, const zdd &B);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Whether one family is a strict subset of the other.
  //////////////////////////////////////////////////////////////////////////////
  bool zdd_subset(const exec_policy &ep, const zdd &A, const zdd &B);

  //////////////////////////////////////////////////////////////////////////////
  /// \see zdd_subset
  //////////////////////////////////////////////////////////////////////////////
  bool operator< (const zdd& lhs, const zdd& rhs);

  /// \cond
  bool operator< (__zdd &&, __zdd &&);
  bool operator< (const zdd &, __zdd &&);
  bool operator< (__zdd &&, const zdd &);
  /// \endcond

  //////////////////////////////////////////////////////////////////////////////
  /// \see zdd_subset
  //////////////////////////////////////////////////////////////////////////////
  bool operator> (const zdd& lhs, const zdd& rhs);

  /// \cond
  bool operator> (__zdd &&, __zdd &&);
  bool operator> (const zdd &, __zdd &&);
  bool operator> (__zdd &&, const zdd &);
  /// \endcond

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Whether the two families are disjoint.
  //////////////////////////////////////////////////////////////////////////////
  bool zdd_disjoint(const zdd &A, const zdd &B);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Whether the two families are disjoint.
  //////////////////////////////////////////////////////////////////////////////
  bool zdd_disjoint(const exec_policy &ep, const zdd &A, const zdd &B);

  /// \}
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  /// \name ZDD Counting Operations
  ///
  /// \{

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The number of (internal) nodes used to represent the family of
  ///        sets.
  //////////////////////////////////////////////////////////////////////////////
  size_t zdd_nodecount(const zdd &A);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The number of variables that exist in the family of sets, i.e. the
  ///        number of levels in the ZDD.
  //////////////////////////////////////////////////////////////////////////////
  zdd::label_type zdd_varcount(const zdd &A);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The number of sets in the family of sets.
  //////////////////////////////////////////////////////////////////////////////
  uint64_t zdd_size(const zdd &A);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The number of sets in the family of sets.
  //////////////////////////////////////////////////////////////////////////////
  uint64_t zdd_size(const exec_policy &ep, const zdd &A);

  /// \}
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  /// \name Set Elements
  ///
  /// \{

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Get (in \em ascending order) all of the variable labels that occur
  ///        in the family.
  ///
  /// \param A  ZDD of interest.
  ///
  /// \param cb Callback function that consumes the variable labels.
  //////////////////////////////////////////////////////////////////////////////
  void zdd_support(const zdd &A, const consumer<zdd::label_type> &cb);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Copy all of the variable labels (in \em ascending order) that occur
  ///        in the family into the given container.
  ///
  /// \param A     ZDD of interest.
  ///
  /// \param begin Single-pass forward iterator for where to place the output.
  ///
  /// \param end   Marks the end for `begin`.
  ///
  /// \returns     An iterator to the first entry that still is left empty.
  ///
  /// \throws out_of_range If the distance between `begin` and `end` is not big
  ///                      enough to contain all variables in `f`.
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  ForwardIt zdd_support(const zdd &A, ForwardIt begin, ForwardIt end)
  {
    zdd_support(A, make_consumer(begin, end));
    return begin;
  }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Get the root's variable label.
  ///
  /// \throws invalid_argument If `A` is a terminal.
  //////////////////////////////////////////////////////////////////////////////
  zdd::label_type zdd_topvar(const zdd &f);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Get the minimal occurring variable in the family.
  ///
  /// \throws invalid_argument If `A` is a terminal.
  //////////////////////////////////////////////////////////////////////////////
  zdd::label_type zdd_minvar(const zdd &A);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Get the maximal occurring variable in the family.
  ///
  /// \throws invalid_argument If `A` is a terminal.
  //////////////////////////////////////////////////////////////////////////////
  zdd::label_type zdd_maxvar(const zdd &A);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief   Whether the family includes the given set of labels.
  ///
  /// \param A Set of interest
  ///
  /// \param a Generator of a bit-vector in \em ascending order. All variables
  ///          geneated should be smaller than or equal to `zdd::max_label`.
  ///
  /// \returns Whether \f$ a \in A \f$
  //////////////////////////////////////////////////////////////////////////////
  bool zdd_contains(const zdd &A, const generator<zdd::label_type> &a);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief       Whether the family includes the given set of labels.
  ///
  /// \param A     Set of interest
  ///
  /// \param begin Single-pass forward iterator of the set of labels in
  ///              \em ascending order. All its values should be smaller than
  ///              or equals to `zdd::max_label`.
  ///
  /// \param end   Marks the end for `begin`.
  ///
  /// \returns Whether \f$ \{\mathit{begin}, \dots, \mathit{end}\} \in A \f$
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  bool zdd_contains(const zdd &A, ForwardIt begin, ForwardIt end)
  { return zdd_contains(A, make_generator(begin, end)); }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief   Retrieves the lexicographically smallest set a in A.
  ///
  /// \param A Set of sets of interest.
  ///
  /// \details Outputs the trace of the low-most path to the true terminal. The
  ///          resulting assignment is lexicographically smallest, where every
  ///          variable is treated as a digit and \f$ x_0 > x_1 > \dots \f$.
  //////////////////////////////////////////////////////////////////////////////
  zdd zdd_minelem(const zdd &A);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief    Retrieves the lexicographically smallest set a in A.
  ///
  /// \param A  Set of sets of interest.
  ///
  /// \param cb Callback function that is called with the variables of the
  ///           smallest set in \em ascending order of the  levels of `A`.
  ///
  /// \pre `A != zdd_empty()`
  //////////////////////////////////////////////////////////////////////////////
  void zdd_minelem(const zdd &A, const consumer<zdd::label_type> &cb);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief       Retrieves the lexicographically smallest set a in A.
  ///
  /// \param A     Set of sets of interest.
  ///
  /// \param begin Single-pass forward iterator for where to place the output.
  ///
  /// \param end   Marks the end for `begin`.
  ///
  /// \returns     An iterator to the first entry that still is left empty.
  ///
  /// \throws out_of_range If the distance between `begin` and `end` is not big
  ///                      enough to contain all variables in `f`.
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  ForwardIt zdd_minelem(const zdd &A, ForwardIt begin, ForwardIt end)
  {
    zdd_minelem(A, make_consumer(begin, end));
    return begin;
  }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief   Retrieves the lexicographically largest set a in A.
  ///
  /// \param A Set of sets of interest.
  ///
  /// \details Outputs the trace of the high-most path to the true terminal. The
  ///          resulting assignment is lexicographically largest, where every
  ///          variable is treated as a digit and \f$ x_0 > x_1 > \dots \f$.
  //////////////////////////////////////////////////////////////////////////////
  zdd zdd_maxelem(const zdd &A);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief    Retrieves the lexicographically largest set a in A.
  ///
  /// \param A  Set of sets of interest.
  ///
  /// \param cb Callback function that is called with the variables of the
  ///           largest set in \em ascending order of the  levels of `A`.
  ///
  /// \pre `A != zdd_empty()`
  //////////////////////////////////////////////////////////////////////////////
  void zdd_maxelem(const zdd &A, const consumer<zdd::label_type> &cb);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief       Retrieves the lexicographically largest set a in A.
  ///
  /// \param A     Set of sets of interest.
  ///
  /// \param begin Single-pass forward iterator for where to place the output.
  ///
  /// \param end   Marks the end for `begin`.
  ///
  /// \returns     An iterator to the first entry that still is left empty.
  ///
  /// \throws out_of_range If the distance between `begin` and `end` is not big
  ///                      enough to contain all variables in `f`.
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  ForwardIt zdd_maxelem(const zdd &A, ForwardIt begin, ForwardIt end)
  {
    zdd_maxelem(A, make_consumer(begin, end));
    return begin;
  }

  /// \}
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  /// \name Conversion to ZDDs
  ///
  /// \{

  //////////////////////////////////////////////////////////////////////////////
  /// \brief     Obtains the ZDD that represents the same function/set as the
  ///            given BDD within the given domain.
  ///
  /// \param f   Boolean function with the given domain
  ///
  /// \param dom Domain of all variables in \em ascending order. All generated
  ///            values should be smaller than or equals to `zdd::max_label`.
  ///
  /// \returns   ZDD that is true for the exact same assignments to variables in
  ///            the given domain.
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_from(const bdd &f, const generator<zdd::label_type> &dom);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Obtains the ZDD that represents the same function/set as the given
  ///        BDD within the given domain.
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_from(const exec_policy &ep,
                 const bdd &f,
                 const generator<zdd::label_type> &dom);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief       Obtains the ZDD that represents the same function/set as the
  ///              given BDD within the given domain.
  ///
  /// \param f     Boolean function with the given domain
  ///
  /// \param begin Single-pass forward iterator that provides the domain's
  ///              variables in \em ascending order. None of its values may
  ///              exceed `zdd::max_label`.
  ///
  /// \param end   Marks the end for `begin`.
  ///
  /// \returns     ZDD that is true for the exact same assignments to variables
  ///              in the given domain.
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  __zdd zdd_from(const bdd &f, ForwardIt begin, ForwardIt end)
  { return zdd_from(f, make_generator(begin, end)); }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Obtains the ZDD that represents the same function/set as the given
  ///        BDD within the given domain.
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  __zdd zdd_from(const exec_policy &ep,
                 const bdd &f,
                 ForwardIt begin,
                 ForwardIt end)
  { return bdd_from(ep, f, make_generator(begin, end)); }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief     Obtains the ZDD that represents the same function/set as the
  ///            given BDD within the global \ref module__domain.
  ///
  /// \param f   Boolean function with the given domain
  ///
  /// \returns   ZDD that is true for the exact same assignments to variables in
  ///            the global domain.
  ///
  /// \pre       The global \ref module__domain is set to a set of variables
  ///            that is equals to or a superset of the variables in `A`.
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_from(const bdd &f);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Obtains the BDD that represents the same function/set as the given
  ///        ZDD within the global domain.
  //////////////////////////////////////////////////////////////////////////////
  __zdd zdd_from(const exec_policy &ep, const bdd &f);

  /// \}
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  /// \name Dot Files of ZDDs
  ///
  /// \{

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Output a DOT drawing of a ZDD to the given output stream.
  //////////////////////////////////////////////////////////////////////////////
  void zdd_printdot(const zdd &A, std::ostream &out = std::cout);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Output a DOT drawing of a ZDD to the file with the given name.
  //////////////////////////////////////////////////////////////////////////////
  void zdd_printdot(const zdd &A, const std::string &file_name);

  /// \}
  //////////////////////////////////////////////////////////////////////////////

  /// \}
  //////////////////////////////////////////////////////////////////////////////
}

#endif // ADIAR_ZDD_H
