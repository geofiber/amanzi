/*
  WhetStone, version 2.1
  Release name: naka-to.

  Copyright 2010-201x held jointly by LANS/LANL, LBNL, and PNNL. 
  Amanzi is released under the three-clause BSD License. 
  The terms of use and "as is" disclaimer for this license are 
  provided in the top-level COPYRIGHT file.

  Author: Konstantin Lipnikov (lipnikov@lanl.gov)

  Maps between mesh objects located on different meshes, e.g. two 
  states of a deformable mesh: virtual element implementation.
*/

#ifndef AMANZI_WHETSTONE_MESH_MAPS_VEM_HH_
#define AMANZI_WHETSTONE_MESH_MAPS_VEM_HH_

#include "Teuchos_RCP.hpp"

#include "Mesh.hh"
#include "Point.hh"

#include "DenseMatrix.hh"
#include "MeshMaps.hh"
#include "Polynomial.hh"
#include "Projector.hh"
#include "Tensor.hh"
#include "WhetStone_typedefs.hh"

namespace Amanzi {
namespace WhetStone {

class MeshMaps_VEM : public MeshMaps { 
 public:
  MeshMaps_VEM(Teuchos::RCP<const AmanziMesh::Mesh> mesh) :
      MeshMaps(mesh),
      projector_(mesh),
      order_(2) {};
  MeshMaps_VEM(Teuchos::RCP<const AmanziMesh::Mesh> mesh0,
               Teuchos::RCP<const AmanziMesh::Mesh> mesh1) :
      MeshMaps(mesh0, mesh1),
      projector_(mesh0),
      order_(2) {};
  ~MeshMaps_VEM() {};

  // Maps
  // -- pseudo-velocity
  virtual void VelocityFace(int f, VectorPolynomial& vf) const override;
  virtual void VelocityCell(int c, const std::vector<VectorPolynomial>& vf,
                            VectorPolynomial& vc) const override;

  // -- Nanson formula
  virtual void NansonFormula(int f, double t, const VectorPolynomial& vf,
                             VectorPolynomial& cn) const override;

  // access
  void set_order(int order) { order_ = order; }

 private:
  // pseudo-velocity on edge e
  void VelocityEdge_(int e, VectorPolynomial& ve) const;

  // old deprecated methods
  void JacobianFaceValue_(int f, const VectorPolynomial& v, const AmanziGeometry::Point& x, Tensor& J) const;

  void LeastSquareProjector_Cell_(int order, int c, const std::vector<VectorPolynomial>& vf,
                                  VectorPolynomial& vc) const;

 private:
  int order_;
  Projector projector_;
};

}  // namespace WhetStone
}  // namespace Amanzi

#endif
