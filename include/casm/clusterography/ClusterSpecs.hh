#ifndef CASM_ClusterSpecs
#define CASM_ClusterSpecs

#include <vector>

#include "casm/clusterography/ClusterOrbits.hh"
#include "casm/crystallography/DoFDecl.hh"
#include "casm/global/enum.hh"
#include "casm/misc/cloneable_ptr.hh"

namespace CASM {

  class SymGroup;

  /** \defgroup ClusterSpecs

      \brief ClusterSpecs Generate IntegralCluster orbits of specific types
      \ingroup Clusterography
      \ingroup IntegralCluster

  */

  /// Base class, enables runtime choice of which orbit type is generated via input file parameters
  ///
  /// Note:
  /// - Most users will not use this class directly
  class ClusterSpecs : public notstd::Cloneable {
    ABSTRACT_CLONEABLE(ClusterSpecs)
  public:

    typedef std::vector<IntegralCluster> IntegralClusterVec;
    typedef std::vector<PrimPeriodicOrbit<IntegralCluster>> PeriodicOrbitVec;
    typedef std::vector<LocalOrbit<IntegralCluster>> LocalOrbitVec;
    typedef std::vector<WithinScelOrbit<IntegralCluster>> WithinScelOrbitVec;

    /// This is the orbit generation method name
    std::string name() const;
    CLUSTER_PERIODICITY_TYPE periodicity_type() const;

    void make_periodic_orbits(
      PeriodicOrbitVec &orbits,
      IntegralClusterVec const &generating_elements) const;

    void make_periodic_orbits(PeriodicOrbitVec &orbits, std::ostream &status) const;

    void make_local_orbits(
      LocalOrbitVec &orbits,
      IntegralClusterVec const &generating_elements) const;

    void make_local_orbits(LocalOrbitVec &orbits, std::ostream &status) const;

    void make_within_scel_orbits(
      WithinScelOrbitVec &orbits,
      IntegralClusterVec const &generating_elements) const;

    void make_within_scel_orbits(WithinScelOrbitVec &orbits, std::ostream &status) const;

  private:
    virtual std::string _name() const = 0;
    virtual CLUSTER_PERIODICITY_TYPE _periodicity_type() const = 0;

    virtual void _make_periodic_orbits(
      PeriodicOrbitVec &orbits,
      IntegralClusterVec const &generating_elements) const;

    virtual void _make_periodic_orbits(PeriodicOrbitVec &orbits, std::ostream &status) const;

    virtual void _make_local_orbits(
      LocalOrbitVec &orbits,
      IntegralClusterVec const &generating_elements) const;

    virtual void _make_local_orbits(LocalOrbitVec &orbits, std::ostream &status) const;

    virtual void _make_within_scel_orbits(
      WithinScelOrbitVec &orbits,
      IntegralClusterVec const &generating_elements) const;

    virtual void _make_within_scel_orbits(WithinScelOrbitVec &orbits, std::ostream &status) const;
  };

  /// Parameters most commonly used for periodic orbit generation
  class PeriodicMaxLengthClusterSpecs : public ClusterSpecs {
    CLONEABLE(PeriodicMaxLengthClusterSpecs)
  public:

    static std::string const method_name; /*periodic_max_length*/

    /// Constructor, using Structure::factor_group for the generating_group
    PeriodicMaxLengthClusterSpecs(std::shared_ptr<Structure const> _shared_prim);

    /// Constructor, using specified generating_group
    PeriodicMaxLengthClusterSpecs(
      std::shared_ptr<Structure const> _shared_prim,
      std::unique_ptr<SymGroup> _generating_group);

    /// ** These get set by constructor **

    /// The prim
    std::shared_ptr<Structure const> shared_prim;

    /// The group used to generate orbits, shared_prim->factor_group()
    notstd::cloneable_ptr<SymGroup> generating_group;

    /// The comparisons used for orbit generation
    notstd::cloneable_ptr<PrimPeriodicSymCompare<IntegralCluster>> sym_compare;

    /// ** These get set individually **

    /// Specifies filter for truncating orbits, by orbit branch. The value max_length[b], is the
    /// max site-to-site distance for clusters to be included in branch b. The b==0 value is
    /// ignored.
    std::vector<double> max_length;

    /// A filter which excludes sites that are part of the unit cell neighborhood from being
    /// included in orbits. If `site_filter(site)==true`, then the site is included, else excluded.
    SiteFilterFunction site_filter;

    /// Specifies particular clusters that should be used to generate orbits.
    std::vector<IntegralClusterOrbitGenerator> custom_generators;


  private:
    std::string _name() const override;
    CLUSTER_PERIODICITY_TYPE _periodicity_type() const override;

    void _make_periodic_orbits(
      PeriodicOrbitVec &orbits,
      IntegralClusterVec const &generating_elements) const override;

