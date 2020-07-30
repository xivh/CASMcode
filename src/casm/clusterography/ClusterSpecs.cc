#include "casm/clusterography/ClusterInvariants.hh"
#include "casm/clusterography/ClusterOrbits_impl.hh"
#include "casm/clusterography/ClusterSpecs_impl.hh"
#include "casm/clusterography/ClusterSymCompare_impl.hh"
#include "casm/clusterography/IntegralCluster_impl.hh"
#include "casm/crystallography/Structure.hh"
#include "casm/symmetry/Orbit_impl.hh"
#include "casm/symmetry/OrbitGeneration_impl.hh"

namespace CASM {

  std::string ClusterSpecs::name() const {
    return this->_name();
  }

  CLUSTER_PERIODICITY_TYPE ClusterSpecs::periodicity_type() const {
    return this->_periodicity_type();
  }

  ClusterSpecs::PeriodicOrbitVec ClusterSpecs::make_periodic_orbits(
    IntegralClusterVec const &generating_elements) const {
    return this->_make_periodic_orbits(generating_elements);
  }

  ClusterSpecs::PeriodicOrbitVec ClusterSpecs::make_periodic_orbits(std::ostream &status) const {
    return this->_make_periodic_orbits(status);
  }

  ClusterSpecs::LocalOrbitVec ClusterSpecs::make_local_orbits(
    IntegralClusterVec const &generating_elements) const {
    return this->_make_local_orbits(generating_elements);
  }

  ClusterSpecs::LocalOrbitVec ClusterSpecs::make_local_orbits(std::ostream &status) const {
    return this->_make_local_orbits(status);
  }

  ClusterSpecs::WithinScelOrbitVec ClusterSpecs::make_within_scel_orbits(
    IntegralClusterVec const &generating_elements) const {
    return this->_make_within_scel_orbits(generating_elements);
  }

  ClusterSpecs::WithinScelOrbitVec ClusterSpecs::make_within_scel_orbits(std::ostream &status) const {
    return this->_make_within_scel_orbits(status);
  }

  ClusterSpecs::PeriodicOrbitVec ClusterSpecs::_make_periodic_orbits(
    IntegralClusterVec const &generating_elements) const {
    throw std::runtime_error("Error: make_periodic_orbits from generating elements not implemented for '" + name() + "'");
  }

  ClusterSpecs::PeriodicOrbitVec ClusterSpecs::_make_periodic_orbits(std::ostream &status) const {
    throw std::runtime_error("Error: make_periodic_orbits not implemented for '" + name() + "'");
  }

  ClusterSpecs::LocalOrbitVec ClusterSpecs::_make_local_orbits(
    IntegralClusterVec const &generating_elements) const  {
    throw std::runtime_error("Error: make_local_orbits from generating elements not implemented for '" + name() + "'");
  }

  ClusterSpecs::LocalOrbitVec ClusterSpecs::_make_local_orbits(std::ostream &status) const {
    throw std::runtime_error("Error: make_local_orbits not implemented for '" + name() + "'");
  }

  ClusterSpecs::WithinScelOrbitVec ClusterSpecs::_make_within_scel_orbits(
    IntegralClusterVec const &generating_elements) const  {
    throw std::runtime_error("Error: _make_within_scel_orbits from generating elements not implemented for '" + name() + "'");
  }

  ClusterSpecs::WithinScelOrbitVec ClusterSpecs::_make_within_scel_orbits(std::ostream &status) const {
    throw std::runtime_error("Error: make_within_scel_orbits not implemented for '" + name() + "'");
  }


  const std::string PeriodicMaxLengthClusterSpecs::method_name = "periodic_max_length";

  PeriodicMaxLengthClusterSpecs::PeriodicMaxLengthClusterSpecs(
    std::shared_ptr<Structure const> _shared_prim,
    std::unique_ptr<SymGroup> _generating_group,
    SiteFilterFunction const &_site_filter,
    std::vector<double> const &_max_length,
    std::vector<IntegralClusterOrbitGenerator> const &_custom_generators):
    shared_prim(_shared_prim),
    generating_group(std::move(_generating_group)),
    sym_compare(
      notstd::make_unique<PrimPeriodicSymCompare<IntegralCluster>>(
        shared_prim,
        shared_prim->lattice().tol())),
    site_filter(_site_filter),
    max_length(_max_length),
    custom_generators(_custom_generators) {
  }

