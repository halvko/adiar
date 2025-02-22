#ifndef ADIAR_INTERNAL_DATA_TYPES_NODE_H
#define ADIAR_INTERNAL_DATA_TYPES_NODE_H

#include <adiar/internal/assert.h>
#include <adiar/internal/data_types/ptr.h>
#include <adiar/internal/data_types/uid.h>
#include <adiar/internal/data_types/tuple.h>

namespace adiar::internal
{
  //////////////////////////////////////////////////////////////////////////////
  /// \brief A Decision Diagram node triple (uid, low, high).
  ///
  /// \remark A node contains a unique identifier for said node in `uid`
  ///         together with pointers to its children in `low` and `high`.
  ///
  /// \remark If a node is a terminal, then `low` and `high` are nil. Otherwise,
  ///         they are always \em not nil.
  //////////////////////////////////////////////////////////////////////////////
  class node
  {
  public:
    // TODO (MDD):
    //   change (with templating) the _children array below to be a std::vector.
    //   To get it to work with TPIE, use the serialization stream instead.

    // TODO (QMDD):
    //   template with 'outdegree' and use it with the value of '4u'. Reexpose
    //   the templated variable in the constexpr below, such that algorithms
    //   also can retrieve it.

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Number of children of this node type.
    ////////////////////////////////////////////////////////////////////////////
    static constexpr size_t outdegree = 2u;

    // TODO (ADD (64-bit)):
    //   template with 'uid_type' reexpose it (and its related 'ptr') with typedefs.

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Type of the pointer of this node's children.
    ////////////////////////////////////////////////////////////////////////////
    using pointer_type = ptr_uint64;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Type of this node's unique identifier.
    ////////////////////////////////////////////////////////////////////////////
    using uid_type = __uid<pointer_type>;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Type of terminal values.
    ////////////////////////////////////////////////////////////////////////////
    using terminal_type = pointer_type::terminal_type;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Type of this node's variable label.
    ////////////////////////////////////////////////////////////////////////////
    using label_type = pointer_type::label_type;

    //////////////////////////////////////////////////////////////////////////////
    /// \brief The maximal possible value for a unique identifier's label.
    //////////////////////////////////////////////////////////////////////////////
    static constexpr label_type max_label = pointer_type::max_label;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Type of this node's level identifier.
    ////////////////////////////////////////////////////////////////////////////
    using id_type = pointer_type::id_type;

    //////////////////////////////////////////////////////////////////////////////
    /// \brief The maximal possible value for this nodes level identifier.
    //////////////////////////////////////////////////////////////////////////////
    static constexpr id_type max_id = pointer_type::max_id;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Type of the children tuple.
    ////////////////////////////////////////////////////////////////////////////
    using children_type = tuple<pointer_type, outdegree>;

  private:
    // TODO (Attributed Edges):
    //   Add logic related to flag on children.

    uid_type _uid;
    children_type _children;

  public:
    ////////////////////////////////////////////////////////////////////////////
    /// \brief   Default construction (trivial).
    ///
    /// \details The default, copy, and move assignment has to be `default` to
    ///          ensure it is a *POD* and hence can be used by TPIE's files.
    ////////////////////////////////////////////////////////////////////////////
    node() = default;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief   Copy construction (trivial).
    ///
    /// \details The default, copy, and move constructor has to be `default` to
    ///          ensure it is a *POD* and hence can be used by TPIE's files.
    ////////////////////////////////////////////////////////////////////////////
    node(const node &n) = default;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief   Move construction (trivial).
    ///
    /// \details The default, copy, and move constructor has to be `default` to
    ///          ensure it is a *POD* and hence can be used by TPIE's files.
    ////////////////////////////////////////////////////////////////////////////
    node(node &&n) = default;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief   Destruction (trivial).
    ///
    /// \details The destructor has to be `default` to ensure it is a *POD* and
    ///          hence can be used by TPIE's files.
    ////////////////////////////////////////////////////////////////////////////
    ~node() = default;

  public:
    ////////////////////////////////////////////////////////////////////////////
    /// \brief   Copy assignment (trivial).
    ///
    /// \details The copy and move assignment has to be `default` to
    ///          ensure it is a *POD* and hence can be used by TPIE's files.
    ////////////////////////////////////////////////////////////////////////////
    node& operator =(const node &n) = default;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief   Move assignment (trivial).
    ///
    /// \details The copy and move assignment has to be `default` to ensure it
    ///          is a *POD* and hence can be used by TPIE's files.
    ////////////////////////////////////////////////////////////////////////////
    node& operator =(node &&n) = default;

  public:
    ////////////////////////////////////////////////////////////////////////////
    /// \brief The unique identifier of this node
    ////////////////////////////////////////////////////////////////////////////
    inline uid_type uid() const
    { return _uid; }

