#ifndef ADIAR_DOMAIN_H
#define ADIAR_DOMAIN_H

////////////////////////////////////////////////////////////////////////////////
/// \defgroup module__domain Variable Domain
///
/// \brief Global domain of variables.
///
/// Some operations relate to the entire variable domain. Instead of passing
/// this around explicitly, you may set it once and then Adiar will take care of
/// using it when needed.
////////////////////////////////////////////////////////////////////////////////

#include <adiar/exception.h>
#include <adiar/functional.h>

#include <adiar/internal/io/file.h>
#include <adiar/internal/io/shared_file_ptr.h>

// TODO: Make 'domain_var' independent of node type. Then remove this include.
#include <adiar/internal/data_types/node.h>

namespace adiar
{
  //////////////////////////////////////////////////////////////////////////////
  /// \addtogroup module__domain
  ///
  /// \{

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The variable type of a domain variable.
  //////////////////////////////////////////////////////////////////////////////
  using domain_var = internal::node::label_type;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The maximum supported domain variable.
  //////////////////////////////////////////////////////////////////////////////
  constexpr domain_var domain_max = internal::node::max_label;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Set the domain globally for all of Adiar to be [*0, varcount*).
  //////////////////////////////////////////////////////////////////////////////
  void domain_set(const domain_var varcount);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Set the domain globally for all of Adiar to be the variables
  ///        produced by the given generator function.
  ///
  /// \param dom Generator that produces variables to be quantified in
  ///            \em ascending order. When none are left, it must return a value
  ///            greater than `domain_max`.
  //////////////////////////////////////////////////////////////////////////////
  void domain_set(const generator<domain_var> &dom);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Set the domain globally for all of Adiar to be the variables
  ///        in the given range of iterators.
  ///
  /// \param begin Single-pass forward iterator that provides the domain
  ///               variables in \em ascending order.
  ///
  /// \param end   Marks the end for `begin`.
  //////////////////////////////////////////////////////////////////////////////
  template<typename ForwardIt>
  void domain_set(ForwardIt begin, ForwardIt end)
  { return domain_set(make_generator(begin, end)); }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief     Set the domain globally for all of Adiar.
  ///
  /// \details   This may be usable, if one needs to change the domain, but
  ///            wants to set it back again to the prior value without recreating
  ///            the entire file.
  ///
  /// \param dom A \ref shared_file containing all labels of the problem domain
  ///            in ascending order.
  ///
  /// \see domain_get()
  //////////////////////////////////////////////////////////////////////////////
  void domain_set(const internal::shared_file<domain_var> &dom);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief     Removes any globally shared domain variables (if any).
  ///
  /// \see domain_set
  //////////////////////////////////////////////////////////////////////////////
  void domain_unset();

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Whether Adiar has a global domain.
  //////////////////////////////////////////////////////////////////////////////
  bool domain_isset();

  //////////////////////////////////////////////////////////////////////////////
  /// \brief   Returns the global domain.
  ///
  /// \details This may be usable, if one needs to change the domain, but
  ///          wants to set it back again to the prior value without recreating
  ///          the entire file.
  ///
  /// \pre `domain_isset() == true`
  ///
  /// \throws domain_error If no domain is set, i.e. `domain_isset() == false`.
  ///
  /// \see domain_set(const shared_file<domain_var> &dom)
  //////////////////////////////////////////////////////////////////////////////
  internal::shared_file<domain_var> domain_get();

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The size of the domain.
  ///
  /// \details Returns `0` if `domain_isset() == false`.
  //////////////////////////////////////////////////////////////////////////////
  domain_var domain_size();

  /// \}
  //////////////////////////////////////////////////////////////////////////////
}

#endif // ADIAR_DOMAIN_H
