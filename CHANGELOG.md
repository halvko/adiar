# v2.0.0

**Work in Progress**

## New Features

- Added new overloads for many functions of Adiar's API that use [1] (pure)
  label *predicates*, [2] (stateful) label *generator* functions, and [3]
  *iterators*. For example, `bdd_exists` is now overloaded with the following
  alternatives

  - `bdd_exists(f, pred)`: quantify all variables *i* in *f* where *pred(i)*
    evaluates to true.

  - `bdd_exists(f, gen)`: quantify all variables *i* generated by *gen()*. Here,
    *gen()* must provides the to-be quantified variables in descending order.

  - `bdd_exists(f, begin, end)`: quantify all variables *i* from *begin* to
    *end* (assuming these are sorted in descending order).

- Similarly, functions that create non-diagram outputs, e.g. `bdd_satmin(...)`
  and `bdd_varprofile(...)`, now take a *consumer* function or iterators as an
  argument, such that they are compatible with any data structure you desire.

- `domain_set(...)` is overloaded to set the global domain given a number of
  variables, a *generator*, or an *iterator*.

- BDD and ZDD operations now optionally take an `exec_policy` as their first
  argument. This can be used to set parameters for the algorithms to non-default
  values.

- The library's version number can be found in *<adiar/version.h>* provides as
  compile-time known and inlinable integers and strings.

### Binary Decision Diagrams

- The functions `bdd_and(...)` and `bdd_or(...)` now support negated variables.
  This can either be parsed as a boolean flag or by negating the label.

- Added `bdd_iscube(f)` predicate together with `bdd_cube(...)` as an alias for
  `bdd_and(...)`.

- Added `bdd_isvar(f)`, `bdd_isithvar(f)`, `bdd_isnithvar(f)` predicates.

- Added `bdd_topvar(f)` to obtain the root-variable (similar to `bdd_minvar(f)`
  until variable reordering is introduced).

- Added `bdd_restrict(f, x, v)` as an overload to restrict a single variable.

- Added `bdd_low(f)` and `bdd_high(f)` as an overload of `bdd_restrict` to
  restrict the *top* variable to *false* or *true*, respectively.

- Added `bdd_const(bool_value)` and `bdd_isconst(f)` as a preferred alias for
  `bdd_terminal()` and `bdd_isterminal()`.

- Added `bdd_top()` and `bdd_bot()` as aliases for `bdd_true()` and
  `bdd_false()`.

### Zero-suppressed Decision Diagrams

- Added `zdd_ispoint(A)` predicate together with `zdd_point(...)` as an alias
  for `zdd_vars(...)`.

- Added `zdd_top(...)` and `zdd_bot(...)` as a ZDD version of `bdd_top()` and
  `bdd_bot()`.

- Added `zdd_topvar(A)` to obtain the root-variable (similar to `zdd_minvar(A)`
  until variable reordering is introduced).

- `zdd_onset(A, ...)` and `zdd_offset(A, ...)` are now overloaded for a single
  variable. By default, this single variable is the *top* variable.

## Optimisations

- The algorithms `bdd_apply`, `zdd_binop`, and their derivatives now use a
  level-by-level random access on one of the input (if possible). This
  circumvents using an entire priority queue, thereby drastically improving
  performance. This is especially beneficial when applying an operator to a very
  large decision diagram together with a narrow one.

  With `exec_policy::access` one can turn this optimisation off and/or force it
  to always be used.

- The functions `bdd_equal` and `zdd_equal` terminate in constant time, if the
  width of the two decision diagrams are not the same.

- Added proper support for quantification of multiple variable. To this end, we
  have implemented three separate algorithms. One can use
  `exec_policy::quantify` to pick between the three:

  1. The *v1.x* approach of quantifying a single variable at a time is still
     available (`exec_policy::quantify::Singleton`).

  2. The *v1.x* algorithm has been generalized to (partially) handle multiple
     variables at once in a single sweep (`exec_policy::quantify::Partial`).
     Furthermore, this generalized sweep can be rerun on its own unreduced
     result (skipping the possibly costly and unecessary Reduce sweep).

  3. Implemented the *nested sweeping* framework to add support for an
     I/O-efficient simulation of BDD operations that (may) recurse on
     intermediate results. This provides a completely new multi-variable
     quantification operations that is similar to the one in other BDD packages
     (`exec_policy::quantify::Nested`).

  By default, one or more of the above algorithms are used in conjunction
  (`exec_policy::quantify::Auto`).