    /* ============================== TERMINAL NODE ========================= */
  public:
    // Provide 'non-default' constructors to make it easy to use outside of TPIE.

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Construct *terminal* node `(value, nil, nil)`.
    ////////////////////////////////////////////////////////////////////////////
    node(const terminal_type value)
      : _uid(pointer_type(value)), _children(ptr_uint64::nil())
    { }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Whether this node represents a terminal value.
    ////////////////////////////////////////////////////////////////////////////
    inline bool is_terminal() const
    { return _uid.is_terminal(); }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The value of this terminal node (assuming it is one).
    ///
    /// \pre `is_terminal()` evaluates to `true`.
    ////////////////////////////////////////////////////////////////////////////
    inline terminal_type value() const
    {
      adiar_assert(is_terminal());
      return _uid.value();
    }

    //////////////////////////////////////////////////////////////////////////////
    /// \brief Whether this node is the false terminal.
    ///
    /// \details This is equivalent to `n.is_terminal() && !n.value()`.
    //////////////////////////////////////////////////////////////////////////////
    inline bool is_false() const
    { return uid().is_false(); }

    //////////////////////////////////////////////////////////////////////////////
    /// \brief Whether this node is the true terminal.
    ///
    /// \details This is equivalent to `n.is_terminal() && n.value()`.
    //////////////////////////////////////////////////////////////////////////////
    inline bool is_true() const
    { return uid().is_true(); }

    /* ============================== INTERNAL NODE ========================= */
  public:
    // Provide 'non-default' constructors to make it easy to use outside of TPIE.

    // TODO (QMDD):
    //   Add generic version of the two constructors below that takes a
    //   'children_type' as argument.

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Construct node `(uid, low, high)`.
    ////////////////////////////////////////////////////////////////////////////
    node(const uid_type &u, const pointer_type &l, const pointer_type &h)
      : _uid(u), _children{l, h}
    { }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Construct *internal* node `((label, id), low, high)`.
    ////////////////////////////////////////////////////////////////////////////
    node(const label_type label, const id_type id, const pointer_type &l, const pointer_type &h)
      : _uid(label, id), _children{l, h}
    {
      adiar_assert(!l.is_nil(), "Cannot create a node with nil child");
      adiar_assert(l.is_terminal() || label < l.label(),
                   "Node is not prior to given low child");

      adiar_assert(!h.is_nil(), "Cannot create a node with nil child");
      adiar_assert(h.is_terminal() || label < h.label(),
                   "Node is not prior to given high child");
    }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Construct *internal* node `((label, id), low, high)`.
    ////////////////////////////////////////////////////////////////////////////
    node(const label_type label, const id_type id, const node &l, const pointer_type &h)
      : node(label, id, l.uid(), h)
    {
      adiar_assert(outdegree == 2, "Constructor is for binary node only.");
    }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Construct *internal* node `((label, id), low, high)`.
    ////////////////////////////////////////////////////////////////////////////
    node(const label_type label, const id_type id, const pointer_type &l, const node &h)
      : node(label, id, l, h.uid())
    {
      adiar_assert(outdegree == 2, "Constructor is for binary node only.");
    }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Construct *internal* node `((label, id), low, high)`.
    ////////////////////////////////////////////////////////////////////////////
    node(const label_type label, const id_type id, const node &l, const node &h)
      : node(label, id, l.uid(), h.uid())
    {
      adiar_assert(outdegree == 2, "Constructor is for binary node only.");
    }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Obtain the label of a node.
    ///
    /// \pre `is_terminal()` evaluates to `false`.
    ////////////////////////////////////////////////////////////////////////////
    // TODO: Rename to `level()` when introducing variable ordering
    inline label_type label() const
    {
      adiar_assert(!is_terminal());
      return uid().label();
    }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Obtain the level-specific identifier of a node.
    ///
    /// \pre `is_terminal()` evaluates to `false`.
    ////////////////////////////////////////////////////////////////////////////
    inline id_type id() const
    {
      adiar_assert(!is_terminal());
      return uid().id();
    }

    /* ================================= CHILDREN =========================== */
  public:
    ////////////////////////////////////////////////////////////////////////////
    /// \brief The node's children sorted based on the semantics of this node.
    ////////////////////////////////////////////////////////////////////////////
    inline const children_type& children() const
    { return _children; }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The node's i'th child (sorted based on the semantics).
    ////////////////////////////////////////////////////////////////////////////
    inline pointer_type child(const size_t i) const
    {
      adiar_assert(i < outdegree, "'i' must be a valid children index.");
      return _children[i];
    }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The 'low' child (also known as the 'else' child), i.e. reflecting
    ///        assigning `false` to variable with the 'label'.
    ///
    /// \details This is similar to writing `.child(false)`.
    ///
    /// \see child
    ////////////////////////////////////////////////////////////////////////////
    inline pointer_type low() const
    {
      adiar_assert(outdegree == 2,
                   "Semantics of 'low' is only defined for binary nodes.");

      return child(false);
    }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The 'high' child (also known as the 'then' child), i.e.
    ///        reflecting assigning `true` to variable with the 'label'.
    ///
    /// \details This is similar to writing `.child(true)`.
    ///
    /// \see child
    ////////////////////////////////////////////////////////////////////////////
    inline pointer_type high() const
    {
      adiar_assert(outdegree == 2,
                   "Semantics of 'high' is only defined for binary node.");

      return child(true);
    }

