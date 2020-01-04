/*
  Multiphase PK
 
  Copyright 2010-201x held jointly by LANS/LANL, LBNL, and PNNL. 
  Amanzi is released under the three-clause BSD License. 
  The terms of use and "as is" disclaimer for this license are 
  provided in the top-level COPYRIGHT file.

  Authors: Konstantin Lipnikov (lipnikov@lanl.gov)

  Boundary function.
*/

#ifndef AMANZI_MULTIPHASE_BOUNDARY_FUNCTION_HH_
#define AMANZI_MULTIPHASE_BOUNDARY_FUNCTION_HH_

#include <string>
#include <vector>

#include "Teuchos_ParameterList.hpp"
#include "Teuchos_RCP.hpp"

#include "Mesh.hh"
#include "PK_DomainFunction.hh"

namespace Amanzi {
namespace Multiphase {

class MultiphaseBoundaryFunction : public PK_DomainFunction {
 public:
  MultiphaseBoundaryFunction()
      : rainfall_(false),
        bc_name_("underfined") {};

  MultiphaseBoundaryFunction(const Teuchos::ParameterList& plist);
  void ComputeSubmodel(const Teuchos::RCP<const AmanziMesh::Mesh>& mesh);

  // modifiers and access
  void set_bc_name(const std::string& name) { bc_name_ = name; }
  std::string bc_name() { return bc_name_; }

 private:
  bool rainfall_;
  std::string bc_name_;
};

}  // namespace Multiphase
}  // namespace Amanzi

#endif