  std::string PeriodicMaxLengthClusterSpecs::_name() const {
    return method_name;
  }

  CLUSTER_PERIODICITY_TYPE PeriodicMaxLengthClusterSpecs::_periodicity_type() const {
    return CLUSTER_PERIODICITY_TYPE::PRIM_PERIODIC;
  };

  ClusterSpecs::PeriodicOrbitVec PeriodicMaxLengthClusterSpecs::_make_periodic_orbits(
    IntegralClusterVec const &generating_elements) const {
    return generate_orbits(generating_elements, *generating_group, *sym_compare);
  }

  ClusterSpecs::PeriodicOrbitVec PeriodicMaxLengthClusterSpecs::_make_periodic_orbits(
    std::ostream &status) const {

    typedef PrimPeriodicOrbit<IntegralCluster> orbit_type;
    std::vector<OrbitBranchSpecs<orbit_type> > specs;

    for(int branch = 0; branch < max_length.size(); ++branch) {

      std::vector<xtal::UnitCellCoord> candidate_sites;
      CandidateSitesFunction f;
      if(branch == 0) {
        f = empty_neighborhood();
      }
      else if(branch == 1) {
        f = origin_neighborhood();
      }
      else {
        f = max_length_neighborhood(max_length[branch]);
      }
      candidate_sites = f(*shared_prim, site_filter);

      ClusterFilterFunction cluster_filter;
      if(branch <= 1) {
        cluster_filter = all_clusters_filter();
      }
      else {
        cluster_filter = max_length_cluster_filter(max_length[branch]);
      }

      specs.emplace_back(
        *shared_prim,
        candidate_sites.begin(),
        candidate_sites.end(),
        *generating_group,
        cluster_filter,
        *sym_compare);
    }

    // now generate orbits
    PeriodicOrbitVec orbits;
    make_orbits(specs.begin(), specs.end(), custom_generators, std::back_inserter(orbits), status);
    return orbits;
  }


  std::string const LocalMaxLengthClusterSpecs::method_name = "local_max_length";

  LocalMaxLengthClusterSpecs::LocalMaxLengthClusterSpecs(
    std::shared_ptr<Structure const> _shared_prim,
    std::unique_ptr<SymGroup> _generating_group,
    IntegralCluster const &_phenomenal,
    SiteFilterFunction const &_site_filter,
    std::vector<double> const &_max_length,
    std::vector<double> const &_cutoff_radius,
    std::vector<IntegralClusterOrbitGenerator> const &_custom_generators):
    shared_prim(_shared_prim),
    generating_group(std::move(_generating_group)),
    sym_compare(
      notstd::make_unique<LocalSymCompare<IntegralCluster>>(
        shared_prim, shared_prim->lattice().tol())),
    phenomenal(_phenomenal),
    site_filter(_site_filter),
    max_length(_max_length),
    cutoff_radius(_cutoff_radius),
    custom_generators(_custom_generators) {
  }

  std::string LocalMaxLengthClusterSpecs::_name() const {
    return method_name;
  }

  CLUSTER_PERIODICITY_TYPE LocalMaxLengthClusterSpecs::_periodicity_type() const {
    return CLUSTER_PERIODICITY_TYPE::LOCAL;
  };

  ClusterSpecs::LocalOrbitVec LocalMaxLengthClusterSpecs::_make_local_orbits(
    IntegralClusterVec const &generating_elements) const {
    return generate_orbits(generating_elements, *generating_group, *sym_compare);
  }