## Bug Fixes

- The result of `statistics_get()` is now fixed such that the values for
  *reduce* and *substitute* are correct.

- Many small fixes to make Adiar compile with GCC, Clang, and MSVC on Linux,
  Mac, and Windows. Simultaneously, we have now set up continuous integration,
  such that we can truly ensure we support all platforms.

## Breaking Changes

There has been a major rewrite of the internal logic of Adiar to pay off
technical debt and to get Adiar ready for an I/O-efficient implementation of the
much more complex BDD operations, e.g. multi-variable quantification. At the
same time, this also brings Adiar much closer to support other types of decision
diagrams (e.g. QMDDs) and to store BDDs on disk and loading them again later.

- All internal logic in *<adiar/internal/...>* has been moved into its nested
  namespace `adiar::internal`. If you use anything from this namespace (e.g. the
  *node* and the *node_writer* classes) then you are not guaranteed non-breaking
  changes.

- The entire public API has been overhauled to have a consistent
  `{prefix}_{nocase}` naming scheme. For example, `adiar::adiar_set_domain(...)`
  has been renamed into `adiar::domain_set(...)`.

- Files, streams and their writers have been moved, rewritten, and renamed.
  Since newly added *predicates* and *generator* functions superseed any use of
  `adiar::file<...>`, all file-based overloads have been removed.

That is, this rewrite only results in breaking changes if you have been
interacting with Adiar's files directly, e.g. you have used the `bdd_restrict`,
`bdd_exists`, `bdd_forall` functions or have created BDDs by hand with the
*node_writer*.

Other breaking changes are:

- The functions of the entire public API now follows a common and consistent
  naming scheme of `{prefix}_{nocase}`. To this end, for example `is_true(f)`
  has been renamed into `bdd_istrue(f)`.

- `zdd_ithvar(i)`
  - The semantics have been changed to be more akin to the BDD semantics. That
    is, `zdd_ithvar(i)` is the set of all bitvectors where *i* is true.
    Symmetrically, the `zdd_nithvar(i)` function has been added.

  - The original semantics of `zdd_ithvar` is still provided with the
    `zdd_singleton` function.

- `bdd_satmin(f)` and `bdd_satmax(f)`, resp. `zdd_minelem(A)` and `zdd_maxelem(A)`:
  - The return type when called without a *consumer* or iterators has been
    turned into a `bdd`, resp. `zdd`. This ought to make their return type more
    usable in a symbolic context.

  - Semantics has changed such that it only reports the variables on the path
    traversed in the decision diagram. That is, if a level is skipped on some
    path, then that variable is also skipped in the output.

- Renamed `bdd_varprofile(f, ...)` to `bdd_support(f, ...)`, resp.
  `zdd_varprofile(A, ...)` to `zdd_support(A, ...)`. This improves its
  discoverability and better reflects its content.

- The `memory_mode` enum introduced in *v1.2* on/off has been moved to
  `exec_policy::memory`.

- All deprecated function from *v1.x* have been removed.

- `bdd_counter(...)` and `zdd_sized_set` have been removed.

- The two-level granularity of statistics has been removed. If you want to
  compile Adiar with statistics you just have to set the CMake variable
  `ADIAR_STATS` to *ON*; the CMake variable `ADIAR_STATS_EXTRA` has been removed
  as its highly detailed statistics is now included within `ADIAR_STATS`.

## License
Adiar 2.0.0 is distributed under the MIT license. But, notice that it depends on
the TPIE library which is licensed under LGPL v3. So, by extension any binary
file of Adiar is covered by the very same license.


# v1.2.2

**Date: 14th of November, 2022**

## Bug Fixes

- `zdd_project(A, dom)`
  does not generate recursions for the wrong ZDD node due to an unfortunate
  swapping of arguments.

- Fixes C++ and CMake such that Adiar compiles and runs on Mac computers with
  default Clang.


# v1.2.1

**Date: 14th of September, 2022**

## New Features

### Binary Decision Diagrams