    void _make_periodic_orbits(PeriodicOrbitVec &orbits, std::ostream &status) const override;
  };


  /// Parameters most commonly used for local orbit generation
  class LocalMaxLengthClusterSpecs : public ClusterSpecs {
    CLONEABLE(LocalMaxLengthClusterSpecs)
  public:

    static std::string const method_name; /*local_max_length*/

    LocalMaxLengthClusterSpecs(
      std::shared_ptr<Structure const> _shared_prim,
      std::unique_ptr<SymGroup> _generating_group,
      IntegralCluster const &phenomenal);

    /// ** These get set by constructor **

    /// The prim
    std::shared_ptr<Structure const> shared_prim;

    /// The invariant group of the phenomenal object, used to generate local orbits
    notstd::cloneable_ptr<SymGroup> generating_group;

    /// The comparisons used for orbit generation
    notstd::cloneable_ptr<LocalSymCompare<IntegralCluster>> sym_compare;

    /// Phenomenal cluster, used to find local neighborhood
    IntegralCluster phenomenal;


    /// ** These get set individually **

    /// Specifies filter for truncating orbits, by orbit branch. The value max_length[b], is the
    /// max site-to-site distance for clusters to be included in branch b. The b==0 value is
    /// ignored.
    std::vector<double> max_length;

    /// Specifies the diff_trans-to-site cutoff radius for sites to be considered part of the local
    /// neighborhood, by orbit branch. The value cutoff_radius[b], is the max distance from any
    /// linearly interpolated path between phenomenal cluster sites to the site under
    /// consideration for that site to be included in clusters for branch b. The b==0 value
    /// is ignored.
    std::vector<double> cutoff_radius;

    /// A filter which excludes sites that are part of the local neighborhood from being included in
    /// orbits. If `site_filter(site)==true`, then the site is included, else excluded.
    SiteFilterFunction site_filter;

    /// Specifies particular clusters that should be used to generate orbits.
    std::vector<IntegralClusterOrbitGenerator> custom_generators;


  private:
    std::string _name() const override;
    CLUSTER_PERIODICITY_TYPE _periodicity_type() const override;

    void _make_local_orbits(
      LocalOrbitVec &orbits,
      IntegralClusterVec const &generating_elements) const override;

    void _make_local_orbits(LocalOrbitVec &orbits, std::ostream &status) const override;
  };

  /// Parameters most commonly used for orbit generation with supercell periodicity
  class WithinScelMaxLengthClusterSpecs : public ClusterSpecs {
    CLONEABLE(WithinScelMaxLengthClusterSpecs)
  public:

    static std::string const method_name; /*within_scel_max_length*/

    /// Constructor
    ///
    /// Note: Parameter _phenomenal is optional. If present, local orbits will be generated using
    /// the cutoff_radius. Otherwise, all sites will be used to generate orbits. In both cases,
    /// the cluster cutoff is based on max_length compared to cluster sites distances calculated
    /// using the minimum distance between any periodic images of cluster sites in the supercell defined by the _shared_prim and _superlattice_matrix (Coordinate::robust_min_dist).
    WithinScelMaxLengthClusterSpecs(
      std::shared_ptr<Structure const> _shared_prim,
      Eigen::Matrix3l const &_superlattice_matrix,
      std::unique_ptr<SymGroup> _generating_group,
      notstd::cloneable_ptr<IntegralCluster> _phenomenal = notstd::cloneable_ptr<IntegralCluster>());

    /// ** These get set by constructor **

    /// The prim
    std::shared_ptr<Structure const> shared_prim;

    /// Used to implement putting sites "within" the supercell, checking distance to nearest images
    Eigen::Matrix3l superlattice_matrix;

    /// The invariant group of the phenomenal object, used to generate local orbits
    notstd::cloneable_ptr<SymGroup> generating_group;

    /// The comparisons used for orbit generation
    notstd::cloneable_ptr<WithinScelSymCompare<IntegralCluster>> sym_compare;

    /// Phenomenal cluster, if valid, use with cutoff_radius to find local neighborhood
    notstd::cloneable_ptr<IntegralCluster> phenomenal;


    /// ** These get set individually **

    /// Specifies filter for truncating orbits, by orbit branch. The value max_length[b], is the
    /// max site-to-site distance for clusters to be included in branch b. The b==0 value is
    /// ignored.
    std::vector<double> max_length;

    /// Specifies the diff_trans-to-site cutoff radius for sites to be considered part of the local
    /// neighborhood, by orbit branch. The value cutoff_radius[b], is the max distance from any
    /// linearly interpolated path between phenomenal cluster sites to the site under
    /// consideration for that site to be included in clusters for branch b. The b==0 value
    /// is ignored.
    std::vector<double> cutoff_radius;