  ClusterSpecs::LocalOrbitVec LocalMaxLengthClusterSpecs::_make_local_orbits(
    std::ostream &status) const {

    typedef LocalOrbit<IntegralCluster> orbit_type;
    std::vector<OrbitBranchSpecs<orbit_type> > specs;

    for(int branch = 0; branch < max_length.size(); ++branch) {

      std::vector<xtal::UnitCellCoord> candidate_sites;
      CandidateSitesFunction f;
      if(branch == 0) {
        f = empty_neighborhood();
      }
      else {
        f = cutoff_radius_neighborhood(phenomenal, cutoff_radius[branch]);
      }
      candidate_sites = f(*shared_prim, site_filter);

      ClusterFilterFunction cluster_filter;
      if(branch <= 1) {
        cluster_filter = all_clusters_filter();
      }
      else {
        cluster_filter = max_length_cluster_filter(max_length[branch]);
      }

      specs.emplace_back(
        *shared_prim,
        candidate_sites.begin(),
        candidate_sites.end(),
        *generating_group,
        cluster_filter,
        *sym_compare);
    }

    // now generate orbits
    LocalOrbitVec orbits;
    make_orbits(specs.begin(), specs.end(), custom_generators, std::back_inserter(orbits), status);
    return orbits;
  }


  std::string const WithinScelMaxLengthClusterSpecs::method_name = "within_scel_max_length";

  WithinScelMaxLengthClusterSpecs::WithinScelMaxLengthClusterSpecs(
    std::shared_ptr<Structure const> _shared_prim,
    Eigen::Matrix3l const &_superlattice_matrix,
    std::unique_ptr<SymGroup> _generating_group,
    SiteFilterFunction const &_site_filter,
    std::vector<double> const &_max_length,
    std::vector<double> const &_cutoff_radius,
    std::vector<IntegralClusterOrbitGenerator> const &_custom_generators,
    notstd::cloneable_ptr<IntegralCluster> _phenomenal):
    shared_prim(_shared_prim),
    superlattice_matrix(_superlattice_matrix),
    generating_group(std::move(_generating_group)),
    sym_compare(
      notstd::make_unique<WithinScelSymCompare<IntegralCluster>>(
        shared_prim,
        superlattice_matrix,
        shared_prim->lattice().tol())),
    phenomenal(std::move(_phenomenal)),
    site_filter(_site_filter),
    max_length(_max_length),
    cutoff_radius(_cutoff_radius),
    custom_generators(_custom_generators) {
  }

  std::string WithinScelMaxLengthClusterSpecs::_name() const {
    return method_name;
  }

  CLUSTER_PERIODICITY_TYPE WithinScelMaxLengthClusterSpecs::_periodicity_type() const {
    return CLUSTER_PERIODICITY_TYPE::WITHIN_SCEL;
  };

  ClusterSpecs::WithinScelOrbitVec WithinScelMaxLengthClusterSpecs::_make_within_scel_orbits(
    IntegralClusterVec const &generating_elements) const {
    return generate_orbits(generating_elements, *generating_group, *sym_compare);
  }

  ClusterSpecs::WithinScelOrbitVec WithinScelMaxLengthClusterSpecs::_make_within_scel_orbits(
    std::ostream &status) const {

    typedef WithinScelOrbit<IntegralCluster> orbit_type;
    std::vector<OrbitBranchSpecs<orbit_type> > specs;

    for(int branch = 0; branch < max_length.size(); ++branch) {

      std::vector<xtal::UnitCellCoord> candidate_sites;
      CandidateSitesFunction f;

      if(phenomenal) {
        if(branch == 0) {
          f = empty_neighborhood();
        }
        else {
          f = within_scel_cutoff_radius_neighborhood(
                *phenomenal,
                cutoff_radius[branch],
                superlattice_matrix);
        }
      }
      else {
        if(branch == 0) {
          f = empty_neighborhood();
        }
        else {
          f = scel_neighborhood(superlattice_matrix);
        }
      }
      candidate_sites = f(*shared_prim, site_filter);

      ClusterFilterFunction cluster_filter;
      if(branch <= 1) {
        cluster_filter = all_clusters_filter();
      }
      else {
        cluster_filter = within_scel_max_length_cluster_filter(
                           max_length[branch],
                           superlattice_matrix);
      }

      specs.emplace_back(
        *shared_prim,
        candidate_sites.begin(),
        candidate_sites.end(),
        *generating_group,
        cluster_filter,
        *sym_compare);
    }

    // now generate orbits
    WithinScelOrbitVec orbits;
    make_orbits(specs.begin(), specs.end(), custom_generators, std::back_inserter(orbits), status);
    return orbits;
  }


