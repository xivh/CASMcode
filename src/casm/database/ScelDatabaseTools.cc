#include "casm/clex/PrimClex.hh"
#include "casm/crystallography/CanonicalForm.hh"
#include "casm/crystallography/Structure.hh"
#include "casm/database/ScelDatabaseTools.hh"

namespace CASM {
  namespace DB {

    /// Make canonical supercell and insert into supercell database
    std::pair<Database<Supercell>::iterator, bool> make_canonical_and_insert(
      PrimClex const *primclex,
      Lattice const &super_lattice,
      Database<Supercell> &supercell_db) {

      auto const &prim = primclex->prim();
      auto const &pg = prim.point_group();
      double xtal_tol = prim.lattice().tol();
      xtal::Lattice canonical_lattice = xtal::canonical::equivalent(super_lattice, pg, xtal_tol);
      return supercell_db.emplace(primclex, canonical_lattice);
    }

    /// Make canonical supercell and insert into supercell database
    std::pair<Database<Supercell>::iterator, bool> make_canonical_and_insert(
      PrimClex const *primclex,
      Eigen::Matrix3l const &transformation_matrix_to_super,
      Database<Supercell> &supercell_db) {
      auto const &prim = primclex->prim();
      xtal::Lattice super_lattice = make_superlattice(prim.lattice(), transformation_matrix_to_super);
      return make_canonical_and_insert(primclex, super_lattice, supercell_db);
    }

    /// Make canonical supercell and insert into supercell database
    std::pair<Database<Supercell>::iterator, bool> make_canonical_and_insert(
      std::shared_ptr<Structure const> const &shared_prim,
      Lattice const &super_lattice,
      Database<Supercell> &supercell_db) {

      auto const &pg = shared_prim->point_group();
      double xtal_tol = shared_prim->lattice().tol();
      xtal::Lattice canonical_lattice = xtal::canonical::equivalent(super_lattice, pg, xtal_tol);
      return supercell_db.emplace(shared_prim, canonical_lattice);
    }

    /// Make canonical supercell and insert into supercell database
    std::pair<Database<Supercell>::iterator, bool> make_canonical_and_insert(
      std::shared_ptr<Structure const> const &shared_prim,
      Eigen::Matrix3l const &transformation_matrix_to_super,
      Database<Supercell> &supercell_db) {

      xtal::Lattice super_lattice = make_superlattice(shared_prim->lattice(), transformation_matrix_to_super);
      return make_canonical_and_insert(shared_prim, super_lattice, supercell_db);
    }

  }
}