    /// A filter which excludes sites that are part of the local neighborhood from being included in
    /// orbits. If `site_filter(site)==true`, then the site is included, else excluded.
    SiteFilterFunction site_filter;

    /// Specifies particular clusters that should be used to generate orbits.
    std::vector<IntegralClusterOrbitGenerator> custom_generators;


  private:
    std::string _name() const override;
    CLUSTER_PERIODICITY_TYPE _periodicity_type() const override;

    void _make_within_scel_orbits(
      WithinScelOrbitVec &orbits,
      IntegralClusterVec const &generating_elements) const override;

    void _make_within_scel_orbits(WithinScelOrbitVec &orbits, std::ostream &status) const override;
  };

  /// Parameters for the most generic orbit generation method currently implemented
  class GenericPeriodicClusterSpecs : public ClusterSpecs {
    CLONEABLE(GenericPeriodicClusterSpecs)

    typedef PrimPeriodicSymCompare<IntegralCluster> SymCompareType;

    GenericPeriodicClusterSpecs() {}

    GenericPeriodicClusterSpecs(
      std::string _method_name,
      std::shared_ptr<Structure const> _shared_prim,
      std::unique_ptr<SymGroup> _generating_group,
      SymCompareType const &_sym_compare,
      SiteFilterFunction _site_filter,
      std::vector<ClusterFilterFunction> _cluster_filter,
      std::vector<CandidateSitesFunction> _candidate_sites,
      std::vector<IntegralClusterOrbitGenerator> _custom_generators);


    /// The prim
    std::shared_ptr<Structure const> shared_prim;

    /// The orbit generating group
    notstd::cloneable_ptr<SymGroup> generating_group;

    /// The comparisons used for orbit generation
    notstd::cloneable_ptr<SymCompareType> sym_compare;

    /// A filter which excludes sites that are part of the neighborhood from being included in
    /// orbits. If `site_filter(site)==true`, then the site is included, else excluded.
    SiteFilterFunction site_filter;

    /// For each orbit branch, a function implementing 'bool filter(ClusterType)', which returns
    /// false for clusters that should not be used to construct an Orbit (i.e. pair distance too
    /// large). The null orbit filter, cluster_filter[0], is ignored.
    std::vector<ClusterFilterFunction> cluster_filter;

    /// For each orbit branch, a function that generates `std::vector<xtal::UnitCellCoord>`, a
    /// vector of all the sites which will be considered for inclusion in the orbit branch. The
    /// null orbit function, candidate_sites[0], is ignored.
    std::vector<CandidateSitesFunction> candidate_sites;

    /// Specifies particular clusters that should be used to generate orbits.
    std::vector<IntegralClusterOrbitGenerator> custom_generators;

  private:
    std::string m_method_name;

    std::string _name() const override;
    CLUSTER_PERIODICITY_TYPE _periodicity_type() const override;

    void _make_periodic_orbits(
      PeriodicOrbitVec &orbits,
      IntegralClusterVec const &generating_elements) const override;

    void _make_periodic_orbits(PeriodicOrbitVec &orbits, std::ostream &status) const override;
  };

  /// Parameters for the most generic orbit generation method currently implemented
  class GenericLocalClusterSpecs : public ClusterSpecs {
    CLONEABLE(GenericLocalClusterSpecs)

    typedef LocalSymCompare<IntegralCluster> SymCompareType;

    GenericLocalClusterSpecs() {}

    GenericLocalClusterSpecs(
      std::string _method_name,
      std::shared_ptr<Structure const> _shared_prim,
      std::unique_ptr<SymGroup> _generating_group,
      SymCompareType const &_sym_compare,
      SiteFilterFunction _site_filter,
      std::vector<ClusterFilterFunction> _cluster_filter,
      std::vector<CandidateSitesFunction> _candidate_sites,
      std::vector<IntegralClusterOrbitGenerator> _custom_generators);


    /// The prim
    std::shared_ptr<Structure const> shared_prim;

    /// The orbit generating group
    notstd::cloneable_ptr<SymGroup> generating_group;

    /// The comparisons used for orbit generation
    notstd::cloneable_ptr<SymCompareType> sym_compare;

    /// A filter which excludes sites that are part of the local neighborhood from being included in
    /// orbits. If `site_filter(site)==true`, then the site is included, else excluded.
    SiteFilterFunction site_filter;

    /// For each orbit branch, a function implementing 'bool filter(ClusterType)', which returns
    /// false for clusters that should not be used to construct an Orbit (i.e. pair distance too
    /// large). The null orbit filter, cluster_filter[0], is ignored.
    std::vector<ClusterFilterFunction> cluster_filter;

    /// For each orbit branch, a function that generates `std::vector<xtal::UnitCellCoord>`, a
    /// vector of all the sites which will be considered for inclusion in the orbit branch. The
    /// null orbit function, candidate_sites[0], is ignored.
    std::vector<CandidateSitesFunction> candidate_sites;