  GenericPeriodicClusterSpecs::GenericPeriodicClusterSpecs(
    std::string _method_name,
    std::shared_ptr<Structure const> _shared_prim,
    std::unique_ptr<SymGroup> _generating_group,
    SymCompareType const &_sym_compare,
    SiteFilterFunction _site_filter,
    std::vector<ClusterFilterFunction> _cluster_filter,
    std::vector<CandidateSitesFunction> _candidate_sites,
    std::vector<IntegralClusterOrbitGenerator> _custom_generators) :
    shared_prim(_shared_prim),
    generating_group(std::move(_generating_group)),
    sym_compare(notstd::clone(_sym_compare)),
    site_filter(_site_filter),
    cluster_filter(_cluster_filter),
    candidate_sites(_candidate_sites),
    custom_generators(_custom_generators),
    m_method_name(_method_name) {}

  std::string GenericPeriodicClusterSpecs::_name() const {
    return m_method_name;
  }

  CLUSTER_PERIODICITY_TYPE GenericPeriodicClusterSpecs::_periodicity_type() const {
    return CLUSTER_PERIODICITY_TYPE::PRIM_PERIODIC;
  }

  ClusterSpecs::PeriodicOrbitVec GenericPeriodicClusterSpecs::_make_periodic_orbits(
    IntegralClusterVec const &generating_elements) const {
    return generate_orbits(generating_elements, *generating_group, *sym_compare);
  }

  ClusterSpecs::PeriodicOrbitVec GenericPeriodicClusterSpecs::_make_periodic_orbits(
    std::ostream &status) const {

    if(cluster_filter.size() != candidate_sites.size()) {
      throw std::runtime_error("Error in GenericPeriodicClusterSpecs::_make_periodic_orbits: cluster_filter.size() != candidate_sites.size()");
    }
    typedef PrimPeriodicOrbit<IntegralCluster> OrbitType;
    std::vector<OrbitBranchSpecs<OrbitType> > specs;

    for(int branch = 0; branch < cluster_filter.size(); ++branch) {

      std::vector<xtal::UnitCellCoord> tmp;
      tmp = candidate_sites[branch](*shared_prim, site_filter);

      specs.emplace_back(
        *shared_prim,
        tmp.begin(),
        tmp.end(),
        *generating_group,
        cluster_filter[branch],
        *sym_compare);
    }

    // now generate orbits
    PeriodicOrbitVec orbits;
    make_orbits(specs.begin(), specs.end(), custom_generators, std::back_inserter(orbits), status);
    return orbits;
  }


  GenericLocalClusterSpecs::GenericLocalClusterSpecs(
    std::string _method_name,
    std::shared_ptr<Structure const> _shared_prim,
    std::unique_ptr<SymGroup> _generating_group,
    SymCompareType const &_sym_compare,
    SiteFilterFunction _site_filter,
    std::vector<ClusterFilterFunction> _cluster_filter,
    std::vector<CandidateSitesFunction> _candidate_sites,
    std::vector<IntegralClusterOrbitGenerator> _custom_generators) :
    shared_prim(_shared_prim),
    generating_group(std::move(_generating_group)),
    sym_compare(notstd::clone(_sym_compare)),
    site_filter(_site_filter),
    cluster_filter(_cluster_filter),
    candidate_sites(_candidate_sites),
    custom_generators(_custom_generators),
    m_method_name(_method_name) {}

  std::string GenericLocalClusterSpecs::_name() const {
    return m_method_name;
  }


  CLUSTER_PERIODICITY_TYPE GenericLocalClusterSpecs::_periodicity_type() const {
    return CLUSTER_PERIODICITY_TYPE::LOCAL;
  }

  ClusterSpecs::LocalOrbitVec GenericLocalClusterSpecs::_make_local_orbits(
    IntegralClusterVec const &generating_elements) const {
    return generate_orbits(generating_elements, *generating_group, *sym_compare);
  }

