#ifndef ADIAR_EXEC_POLICY_H
#define ADIAR_EXEC_POLICY_H

namespace adiar
{
  //////////////////////////////////////////////////////////////////////////////
  /// \addtogroup module__adiar
  ///
  /// \{

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Settings to dictate the execution of Adiar's algorithms.
  ///
  /// Adiar's algorithms work very differently from other BDD implementations.
  /// Hence, it makes use of multiple novel techniques to make it competitive
  /// across the entire spectrum of BDD shapes and sizes. These can be turned
  /// on/off or tweaked by changing these settings.
  ///
  /// Most likely, you would want to apply all techniques (in a safe way) and so
  /// you do not need to think about changing any of these.
  ///
  /// For example, you can fix the `bdd_exists` to only use \em internal memory
  /// and the \em Nested \em Sweeping framework as follows:
  /// ```cpp
  /// bdd_exists(exec_policy::memory::Internal & exec_policy::quantify::Nested,
  ///            f,
  ///            vars.rbegin(), vars.rend());
  /// ```
  //////////////////////////////////////////////////////////////////////////////
  class exec_policy
  {
  public:
    ////////////////////////////////////////////////////////////////////////////
    /// \brief   Whether Adiar should exclusively use random access or priority
    ///          queues or automatically pick either way based on the width of
    ///          inputs.
    ///
    /// \details Adiar’s Algorithms delay recursion through use of auxiliary
    ///          data structures. These auxiliary data structures are redundant
    ///          when the width of one or more inputs fits in the internal
    ///          memory. In these cases it is much faster to load in the entire
    ///          level of a diagram and then use random access.
    ///
    /// \warning Using `Random_Access` may lead to crashes if all inputs are too
    ///          wide or input to random access is not canonical!
    ///
    /// \see bdd_apply zdd_binop
    ////////////////////////////////////////////////////////////////////////////
    enum class access
    {
      /** Pick \em Random Access when an input is narrow enough. */
      Auto,
      /** Always use Random Access */
      Random_Access,
      /** Always use Priority Queues */
      Priority_Queue
    };

    ////////////////////////////////////////////////////////////////////////////
    /// \brief   Whether Adiar should exclusively use internal or external
    ///          memory or automatically pick either type based on size of
    ///          input.
    ///
    /// \details Adiar’s Algorithms delay recursion through use of auxiliary
    ///          data structures. These auxiliary data structures can be
    ///          optimised for internal memory, and so have a high performance
    ///          on very small instances, or they can be designed for external
    ///          memory such that they can handle decision diagrams much larger
    ///          than the available memory.
    ///
    /// \note    For more details, please read "Predicting Memory Demands of BDD
    ///          Operations using Maximum Graph Cuts" ATVA 2023.
    ///
    /// \warning Using `Internal` may lead to crashes if an input or output is
    ///          too large!
    ////////////////////////////////////////////////////////////////////////////
    enum class memory
    {
      /** Pick \em internal memory as long as it is safe to do so. */
      Auto,
      /** Always use \em internal memory */
      Internal,
      /** Always use \em external memory */
      External
    };

    ////////////////////////////////////////////////////////////////////////////
    /// \brief   Strategies for Adiar to use in quantify/project algorithms.
    ///
    /// \details Adiar’s supports multiple approaches to compute the
    ///          quantification of multiple variables. While `Auto`
    ///          heuristically uses one or more of the three options, one can
    ///          choose to run the approach of particular interest
    ///
    /// \remark  Not all approaches can apply to each algorithm. If the desired
    ///          approach does not apply, then an approach that is *less* than
    ///          it is used instead. For example, `Partial` cannot be used with
    ///          a `generator` and so `Singleton` is used instead.
    ///
    /// \see bdd_exists bdd_forall zdd_project
    ////////////////////////////////////////////////////////////////////////////
    enum class quantify
    {
      /** Automatically decide what approach to use (may switch half-way). */
      Auto,
      /** Use the \em nested \em sweeping framework. */
      Nested,
      /** Use repeated \em partial \em quantification. */
      Partial,
      /** Quantify each variable independently. */
      Singleton
    };

    // TODO: Move Nested Sweeping constants/strategies in here too...

  private:
    // TODO: Merge all enums into a single 64 bit integer to safe on space?

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Chosen `access` (default `Auto`).
    ////////////////////////////////////////////////////////////////////////////
    access _access_mode = access::Auto;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Chosen `memory`  (default `Auto`).
    ////////////////////////////////////////////////////////////////////////////
    memory _memory_mode = memory::Auto;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Chosen `quantify` (default `Auto`).
    ////////////////////////////////////////////////////////////////////////////
    quantify _quantify_alg = quantify::Auto;

