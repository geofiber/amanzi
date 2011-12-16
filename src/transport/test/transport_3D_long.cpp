/*
The transport component of the Amanzi code, serial unit tests.
License: BSD
Author: Konstantin Lipnikov (lipnikov@lanl.gov)
*/

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>

#include "UnitTest++.h"

#include "Teuchos_ParameterList.hpp"
#include "Teuchos_RCP.hpp"
#include "Teuchos_XMLParameterListHelpers.hpp"

#include "MSTK_types.h"
#include "Mesh_MSTK.hh"
#include "MeshAudit.hh"
#include "gmv_mesh.hh"

#include "State.hpp"
#include "Transport_PK.hpp"


Amanzi::AmanziGeometry::Point f_velocity(const Amanzi::AmanziGeometry::Point& x, double t ) { 
  return Amanzi::AmanziGeometry::Point(1.0, 0.0, 0.0);
}


/* **************************************************************** */
TEST(ADVANCE_WITH_3D_MESH) {
  using namespace Teuchos;
  using namespace Amanzi;
  using namespace Amanzi::AmanziMesh;
  using namespace Amanzi::AmanziTransport;
  using namespace Amanzi::AmanziGeometry;

cout << "Test: 2.5D transport on a cubic mesh for long time" << endl;
#ifdef HAVE_MPI
  Epetra_MpiComm  *comm = new Epetra_MpiComm(MPI_COMM_WORLD);
#else
  Epetra_SerialComm  *comm = new Epetra_SerialComm();
#endif

  /* read parameter list */
  ParameterList parameter_list;
  string xmlFileName = "test/transport_3D_long.xml";
  updateParametersFromXmlFile(xmlFileName, &parameter_list);

  /* create an MSTK mesh framework */
  ParameterList region_list = parameter_list.get<Teuchos::ParameterList>("Regions");
  GeometricModelPtr gm = new GeometricModel(2, region_list);
  RCP<Mesh> mesh = rcp(new Mesh_MSTK("test/rect3D_50x50x1.exo", MPI_COMM_WORLD, 3, gm));

  //Amanzi::MeshAudit audit(mesh);
  //audit.Verify();   
  
  /* create a MPC state with one component */
  int num_components = 1;
  State mpc_state(num_components, mesh);
 
  /* create a transport state from the MPC state and populate it */
  RCP<Transport_State> TS = rcp(new Transport_State(mpc_state));

  TS->analytic_darcy_flux(f_velocity);
  TS->analytic_porosity();
  TS->analytic_water_saturation();
  TS->analytic_water_density();

  /* initialize a transport process kernel from the transport state */
  Transport_PK TPK(parameter_list, TS);
  TPK.set_standalone_mode(true);
  TPK.print_statistics();
 
  /* advance the transport state */
  int iter, k;
  double T = 0.0;
  RCP<Transport_State> TS_next = TPK.get_transport_state_next();

  RCP<Epetra_MultiVector> tcc = TS->get_total_component_concentration();
  RCP<Epetra_MultiVector> tcc_next = TS_next->get_total_component_concentration();

  iter = 0;
  bool flag = true;
  while (T < 0.3) {
    double dT = TPK.calculate_transport_dT();
    TPK.advance(dT);
    T += dT;
    iter++;

    if (T>0.1 && flag) {
      flag = false;
      if (TPK.MyPID == 0) {
        GMV::open_data_file(*mesh, (std::string)"transport.gmv");
        GMV::start_data();
        GMV::write_cell_data(*tcc_next, 0, "component0");
        GMV::close_data_file();
      }
      break;
    }

    *tcc = *tcc_next;
  }
 
  delete comm;
}