  ClusterSpecs::LocalOrbitVec GenericLocalClusterSpecs::_make_local_orbits(
    std::ostream &status) const {

    if(cluster_filter.size() != candidate_sites.size()) {
      throw std::runtime_error("Error in GenericLocalClusterSpecs::_make_local_orbits: cluster_filter.size() != candidate_sites.size()");
    }
    typedef LocalOrbit<IntegralCluster> OrbitType;
    std::vector<OrbitBranchSpecs<OrbitType> > specs;

    for(int branch = 0; branch < cluster_filter.size(); ++branch) {

      std::vector<xtal::UnitCellCoord> tmp;
      tmp = candidate_sites[branch](*shared_prim, site_filter);

      specs.emplace_back(
        *shared_prim,
        tmp.begin(),
        tmp.end(),
        *generating_group,
        cluster_filter[branch],
        *sym_compare);
    }

    // now generate orbits
    LocalOrbitVec orbits;
    make_orbits(specs.begin(), specs.end(), custom_generators, std::back_inserter(orbits), status);
    return orbits;
  }


  GenericWithinScelClusterSpecs::GenericWithinScelClusterSpecs(
    std::string _method_name,
    std::shared_ptr<Structure const> _shared_prim,
    std::unique_ptr<SymGroup> _generating_group,
    SymCompareType const &_sym_compare,
    SiteFilterFunction _site_filter,
    std::vector<ClusterFilterFunction> _cluster_filter,
    std::vector<CandidateSitesFunction> _candidate_sites,
    std::vector<IntegralClusterOrbitGenerator> _custom_generators) :
    shared_prim(_shared_prim),
    generating_group(std::move(_generating_group)),
    sym_compare(notstd::clone(_sym_compare)),
    site_filter(_site_filter),
    cluster_filter(_cluster_filter),
    candidate_sites(_candidate_sites),
    custom_generators(_custom_generators),
    m_method_name(_method_name) {}

  std::string GenericWithinScelClusterSpecs::_name() const {
    return m_method_name;
  }

  CLUSTER_PERIODICITY_TYPE GenericWithinScelClusterSpecs::_periodicity_type() const {
    return CLUSTER_PERIODICITY_TYPE::LOCAL;
  }

  ClusterSpecs::WithinScelOrbitVec GenericWithinScelClusterSpecs::_make_within_scel_orbits(
    IntegralClusterVec const &generating_elements) const {
    return generate_orbits(generating_elements, *generating_group, *sym_compare);
  }

  ClusterSpecs::WithinScelOrbitVec GenericWithinScelClusterSpecs::_make_within_scel_orbits(
    std::ostream &status) const {

    if(cluster_filter.size() != candidate_sites.size()) {
      throw std::runtime_error("Error in GenericWithinScelClusterSpecs::_make_within_scel_orbits: cluster_filter.size() != candidate_sites.size()");
    }
    typedef WithinScelOrbit<IntegralCluster> OrbitType;
    std::vector<OrbitBranchSpecs<OrbitType> > specs;

    for(int branch = 0; branch < cluster_filter.size(); ++branch) {

      std::vector<xtal::UnitCellCoord> tmp;
      tmp = candidate_sites[branch](*shared_prim, site_filter);

      specs.emplace_back(
        *shared_prim,
        tmp.begin(),
        tmp.end(),
        *generating_group,
        cluster_filter[branch],
        *sym_compare);
    }

    // now generate orbits
    WithinScelOrbitVec orbits;
    make_orbits(specs.begin(), specs.end(), custom_generators, std::back_inserter(orbits), status);
    return orbits;
  }


  namespace ClusterSpecs_impl {

    class DoFSitesFilter {
    public:
      DoFSitesFilter(std::vector<DoFKey> const &_dofs):
        dofs(_dofs) {}

      bool operator()(xtal::Site const &site) {
        if(dofs.empty() && (site.dof_size() != 0 || site.occupant_dof().size() > 1)) {
          return true;
        }
        for(DoFKey const &dof : dofs) {
          if(site.has_dof(dof)) {
            return true;
          }
          else if(dof == "occ" && site.occupant_dof().size() > 1) {
            return true;
          }
        }
        return false;
      }

