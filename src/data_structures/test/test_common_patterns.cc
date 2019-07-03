/*
  Mesh operations

  Copyright 2010-2012 held jointly by LANS/LANL, LBNL, and PNNL. 
  Amanzi is released under the three-clause BSD License. 
  The terms of use and "as is" disclaimer for this license are 
  provided in the top-level COPYRIGHT file.

  Author: Ethan Coon (ecoon@lanl.gov)

  This test suite is intended to explore how to access mesh operations on
  devices.
  
*/
#include <vector>

#include "UnitTest++.h"
#include "Teuchos_GlobalMPISession.hpp"
#include "Teuchos_RCP.hpp"

#include "MeshFactory.hh"
#include "AmanziTypes.hh"
#include "AmanziComm.hh"
#include "AmanziVector.hh"
#include "CompositeVector.hh"
#include "CompositeVectorSpace.hh"

using namespace Amanzi;
using namespace Amanzi::AmanziMesh;


struct TestHarness {
  Comm_ptr_type comm;
  Teuchos::RCP<Mesh> mesh;

  TestHarness() {
    comm = getDefaultComm();
    MeshFactory meshfactory(comm);
    mesh = meshfactory.create(0.0, 0.0, 0.0, 4.0, 4.0, 4.0, 8, 1, 1);
  }

  ~TestHarness() {}

  Teuchos::RCP<CompositeVector> CreateVec(const std::string& name, Entity_kind kind, int num_vectors) {
    auto x_space = Teuchos::rcp(new CompositeVectorSpace());
    x_space->SetMesh(mesh)->SetGhosted()
        ->SetComponents({name,}, {kind,}, {num_vectors,});
    return x_space->Create();
  }
};



SUITE(COMMON_MESH_OPERATIONS) {

  TEST_FIXTURE(TestHarness, FOR_EACH_CELL_VOLUME_LAMBDA) {
    // does a Kokkos for_each over cell volumes to calculate a derived quantity using lambdas
    auto sl = CreateVec("cell", Entity_kind::CELL, 1);
    sl->PutScalar(0.5);
    Teuchos::RCP<const CompositeVector> sl_c(sl);

    auto poro = CreateVec("cell", Entity_kind::CELL, 1);
    poro->PutScalar(0.25);
    Teuchos::RCP<const CompositeVector> poro_c(poro);

    auto wc = CreateVec("cell", Entity_kind::CELL, 1);

    // compute on device
    {
      auto sl_view = sl_c->ViewComponent<AmanziDefaultDevice>("cell", 0, false);
      auto poro_view = poro_c->ViewComponent<AmanziDefaultDevice>("cell", 0, false);
      auto wc_view = wc->ViewComponent<AmanziDefaultDevice>("cell", 0, false);
      auto cv_view = mesh->cell_volumes(); 

      std::cout<<"cv: "<<cv_view.extent(0)<<"sl: "<<sl_view.extent(0)<<std::endl; 
      assert(cv_view.extent(0) >= sl_view.extent(0)); 
      
      //typedef Kokkos::RangePolicy<AmanziDefaultDevice, LO> Policy_type;
      Kokkos::parallel_for(sl_view.extent(0),
                           KOKKOS_LAMBDA(const LO i) {
                             wc_view[i] = sl_view[i] * poro_view[i] * cv_view(i);
                           });
    }

    // test on the host
    {
      Teuchos::RCP<const CompositeVector> wc_c(wc);
      auto wc_view = wc_c->ViewComponent<AmanziDefaultHost>("cell", 0, false);
      for (LO i=0; i!=wc_view.extent(0); ++i) {
        CHECK_CLOSE(0.5 * 0.25 * 8.0, wc_view(i), 1.e-10);
      }
    }

  }
}
    
    
    
