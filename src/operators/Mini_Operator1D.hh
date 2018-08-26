/*
  Operators

  Copyright 2010-201x held jointly by LANS/LANL, LBNL, and PNNL. 
  Amanzi is released under the three-clause BSD License. 
  The terms of use and "as is" disclaimer for this license are 
  provided in the top-level COPYRIGHT file.

  Author: Konstantin Lipnikov (lipnikov@lanl.gov)

  This provides a base class for other mini classes that implement 
  mathematical models for special physics.
*/

#ifndef AMANZI_MINI_OPERATOR_1D_HH_
#define AMANZI_MINI_OPERATOR_1D_HH_

#include <memory>

#include <DenseVector.hh>

namespace Amanzi {
namespace Operators {

class Mini_Operator1D {
 public:
  Mini_Operator1D() {};
  ~Mini_Operator1D() {};

  // initialize 1D mesh and geometry
  void Init(std::shared_ptr<const WhetStone::DenseVector> mesh,
            const std::string& geometry, double area_min, double area_max);

  // main operations
  void ApplyInverse(const WhetStone::DenseVector& rhs, WhetStone::DenseVector& sol);

  // modifying operator
  void AddAccumulationTerm(double s0, double s1, double dt,
                           WhetStone::DenseVector& sol);

  void ScaleMatrix(double scale) { diag_ *= scale; 
                                   up_ *= scale; down_ *= scale; }

  void GetMatrixRow(int i, double* al, double* ad, double* ar) const;
  void SetMatrixRow(int i, double al, double ad, double ar);

  // elementary mesh operations
  double mesh_cell_volume(int i) { return (*mesh_)(i + 1) - (*mesh_)(i); } 

  // access
  const WhetStone::DenseVector& rhs() const { return rhs_; }

 protected:
  // mesh
  std::shared_ptr<const WhetStone::DenseVector> mesh_;
  int igeo_;
  double area_min_, area_max_;

  // matrix
  WhetStone::DenseVector diag_, up_, down_;
  WhetStone::DenseVector rhs_;
};

}  // namespace Operators
}  // namespace Amanzi

#endif


