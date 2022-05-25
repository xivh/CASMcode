#ifndef CASM_FillSupercell_impl
#define CASM_FillSupercell_impl

#include "casm/clex/ConfigEnumByPermutation.hh"
#include "casm/clex/Configuration.hh"
#include "casm/clex/FillSupercell.hh"
#include "casm/clex/Supercell_impl.hh"
#include "casm/crystallography/Structure.hh"
#include "casm/symmetry/SymTools.hh"

namespace CASM {

/// Make all equivalent configurations with respect to the prim factor group
/// that fill a supercell
///
/// Method:
/// - Generate primitive of "configuration"
/// - Find all non-equivalent ways (w.r.t. supercell factor group) to fill
/// "shared_supercell" with
///   the primitive configuration
/// - For each, use ConfigEnumByPermutation to generate all equivalents (w.r.t.
/// supercell factor group)
///   in "shared_supercell"
///
/// Note:
/// - Does not check if "shared_supercell" can be tiled by configuration. To do
/// this check
///   with `is_valid_sub_configuration`.
template <typename ConfigOutputIterator>
ConfigOutputIterator make_all_super_configurations(
    Configuration const &configuration,
    std::shared_ptr<Supercell const> shared_supercell,
    ConfigOutputIterator result) {
  // There may be equivalent configurations (as infinite crystals) that can not
  // be obtained via Supercell permutations. This method finds all of these by
  // identifying the unique ways the primitive configuration's lattice can tile
  // the supercell.
  //
  // notes:
  // - The prim.factor_group() generates an orbit of equivalent primitive
  // configuration lattices
  // - The shared_supercell->factor_group() has possibly lower symmetry,
  // resulting in sub-orbits
  //   of lattices that cannot be obtained via Supercell permutations
  // - This will find a prim.factor_group() operation to transform the primitive
  // configuration
  //   lattice into an element from each sub-orbit.
  // - Then, for each sub-orbit generating lattice, if it can tile
  // shared_supercell, it is tiled
  //   into shared_supercell to create an initial configuration which is
  //   permuted to generate equivalents.

  // --- gather input and make useful functions ---
  auto const &prim = shared_supercell->prim();
  auto const &supercell_sym_info = shared_supercell->sym_info();
  Configuration primitive_configuration = configuration.primitive();
  SymGroup const &super_group = prim.factor_group();

  // Subgroup of prim factor group that leaves primitive_configuration's lattice
  // invariant
  auto prim_config_lattice_invariant_subgroup = sym::invariant_subgroup(
      super_group, primitive_configuration.ideal_lattice());

  // Find primitive configuration lattice sub-orbit generating elements for
  //   super_group -> shared_supercell->factor_group() symmetry breaking
  //
  // Function returns true if super_group_op.index() is the minimum of all
  // generated by:
  //     subgroup_op * super_group_op * invariant_subgroup_op
  //
  // If true, sym::copy_apply(super_group_op,
  // primitive_configuration.ideal_lattice()) is a unique
  //    element of a sub-orbit of lattices
  auto generates_unique_prim_config_lattice_wrt_supercell_factor_group =
      [&](SymOp const &super_group_op) {
        for (auto const &invariant_subgroup_op :
             prim_config_lattice_invariant_subgroup) {
          for (auto const &subgroup_op : supercell_sym_info.factor_group()) {
            if ((subgroup_op * super_group_op * invariant_subgroup_op).index() <
                super_group_op.index()) {
              return false;
            }
          }
        }
        return true;
      };

  // Function to find first subgroup_op, if it exists, such that
  //  copy_apply(subgroup_op * super_group_op, prim_config_lattice) fills the
  //  supercell
  //
  // TODO: is it necessary to check all supercell factor group ops?
  auto find_supercell_factor_group_op_such_that_product_fills_supercell =
      [&](SymOp const &super_group_op) {
        auto const &super_lattice = shared_supercell->lattice();
        auto const &prim_config_lattice =
            primitive_configuration.ideal_lattice();
        double xtal_tol = prim_config_lattice.tol();
        auto it = supercell_sym_info.factor_group().begin();
        auto end = supercell_sym_info.factor_group().end();
        for (; it != end; ++it) {
          auto test_op = (*it) * super_group_op;
          auto test_lattice = sym::copy_apply(test_op, prim_config_lattice);
          if (is_superlattice(super_lattice, test_lattice, xtal_tol).first) {
            return it;
          }
        }
        return end;
      };

  // --- make all super configurations ---

  for (auto const &super_group_op : super_group) {
    if (generates_unique_prim_config_lattice_wrt_supercell_factor_group(
            super_group_op)) {
      auto it =
          find_supercell_factor_group_op_such_that_product_fills_supercell(
              super_group_op);
      if (it != supercell_sym_info.factor_group().end()) {
        Configuration initial_configuration = fill_supercell(
            (*it) * super_group_op, primitive_configuration, shared_supercell);
        ConfigEnumByPermutation enumerator{initial_configuration};
        for (auto const &equivalent_configuration : enumerator) {
          *result++ = equivalent_configuration;
        }
      }
    }
  }

  return result;
}

}  // namespace CASM

#endif