  public:
    ////////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor with all options set to their default value.
    ////////////////////////////////////////////////////////////////////////////
    exec_policy() = default;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Copy constructor.
    ////////////////////////////////////////////////////////////////////////////
    exec_policy(const exec_policy&) = default;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Move constructor.
    ////////////////////////////////////////////////////////////////////////////
    exec_policy(exec_policy&&) = default;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Conversion construction from `access` enum.
    ////////////////////////////////////////////////////////////////////////////
    exec_policy(const access &am)
      : _access_mode(am)
    { }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Conversion construction from `memory` enum.
    ////////////////////////////////////////////////////////////////////////////
    exec_policy(const memory &mm)
      : _memory_mode(mm)
    { }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Conversion construction from `quantify` enum.
    ////////////////////////////////////////////////////////////////////////////
    exec_policy(const quantify &qm)
      : _quantify_alg(qm)
    { }

    // TODO: constructor with defaults for a specific 'version number'?

  public:
    ////////////////////////////////////////////////////////////////////////////
    /// \brief Copy assignment
    ////////////////////////////////////////////////////////////////////////////
    exec_policy& operator =(const exec_policy &) = default;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Move assignment
    ////////////////////////////////////////////////////////////////////////////
    exec_policy& operator =(exec_policy &&) = default;

  public:
    ////////////////////////////////////////////////////////////////////////////
    /// \brief Chosen access mode.
    ////////////////////////////////////////////////////////////////////////////
    const access& access_mode() const
    { return _access_mode; }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Chosen memory type.
    ////////////////////////////////////////////////////////////////////////////
    const memory& memory_mode() const
    { return _memory_mode; }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Chosen quantification strategy.
    ////////////////////////////////////////////////////////////////////////////
    const quantify& quantify_alg() const
    { return _quantify_alg; }

  public:
    ////////////////////////////////////////////////////////////////////////////
    /// \brief Check for equality of settings.
    ////////////////////////////////////////////////////////////////////////////
    bool operator ==(const exec_policy& ep) const
    {
      // Order based from the most generic to the most specific setting.
      return this->memory_mode()  == ep.memory_mode()
          && this->access_mode()  == ep.access_mode()
          && this->quantify_alg() == ep.quantify_alg()
        ;
    }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Check for at least one mismatch in the settings.
    ////////////////////////////////////////////////////////////////////////////
    bool operator !=(const exec_policy& ep) const
    {
      return !(*this == ep);
    }

  public:
    ////////////////////////////////////////////////////////////////////////////
    /// \brief Set the access mode.
    ////////////////////////////////////////////////////////////////////////////
    exec_policy& set(const access &am)
    {
      this->_access_mode = am;
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Create a copy with the access mode changed.
    ////////////////////////////////////////////////////////////////////////////
    exec_policy operator &(const access& am)
    {
      exec_policy ep = *this;
      return ep.set(am);
    }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Set the memory mode.
    ////////////////////////////////////////////////////////////////////////////
    exec_policy& set(const memory &mm)
    {
      this->_memory_mode = mm;
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Create a copy with the memory mode changed.
    ////////////////////////////////////////////////////////////////////////////
    exec_policy operator &(const memory& mm)
    {
      exec_policy ep = *this;
      return ep.set(mm);
    }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Set the quantify strategy.
    ////////////////////////////////////////////////////////////////////////////
    exec_policy& set(const quantify &qs)
    {
      this->_quantify_alg = qs;
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Create a copy with the quantify strategy changed.
    ////////////////////////////////////////////////////////////////////////////
    exec_policy operator &(const quantify& qs)
    {
      exec_policy ep = *this;
      return ep.set(qs);
    }
  };

  /// \}
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Operator overload to support lifting enum values to `exec_policy`.
  //////////////////////////////////////////////////////////////////////////////
  inline exec_policy operator &(const exec_policy::access &am,
                                const exec_policy::memory &mm)
  { return exec_policy(am) & mm; }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Operator overload to support lifting enum values to `exec_policy`.
  //////////////////////////////////////////////////////////////////////////////
  inline exec_policy operator &(const exec_policy::memory &mm,
                                const exec_policy::access &am)
  { return exec_policy(mm) & am; }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Operator overload to support lifting enum values to `exec_policy`.
  //////////////////////////////////////////////////////////////////////////////
  inline exec_policy operator &(const exec_policy::access &am,
                                const exec_policy::quantify &qs)
  { return exec_policy(am) & qs; }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Operator overload to support lifting enum values to `exec_policy`.
  //////////////////////////////////////////////////////////////////////////////
  inline exec_policy operator &(const exec_policy::quantify &qs,
                                const exec_policy::access &am)
  { return exec_policy(qs) & am; }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Operator overload to support lifting enum values to `exec_policy`.
  //////////////////////////////////////////////////////////////////////////////
  inline exec_policy operator &(const exec_policy::memory &mm,
                                const exec_policy::quantify &qs)
  { return exec_policy(mm) & qs; }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Operator overload to support lifting enum values to `exec_policy`.
  //////////////////////////////////////////////////////////////////////////////
  inline exec_policy operator &(const exec_policy::quantify &qs,
                                const exec_policy::memory &mm)
  { return exec_policy(qs) & mm; }
}

#endif // ADIAR_EXEC_POLICY_H