- `bdd_satcount(bdd f)`
  If the global domain is set, then that value will take precedence over the
  number of levels in *f* (assuming *f* has fewer levels than the domain claims
  to exist).

- `bdd_printdot(bdd f, std::ostream out)`
  Added to allow more flexibility when outputting DOT files.

### Zero-suppressed Decision Diagrams

- `zdd_printdot(zdd A, std::ostream out)`
  Added to allow more flexibility when outputting DOT files.

### Documentation

Instead of separate Markdown files, the documentation is generated directly from
the C++ codebase with Doxygen. You can generate the documentation (assuming
Doxygen is intalled) with the `docs` Makefile target.

## Bug Fixes

- `adiar_printstats()`
  now prints levelized priority queue statistics even if only the unbucketed
  priority queue has been used.

## Deprecations

- *adiar/bdd.h*
  - `output_dot(bdd f, std::string file_name)` -> `bdd_printdot(bdd f, std::string file_name)`
- *adiar/zdd.h*
  - `output_dot(zdd A, std::string file_name)` -> `zdd_printdot(zdd A, std::string file_name)`


# v1.2.0

**Date: 29th of July, 2022**

## New Features

- A *file<label_t>* can globally be set as the problem domain, i.e. the set of
  variables you are using.

  - `adiar_set_domain(file<label_t> dom)`
     sets the global domain variable.
  - `adiar_has_domain()`
     checks whether a global domain already is set.
  - `domain_get()`
     provides the current *file<label_t>* that acts as the global domain
     (assuming `adiar_has_domain()` evaluates to `true`).

- `bdd_builder` and `zdd_builder`
  is a new class to replace using the *node_writer* directly. This enables a
  much more natural construction of BDDs and ZDDs bottom-up by hiding away
  several details. Furthermore, it makes use of exceptions rather than
  immediately terminating assertions.

### Binary Decision Diagrams

- `bdd_from(A)`
  converts from a ZDD to a BDD using the global domain.

- `bdd_equal(f, g)`
  is an alternative to using the `==` operator.

- `bdd_unequal(f, g)`
  is an alternative to using the `!=` operator.

- `bdd_varprofile(f)`
   obtains a *file<label_t>* containing all of the variables present in a BDD.

### Zero-suppressed Decision Diagrams

- `zdd_complement(A)`
  computes the complement within the global domain.

- `zdd_from(f)`
  converts from a BDD to a ZDD using the global domain.

- `zdd_varprofile(A)`
  obtains a *file<label_t>* containing all of the variables present in a ZDD.

### Statistics

- Statistics have been heavily extended with information on how often each type
  of auxiliary data structures (internal or external) have been used.