      std::vector<DoFKey> dofs;
    };

    class AllClusters {
    public:
      bool operator()(IntegralCluster const &clust) {
        return true;
      }
    };

    class MaxLengthClusterFilter {
    public:
      MaxLengthClusterFilter(double _max_length):
        max_length(_max_length) {}

      bool operator()(IntegralCluster const &clust) {
        if(clust.size() <= 1) {
          return true;
        }
        ClusterInvariants invariants {clust};
        return invariants.displacement().back() < max_length;
      }

    private:
      double max_length;
    };

    class WithinScelMaxLengthClusterFilter {
    public:
      WithinScelMaxLengthClusterFilter(
        double _max_length,
        Eigen::Matrix3l const &_superlattice_matrix):
        max_length(_max_length),
        superlattice_matrix(_superlattice_matrix) {}

      bool operator()(IntegralCluster const &clust) {
        if(clust.size() <= 1) {
          return true;
        }
        WithinScelClusterInvariants invariants {clust, superlattice_matrix};
        return invariants.displacement().back() < max_length;
      }

    private:
      double max_length;
      Eigen::Matrix3l superlattice_matrix;
    };

    class EmptyNeighborhood {
    public:
      std::vector<xtal::UnitCellCoord> operator()(Structure const &prim, SiteFilterFunction site_filter) {
        return std::vector<xtal::UnitCellCoord> {};
      }
    };

    class OriginNeighborhood {
    public:
      std::vector<xtal::UnitCellCoord> operator()(Structure const &prim, SiteFilterFunction site_filter) {
        std::vector<xtal::UnitCellCoord> result;
        for(int i = 0; i < prim.basis().size(); ++i) {
          if(site_filter(prim.basis()[i])) {
            result.emplace_back(i, 0, 0, 0);
          }
        }
        return result;
      }
    };

    class MaxLengthNeighborhood {
    public:
      MaxLengthNeighborhood(double _max_length):
        max_length(_max_length) {};

      std::vector<xtal::UnitCellCoord> operator()(Structure const &prim, SiteFilterFunction site_filter) {
        std::vector<xtal::UnitCellCoord> result;
        double xtal_tol = prim.lattice().tol();
        neighborhood(prim, max_length, site_filter, std::back_inserter(result), xtal_tol);
        return result;
      }

    private:
      double max_length;
    };

    class ScelNeighborhood {
    public:
      ScelNeighborhood(Eigen::Matrix3l const &supercell_matrix):
        lattice_points(xtal::make_lattice_points(supercell_matrix)) {}

      std::vector<xtal::UnitCellCoord> operator()(Structure const &prim, SiteFilterFunction site_filter) {
        std::vector<xtal::UnitCellCoord> result;
        int b = 0;
        for(auto const &site : prim.basis()) {
          if(site_filter(site)) {
            for(auto const &lattice_point : lattice_points) {
              result.emplace_back(b, lattice_point);
            }
          }
          ++b;
        }
        return result;
      }

    private:
      std::vector<xtal::UnitCell> lattice_points;
    };

    class CutoffRadiusNeighborhood {
    public:
      CutoffRadiusNeighborhood(IntegralCluster const &_phenomenal, double _cutoff_radius):
        phenomenal(_phenomenal), cutoff_radius(_cutoff_radius) {};

      std::vector<xtal::UnitCellCoord> operator()(Structure const &prim, SiteFilterFunction site_filter) {
        std::vector<xtal::UnitCellCoord> result;
        double xtal_tol = prim.lattice().tol();
        neighborhood(phenomenal, cutoff_radius, site_filter, std::back_inserter(result), xtal_tol);
        return result;
      }

    private:
      IntegralCluster phenomenal;
      double cutoff_radius;
    };

    class WithinScelCutoffRadiusNeighborhood {
    public:

      WithinScelCutoffRadiusNeighborhood(
        IntegralCluster const &_phenomenal,
        double _cutoff_radius,
        Eigen::Matrix3l const &_superlattice_matrix):
        cutoff_radius_neighborhood_f(_phenomenal, _cutoff_radius),
        within_scel_f(_superlattice_matrix) {};

