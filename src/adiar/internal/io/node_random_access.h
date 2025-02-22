#ifndef ADIAR_INTERNAL_IO_NODE_RANDOM_ACCESS_H
#define ADIAR_INTERNAL_IO_NODE_RANDOM_ACCESS_H

#include <adiar/internal/assert.h>
#include <adiar/internal/data_types/node.h>
#include <adiar/internal/io/levelized_file.h>
#include <adiar/internal/io/levelized_file_stream.h>

namespace adiar::internal
{
  // TODO: Generalize parts of 'node_random_access' to reuse it with levelized
  // files with other types of content. Yet, what use-case do we have for this?

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Random-access to the contents of a levelized file of node.
  //////////////////////////////////////////////////////////////////////////////
  template<typename T = node, bool Reverse = false>
  class node_random_access
  {
    using value_type = T;

    static_assert(!Reverse, "Reversed logic is not implemented.");

    using uid_type   = typename value_type::uid_type;
    using label_type = typename value_type::label_type;
    using id_type    = typename value_type::id_type;

  public:
    static size_t memory_usage(tpie::memory_size_type max_width)
    {
      return node_stream<Reverse>::memory_usage()
           + tpie::array<value_type>::memory_usage(max_width);
    }

    static size_t memory_usage(const dd &diagram)
    {
      return node_stream<Reverse>::memory_usage()
        + tpie::array<value_type>::memory_usage(diagram->width);
    }

  public:
    ////////////////////////////////////////////////////////////////////////////
    /// \brief Value to mark there is no current level.
    ////////////////////////////////////////////////////////////////////////////
    static constexpr label_type no_level = -1;

    static_assert(uid_type::max_label < no_level,
                  "'no_level' should be an invalid label value");

  private:
    ////////////////////////////////////////////////////////////////////////////
    /// \brief File stream to obtain the contents of each level
    ////////////////////////////////////////////////////////////////////////////
    node_stream<Reverse> _ns;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Maximum width of the contents of 'lfs'. This is the maximum
    /// number of elements needed to be placed within.
    ////////////////////////////////////////////////////////////////////////////
    const id_type _max_width;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Buffer with all elements of the current level.
    ////////////////////////////////////////////////////////////////////////////
    tpie::array<value_type> _level_buffer;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Buffer with all elements of the current level.
    ////////////////////////////////////////////////////////////////////////////
    label_type _curr_level = no_level;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Width of the current level.
    ////////////////////////////////////////////////////////////////////////////
    id_type _curr_width = 0;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Root of the diagram.
    ////////////////////////////////////////////////////////////////////////////
    uid_type _root;

    // TODO: Add canonicity flag to discern whether to compute the index (as
    //       now) or to use binary search.

  public:
    ////////////////////////////////////////////////////////////////////////////
    // node_random_access()
    // { }
    //
    // TODO: Add 'attach(...)', 'attached()' 'detach()' working multi-usage.

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Construct attached to a levelized file of nodes.
    ///
    /// \pre The given levelized file is canonical.
    ////////////////////////////////////////////////////////////////////////////
    node_random_access(const levelized_file<value_type> &f,
                       const bool negate = false)
      : _ns(f, negate), _max_width(f.width), _level_buffer(f.width)
    {
      adiar_assert(f.canonical);
      init();
    }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Construct attached to a shared levelized file of nodes.
    ///
    /// \pre The given shared levelized file is canonical.
    ////////////////////////////////////////////////////////////////////////////
    node_random_access(const shared_ptr<levelized_file<value_type>> &f,
                       const bool negate = false)
      : _ns(f, negate), _max_width(f->width), _level_buffer(f->width)
    {
      adiar_assert(f->canonical);
      init();
    }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Construct attached to a decision diagram.
    ///
    /// \pre The given decision diagram is canonical.
    ////////////////////////////////////////////////////////////////////////////
    node_random_access(const dd &diagram)
      : _ns(diagram), _max_width(diagram->width), _level_buffer(diagram->width)
    {
      adiar_assert(diagram->canonical);
      init();
    }

  private:
    void init()
    {
      adiar_assert(_ns.can_pull(), "given file should be non-empty");

      // Skip the terminal node for terminal only BDDs.
      _root = _ns.peek().uid();
      if (_root.is_terminal()) {
        _ns.pull();
      }
    }

  public:
    ////////////////////////////////////////////////////////////////////////////
    /// \brief Return root of the diagram.
    ////////////////////////////////////////////////////////////////////////////
    uid_type root() const
    { return _root; }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Whether there are any more levels.
    ////////////////////////////////////////////////////////////////////////////
    bool has_next_level() const
    { return _ns.can_pull(); }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The immediate next available non-empty level.
    ///
    /// \pre `has_next_level() == true`
    ////////////////////////////////////////////////////////////////////////////
    label_type next_level()
    { return _ns.peek().uid().label(); }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Sets up the random access buffer for the specified level.
    ///
    /// \param level The next level to provide random access to. If the requested
    ///              level does not exist, then the buffer will be empty.
    ///
    /// \pre `has_current_level() == false` or `current_level() < level`
    ////////////////////////////////////////////////////////////////////////////
    void setup_next_level(const label_type level)
    {
      adiar_assert(!has_current_level() || current_level() < level);

      // Set to new level and mark the entire buffer's content garbage.
      _curr_level = level;
      _curr_width = 0;

      // Stop early when going "beyond" the available levels
      if (!has_next_level()) { return; }

      // Skip all levels not of interest
      while (_ns.can_pull() && _ns.peek().uid().label() < level) {
        _ns.pull();
      }

      // Copy over all elements from the requested level
      while (_ns.can_pull() && _ns.peek().uid().label() == level) {
        _level_buffer[_curr_width++] = _ns.pull();
      }
    }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Sets up the random access buffer for the immediate next available
    ///        non-empty level.
    ////////////////////////////////////////////////////////////////////////////
    void setup_next_level()
    { setup_next_level(next_level()); }

  public:
    ////////////////////////////////////////////////////////////////////////////
    /// \brief Whether there is any current level to access elements from.
    ////////////////////////////////////////////////////////////////////////////
    bool has_current_level() const
    { return _curr_level != no_level; }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The label of the current level.
    ////////////////////////////////////////////////////////////////////////////
    label_type current_level() const
    { return _curr_level; }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The width of the current level, i.e. the number of elements one
    ///        can access to.
    ////////////////////////////////////////////////////////////////////////////
    label_type current_width() const
    { return _curr_width; }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Whether the current level is empty.
    ////////////////////////////////////////////////////////////////////////////
    bool empty_level() const
    { return _curr_width == 0u; }

  public:
    ////////////////////////////////////////////////////////////////////////////
    /// \brief Obtain the element of the current level at the given index.
    ///
    /// \pre `idx < current_width()`
    ////////////////////////////////////////////////////////////////////////////
    const value_type& at(id_type idx) const
    {
      adiar_assert(idx < current_width());
      return _level_buffer[idx];
    }

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Obtain the node with the given uid.
    ////////////////////////////////////////////////////////////////////////////
    const value_type& at(uid_type u) const
    {
      adiar_assert(u.label() == current_level());

      // adiar_assert(... < current_width()); is in 'return at(...)'
      return at(current_width() - ((uid_type::max_id + 1u) - u.id()));
    }
  };
}

#endif // ADIAR_INTERNAL_IO_NODE_RANDOM_ACCESS_H