    /* =============================== COMPARATORS ========================== */
  public:

    //////////////////////////////////////////////////////////////////////////////
    /// \brief Sorting of node based based on its `uid()` (i.e. its time-stamp).
    ///        If they are the same, then it secondly is sorted on its cildren.
    //////////////////////////////////////////////////////////////////////////////
    inline bool operator<  (const node &o) const
    {
      return (this->_uid < o._uid)
        || (this->_uid == o._uid && this->_children < o._children);
    }

    //////////////////////////////////////////////////////////////////////////////
    /// \brief Sorting of node based based on its `uid()` (i.e. its time-stamp).
    ///        If they are the same, then it secondly is sorted on its cildren.
    //////////////////////////////////////////////////////////////////////////////
    inline bool operator>  (const node &o) const
    { return (o < *this); }

    //////////////////////////////////////////////////////////////////////////////
    /// \brief Check uid and children match.
    //////////////////////////////////////////////////////////////////////////////
    inline bool operator== (const node &o) const
    {
      return this->_uid == o._uid
        && this->_children[false] == o._children[false]
        && this->_children[true]  == o._children[true];
    }

    //////////////////////////////////////////////////////////////////////////////
    /// \brief Check either uid or one of the children mismatch.
    //////////////////////////////////////////////////////////////////////////////
    inline bool operator!= (const node &o) const
    { return !(*this == o); }

    //////////////////////////////////////////////////////////////////////////////
    /// \brief Sorting of node based based on its `uid()` (i.e. its time-stamp).
    ///        If they are the same, then it secondly is sorted on its cildren.
    //////////////////////////////////////////////////////////////////////////////
    inline bool operator<= (const node &o) const
    { return (*this < o) || (*this == o); }

    //////////////////////////////////////////////////////////////////////////////
    /// \brief Sorting of node based based on its `uid()` (i.e. its time-stamp).
    ///        If they are the same, then it secondly is sorted on its cildren.
    //////////////////////////////////////////////////////////////////////////////
    inline bool operator>= (const node &o) const
    { return (*this > o) || (*this == o); }

    /* ================================ OPERATORS =========================== */
  public:
    ////////////////////////////////////////////////////////////////////////////
    /// \brief Create the node representing the (locally) negated function:
    ///        pointers to terminal children are negated while pointers to other
    ///        nodes are left unchanged.
    ////////////////////////////////////////////////////////////////////////////
    node operator! () const
    {
      if (this->is_terminal()) {
        return node(!this->_uid, pointer_type::nil(), pointer_type::nil());
      }
      return node(this->_uid, !this->_children[false], !this->_children[true]);
    }
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Ordering of node in comparison to a unique identifier.
  //////////////////////////////////////////////////////////////////////////////
  inline bool operator< (const node &n, const node::uid_type &u)
  { return n.uid() < u; }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Ordering of node in comparison to a unique identifier.
  //////////////////////////////////////////////////////////////////////////////
  inline bool operator< (const node::uid_type &u, const node &n)
  { return u < n.uid(); }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Ordering of node in comparison to a unique identifier.
  //////////////////////////////////////////////////////////////////////////////
  inline bool operator<= (const node &n, const node::uid_type &u)
  { return n.uid() <= u; }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Ordering of node in comparison to a unique identifier.
  //////////////////////////////////////////////////////////////////////////////
  inline bool operator<= (const node::uid_type &u, const node &n)
  { return u <= n.uid(); }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Ordering of node in comparison to a unique identifier.
  //////////////////////////////////////////////////////////////////////////////
  inline bool operator> (const node &n, const node::uid_type &u)
  { return n.uid() > u; }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Ordering of node in comparison to a unique identifier.
  //////////////////////////////////////////////////////////////////////////////
  inline bool operator> (const node::uid_type &u, const node &n)
  { return u > n.uid(); }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Ordering of node in comparison to a unique identifier.
  //////////////////////////////////////////////////////////////////////////////
  inline bool operator>= (const node &n, const node::uid_type &u)
  { return n.uid() >= u; }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Ordering of node in comparison to a unique identifier.
  //////////////////////////////////////////////////////////////////////////////
  inline bool operator>= (const node::uid_type &u, const node &n)
  { return u >= n.uid(); }
}

#endif // ADIAR_INTERNAL_DATA_TYPES_NODE_H