- All statistics variables are now fixed-precision numbers (using the [CNL
  library](https://github.com/johnmcfarlane/cnl)) making sure there are no
  overflows in the provided numbers.

- `adiar_statsreset()`
  resets all statistics values back to 0.

## Optimisations

The primary focus of this release is to drastically improve performance for
smaller instances. To this end, we still use the very same algorithms, but if
the input is small enough then we can safely use purely internal memory
auxiliary data structures within each algorithm.

Prior to this, we could only guarantee up to a comparable performance to other
BDD packages when they involve lots of large decision diagrams. Now, we can
lower the threshold for Adiar being useful by several orders of magnitude!

## Bug Fixes

- Results from `bdd_nithvar(label_t i)`, `bdd_ithvar(label_t i)`, and
  `zdd_ithvar(label_t i)` are now marked as *canonical* and so can be used with
  the linear-scan equality checking.

- Fixed the reduction phase may use 2 MiB more memory than is available.

- Fixed fine grained statistics (`ADIAR_STATS_EXTRA`) are turned on even though
  only the coarse-grained statistics (`ADIAR_STATS`) was desired.

- DOT files of ZDDs now print terminals as *Ø* and *{Ø}*.

## Deprecations

The word *sink* has been replaced with the word *terminal* that is more commonly
used in the context of decision diagrams.

- *adiar/data.h*
  - `create_sink_uid(bool val)` -> `create_terminal_uid(bool val)`
  - `create_sink_ptr(bool val)` -> `create_terminal_ptr(bool val)`
  - `create_sink(bool val)` -> `create_terminal(bool val)`

- *adiar/bdd.h*
  - `is_sink(bdd f)` -> `is_terminal(bdd f)`
  - `bdd_sink(bool val)` -> `bdd_terminal(bool val)`

- *adiar/zdd.h*
  - `is_sink(zdd A)` -> `is_terminal(zdd A)`
  - `zdd_sink(bool val)` -> `zdd_terminal(bool val)`

## Breaking Changes

The *terminal predicates* `is_any`, `is_true` and `is_false` with the prior
`is_sink(zdd A, pred)` functions were too complicated. The above performance
improvements allows us for a much simpler (and faster) implementation.
Deprecation was not possible due to name conflicts with their replacements
below.

- *adiar/data.h*
  - `is_sink(ptr_t p)`, `is_true(ptr_t p)`, and `is_false(ptr_t p)`.

- *adiar/bdd.h*
  - `is_sink(bdd f)`, `is_true(bdd f)` and `is_false(bdd f)`.

- *adiar/zdd.h*
  - `is_sink(zdd A)`, `is_null(zdd A)` and `is_empty(zdd A)`.


# v1.1.0

**Date: 25th of January, 2022**

## New Features

### Zero-suppressed Decision Diagrams

Adds support for *zero-suppressed* decision diagrams with the `zdd` class. All
operations on *ZDD*s are based on the family of sets semantics as in the
original paper by Minato.

#### Constructors
- `zdd zdd_sink(bool v)` (and `zdd_empty()` and `zdd_null()` as alternatives)

- `zdd zdd_ithvar(label_t i)`

- `zdd zdd_vars(file<label_t> vars)`, `zdd zdd_singletons(file<label_t> vars)`,
  and `zdd zdd_powerset(file<label_t> vars)` to respectively construct an *and*,
  *or*, and the *don't care* chain.

- `zdd zdd_sized_set<pred_t>(file<label_t> vars, k, pred)` to construct the sets
  of variables in *vars* whose size satisfies the given predicate in relation to
  *k*.

#### Basic Manipulation
- `zdd zdd_binop(zdd A, zdd B, bool_op op)` to apply a binary operator to two
  families of sets (also includes aliases for the _or_, _and_, and _diff_
  operators).

- `zdd zdd_change(zdd A, file<label_t> vars)` to compute the symmetric
  difference.

- `zdd zdd_complement(zdd A, file<label_t> dom)` to construct the complement.

- `zdd zdd_expand(zdd A, file<label_t> vars)` to expand the domain with new
  variables.

- `zdd zdd_offset(zdd A, file<label_t> vars)` to compute the subset without the
  given variables.

- `zdd zdd_onset(zdd A, file<label_t> vars)` to compute the subset with the
  given variables.

- `zdd zdd_project(zdd A, file<label_t> is)` to project onto a (smaller) domain.

#### Counting Operations
- `uint64_t zdd_nodecount(zdd A)` the number of (non-terminal) nodes.

- `uint64_t zdd_varcount(zdd A)` the number of levels present (i.e. variables).

- `uint64_t bdd_size(bdd A)` the number of elements in the family of sets.

#### Predicates
- `bool zdd_equal(zdd A, zdd B)` to check for set equality.

- `bool zdd_unequal(zdd A, zdd B)` to check set inequality.

- `bool zdd_subseteq(zdd A, zdd B)` to check for weak subset inclusion.

- `bool zdd_subset(zdd A, zdd B)` to check for strict subset inclusion.

- `bool zdd_disjoint(zdd A, zdd B)` to check for the sets being disjoint.

#### Set Elements
- `bool zdd_contains(zdd A, file<label_t> a)`

- `std::optional<file<label_t>> zdd_minelem(zdd A)`

- `std::optional<file<label_t>> zdd_maxelem(zdd A)`

#### Other Functions
- `output_dot(bdd f, std::string filename)` to output a visualizable _*.dot*
  file.

- `zdd zdd_from(bdd f, file<label_t> dom)` and `bdd bdd_from(zdd f,
  file<label_t> dom)` to convert between *BDD*s and *ZDD*s interpreted in the
  given domain.

### Statistics

Compile Adiar with `ADIAR_STATS` or `ADIAR_STATS_EXTRA` to gather statistics
about the execution of the internal algorithms. With `ADIAR_STATS`, Adiar will
record all statistics that only introduce a small constant time overhead to
every operation. `ADIAR_STATS_EXTRA` also gathers much more detailed
information, such as the bucket-hits of the levelized priority queue, which does
introduce a linear-time overhead to every operation.

- `stats_t adiar_stats()` to obtain a copy of the raw data values.

- `void stats_print(std::ostream)` to print all statistics to an output stream.


# v1.0.1

**Date: 6th of September, 2021**

## Optimisations

The *Equality Checking* algorithm (`==`) has been improved from its initial
*O(N<sup>2</sup> log N<sup>2</sup>)* time complexity.

- If both given BDDs are *canonical* (definition: as it would be output by
  Adiar's *Reduce* algorithm) and have the same value in their *negation
  flag*, then equality checking is done with a simple (and much faster) linear
  scan.

- In all other cases the prior time-forward processing algorithm is used. But
  this one has been improved to be an *O(N log N)* time comparison algorithm.
  That is, equality checking is not only a constant improvement computing `~(f ^
  g) == bdd_true()` but it is provably faster (both in terms of time and I/Os).

## Bug fixes

- `bdd_apply` has the comparator on recursion tuples break ties correctly. Prior
   to this, the same recursion request could potentially be handled multiple
   times independently, since all its "calls" ended up interleaving with other
   tied requests in the priority queues. This bug fix ensures the algorithm runs
   in quadratic time.

- `bdd_counter` now returns trivially false cases as such.

- *CMake* is now properly set up, such that Adiar compiles with C++17 regardless
   of its parent project. This allows the user to omit the use of
   `set_target_properties(<target> PROPERTIES CXX_STANDARD 17)` in their own
   CMake settings.

## Breaking Changes

- `adiar_init(memory)` now takes its memory argument in *bytes* rather than in
  *MiB*.

# v1.0.0

**Date: 25th of April, 2021**

## Features

### Binary Decision Diagrams

`bdd` class to hide away management of files and running the _reduce_ algorithm.
This takes care of reference counting and optimal garbage collection.

#### Constructors
- `bdd_sink(bool v)` (and `bdd_true()` and `bdd_false()` as alternatives)

- `bdd_ithvar(label_t i)` and `bdd_nithvar(label_t i)`

- `bdd_and(file<label_t> ls)` and `bdd_or(file<label_t> ls)` to construct an
  *and*/*or* chain.

- `bdd_counter(label_t min_var, label_t max_var, label_t threshold)` to
  construct whether exactly *threshold* many variables in the given interval are
  true.

Furthermore, the `node_writer` class is also provided as a means to construct a
BDD manually bottom-up.

#### Basic Manipulation
- `bdd_apply(bdd f, bdd g, bool_op op)` to combine two BDDs with a binary
  operator (also includes aliases for every possible *op*)

- `bdd_ite(bdd f, bdd g, bdd h)` to compute the if-then-else

- `bdd_not(bdd f)` to negate a bdd

- `bdd_restrict(bdd f, assignment_file as)` to fix the value of one or more
  variables

- `bdd_exists(bdd f, label_t i)` and `bdd_forall(bdd f, label_t i)` to
  existentially or forall quantify a single variable (also includes versions
  with the second argument being multiple labels in a `file<label_t>`).

#### Counting Operations
- `bdd_nodecount(bdd f)` the number of (non-terminal) nodes.

- `bdd_varcount(bdd f)` the number of levels present (i.e. variables).

- `bdd_pathcount(bdd f)` the number of unique paths to the *true* terminal.

- `bdd_satcount(bdd f, size_t varcount)` the number of satisfying assignments.

#### Input variables
- `bdd_eval(bdd f, assignment_file x)` computes *f(x)*.

- `bdd_satmin(bdd f)`, resp. `bdd_satmax(bdd f)`, to find the lexicographical
  smallest, resp. largest, satisfying assignment.

#### Other Functions
- `output_dot(bdd f, std::string filename)` to output a visualizable *.dot*
  file.

## License
Adiar 1.0.0 is distributed under the MIT license. But, notice that it depends on
the TPIE library which is licensed under LGPL v3. So, by extension any binary
file of Adiar is covered by the very same license.