    /// Specifies particular clusters that should be used to generate orbits.
    std::vector<IntegralClusterOrbitGenerator> custom_generators;

  private:
    std::string m_method_name;

    std::string _name() const override;
    CLUSTER_PERIODICITY_TYPE _periodicity_type() const override;

    void _make_local_orbits(
      LocalOrbitVec &orbits,
      IntegralClusterVec const &generating_elements) const override;

    void _make_local_orbits(LocalOrbitVec &orbits, std::ostream &status) const override;
  };


  /// Parameters for the most generic orbit generation method currently implemented
  class GenericWithinScelClusterSpecs : public ClusterSpecs {
    CLONEABLE(GenericWithinScelClusterSpecs)

    typedef WithinScelSymCompare<IntegralCluster> SymCompareType;

    GenericWithinScelClusterSpecs() {}

    GenericWithinScelClusterSpecs(
      std::string _method_name,
      std::shared_ptr<Structure const> _shared_prim,
      std::unique_ptr<SymGroup> _generating_group,
      SymCompareType const &_sym_compare,
      SiteFilterFunction _site_filter,
      std::vector<ClusterFilterFunction> _cluster_filter,
      std::vector<CandidateSitesFunction> _candidate_sites,
      std::vector<IntegralClusterOrbitGenerator> _custom_generators);


    /// The prim
    std::shared_ptr<Structure const> shared_prim;

    /// The orbit generating group
    notstd::cloneable_ptr<SymGroup> generating_group;

    /// The comparisons used for orbit generation
    notstd::cloneable_ptr<SymCompareType> sym_compare;

    /// A filter which excludes sites that are part of the neighborhood from being included in
    /// orbits. If `site_filter(site)==true`, then the site is included, else excluded.
    SiteFilterFunction site_filter;

    /// For each orbit branch, a function implementing 'bool filter(ClusterType)', which returns
    /// false for clusters that should not be used to construct an Orbit (i.e. pair distance too
    /// large). The null orbit filter, cluster_filter[0], is ignored.
    std::vector<ClusterFilterFunction> cluster_filter;

    /// For each orbit branch, a function that generates `std::vector<xtal::UnitCellCoord>`, a
    /// vector of all the sites which will be considered for inclusion in the orbit branch. The
    /// null orbit function, candidate_sites[0], is ignored.
    std::vector<CandidateSitesFunction> candidate_sites;

    /// Specifies particular clusters that should be used to generate orbits.
    std::vector<IntegralClusterOrbitGenerator> custom_generators;

  private:
    std::string m_method_name;

    std::string _name() const override;
    CLUSTER_PERIODICITY_TYPE _periodicity_type() const override;

    void _make_within_scel_orbits(
      WithinScelOrbitVec &orbits,
      IntegralClusterVec const &generating_elements) const override;

    void _make_within_scel_orbits(WithinScelOrbitVec &orbits, std::ostream &status) const override;
  };


  // ** Filter functions **

  /// \brief Generate clusters using all Site
  bool all_sites_filter(const xtal::Site &site);

  /// \brief Generate clusters using Site with site_occupant.size() > 1
  bool alloy_sites_filter(const xtal::Site &site);

  /// \brief Generate clusters using Site with specified DoF
  SiteFilterFunction dof_sites_filter(const std::vector<DoFKey> &dofs = {});

  /// Accept all clusters
  ClusterFilterFunction all_clusters_filter();

  /// Accept clusters with max pair distance less than specified value
  ClusterFilterFunction max_length_cluster_filter(double max_length);

  /// Accept clusters with max pair distance (using closest images) less than specified value
  ClusterFilterFunction within_scel_max_length_cluster_filter(
    double max_length,
    Eigen::Matrix3l const &superlattice_matrix);

  /// No sites (for null orbit, or global dof only)
  CandidateSitesFunction empty_neighborhood();

  /// Only sites in the origin unit cell {b, 0, 0, 0}
  CandidateSitesFunction origin_neighborhood();

  /// Sites in the supercell defined by the superlattice_matrix
  CandidateSitesFunction scel_neighborhood(Eigen::Matrix3l const &superlattice_matrix);

  /// Sites within max pair distance to any site in the origin unit cell {b, 0, 0, 0}
  CandidateSitesFunction max_length_neighborhood(double max_length);

  /// Sites within max pair distance to any site in the phenomenal cluster
  CandidateSitesFunction cutoff_radius_neighborhood(IntegralCluster const &phenomenal, double cutoff_radius);

  /// Sites within max pair distance (using closest images) to any site in the phenomenal cluster
  CandidateSitesFunction within_scel_cutoff_radius_neighborhood(
    IntegralCluster const &phenomenal,
    double cutoff_radius,
    Eigen::Matrix3l const &superlattice_matrix);

}
#endif