      std::vector<xtal::UnitCellCoord> operator()(Structure const &prim, SiteFilterFunction site_filter) {

        // local neighborhood in & out of supercell -- can produce duplicates when brought within
        std::vector<xtal::UnitCellCoord> local = cutoff_radius_neighborhood_f(prim, site_filter);

        // bring local neighborhood sites inside supercell and keep uniques
        std::set<xtal::UnitCellCoord> unique_uccoord;
        for(auto const &uccoord : local) {
          unique_uccoord.insert(within_scel_f(uccoord));
        }

        return std::vector<xtal::UnitCellCoord> {unique_uccoord.begin(), unique_uccoord.end()};
      }

    private:
      CutoffRadiusNeighborhood cutoff_radius_neighborhood_f;
      xtal::IntegralCoordinateWithin_f within_scel_f;

    };
  }


  /// \brief Generate clusters using all Site
  bool all_sites_filter(xtal::Site const &site) {
    return true;
  }

  /// \brief Generate clusters using Site with site_occupant.size() > 1
  bool alloy_sites_filter(xtal::Site const &site) {
    return site.occupant_dof().size() > 1;
  }

  /// \brief Generate clusters using Site with specified DoF
  ///
  /// If dofs is empty, return true if Site has any continuous DoF or >1 allowed occupant DoF
  /// If dofs is not empty, return true if Site has any of the DoF types included. Use "occ" for /
  /// Site with >1 occupant allowed
  SiteFilterFunction dof_sites_filter(std::vector<DoFKey> const &dofs) {
    return ClusterSpecs_impl::DoFSitesFilter {dofs};
  }


  /// Accept all clusters
  ClusterFilterFunction all_clusters_filter() {
    return ClusterSpecs_impl::AllClusters{};
  }

  /// Accept clusters with max pair distance less than max_length
  ClusterFilterFunction max_length_cluster_filter(double max_length) {
    return ClusterSpecs_impl::MaxLengthClusterFilter{max_length};
  }

  /// Accept clusters with max pair distance (using closest images) less than max_length
  ClusterFilterFunction within_scel_max_length_cluster_filter(
    double max_length,
    Eigen::Matrix3l const &superlattice_matrix) {
    return ClusterSpecs_impl::WithinScelMaxLengthClusterFilter{max_length, superlattice_matrix};
  }

  /// No sites (for null orbit, or global dof only)
  CandidateSitesFunction empty_neighborhood() {
    return ClusterSpecs_impl::EmptyNeighborhood{};
  }

  /// Only sites in the origin unit cell {b, 0, 0, 0}
  CandidateSitesFunction origin_neighborhood() {
    return ClusterSpecs_impl::OriginNeighborhood{};
  }

  /// Sites in the superlattice defined by the superlattice_matrix
  CandidateSitesFunction scel_neighborhood(Eigen::Matrix3l const &superlattice_matrix) {
    return ClusterSpecs_impl::ScelNeighborhood{superlattice_matrix};
  }

  /// Sites within max_length distance to any site in the origin unit cell {b, 0, 0, 0}
  CandidateSitesFunction max_length_neighborhood(double max_length) {
    return ClusterSpecs_impl::MaxLengthNeighborhood{max_length};
  }

  /// Sites within cutoff_radius distance to any site in the phenomenal cluster
  CandidateSitesFunction cutoff_radius_neighborhood(IntegralCluster const &phenomenal, double cutoff_radius) {
    return ClusterSpecs_impl::CutoffRadiusNeighborhood {phenomenal, cutoff_radius};
  }

  /// Sites within cutoff_radius distance (using closest images) to any site in the phenomenal cluster
  CandidateSitesFunction within_scel_cutoff_radius_neighborhood(
    IntegralCluster const &phenomenal,
    double cutoff_radius,
    Eigen::Matrix3l const &superlattice_matrix)  {
    return ClusterSpecs_impl::WithinScelCutoffRadiusNeighborhood {
      phenomenal, cutoff_radius, superlattice_matrix};
  }

}
