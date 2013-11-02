/*
  The transport component of the Amanzi code, serial unit tests.
  License: BSD
  Author: Konstantin Lipnikov (lipnikov@lanl.gov)
*/

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <vector>

#include "UnitTest++.h"

#include "Teuchos_ParameterList.hpp"
#include "Teuchos_RCP.hpp"
#include "Teuchos_ParameterXMLFileReader.hpp"

#include "MeshFactory.hh"
#include "MeshAudit.hh"

#include "State.hh"
#include "Transport_PK.hh"


/* **************************************************************** 
 * Test LimiterBarthJespersen()routine.
 * ************************************************************* */
TEST(LIMITER_BARTH_JESPERSEN) {
  using namespace Teuchos;
  using namespace Amanzi;
  using namespace Amanzi::AmanziMesh;
  using namespace Amanzi::AmanziTransport;
  using namespace Amanzi::AmanziGeometry;

  std::cout << "Test: read transport XML file" << endl;
#ifdef HAVE_MPI
  Epetra_MpiComm* comm = new Epetra_MpiComm(MPI_COMM_WORLD);
#else
  Epetra_SerialComm* comm = new Epetra_SerialComm();
#endif

  /* read parameter list */
  string xmlFileName = "test/transport_limiters.xml";
  ParameterXMLFileReader xmlreader(xmlFileName);
  ParameterList plist = xmlreader.getParameters();  
 
  /* create an MSTK mesh framework */
  ParameterList region_list = plist.get<Teuchos::ParameterList>("Regions");
  GeometricModelPtr gm = new GeometricModel(3, region_list, comm);

  FrameworkPreference pref;
  pref.clear();
  pref.push_back(MSTK);

  MeshFactory factory(comm);
  factory.preference(pref);
  RCP<const Mesh> mesh = factory(0.0,0.0,0.0, 1.0,1.0,1.0, 3, 4, 7, gm); 

  /* create a simple state and populate it */
  Amanzi::VerboseObject::hide_line_prefix = true;

  std::vector<std::string> component_names;
  component_names.push_back("Component 0");

  RCP<State> S = rcp(new State());
  S->RegisterDomainMesh(mesh);

  Transport_PK TPK(plist, S, component_names);
  TPK.CreateDefaultState(mesh, 1);

  /* modify the default state for the problem at hand */
  std::string passwd("state"); 
  Teuchos::RCP<Epetra_MultiVector> 
      flux = S->GetFieldData("darcy_flux", passwd)->ViewComponent("face", false);

  AmanziGeometry::Point velocity(1.0, 0.0, 0.0);
  int nfaces_owned = mesh->num_entities(AmanziMesh::FACE, AmanziMesh::OWNED);
  for (int f = 0; f < nfaces_owned; f++) {
    const AmanziGeometry::Point& normal = mesh->face_normal(f);
    (*flux)[0][f] = velocity * normal;
  }

  /* initialize a transport process kernel */
  TPK.InitPK();
  TPK.PrintStatistics();
  double dT = TPK.CalculateTransportDt();  // We call it to identify upwind cells.

  /* create a linear field */
  const Epetra_Map& cmap = mesh->cell_map(false);
  RCP<Epetra_Vector> scalar_field = rcp(new Epetra_Vector(cmap));

  RCP<CompositeVector> gradient = CreateCompositeVector(mesh, AmanziMesh::CELL, 3, true);
  gradient->CreateData();
  RCP<Epetra_MultiVector> grad = gradient->ViewComponent("cell", false);

  int ncells = mesh->num_entities(AmanziMesh::CELL, AmanziMesh::OWNED);
  for (int c = 0; c < ncells; c++) {
    const AmanziGeometry::Point& xc = mesh->cell_centroid(c);
    (*scalar_field)[c] = 5.0 - xc[0] - 0.5 * xc[1] - 0.2 * xc[2];
    (*grad)[0][c] = -1.0;
    (*grad)[1][c] = -0.5;
    (*grad)[2][c] = -0.2;
  }

  /* calculate and verify limiters */
  RCP<Epetra_Vector> limiter = rcp(new Epetra_Vector(cmap));
  TPK.LimiterBarthJespersen(0, scalar_field, gradient, limiter);

  for (int c = 0; c < ncells - 1; c++) {  // the corner cell gives limiter=0
    CHECK_CLOSE(1.0, (*limiter)[c], 1e-6);
  }
 
  delete comm;
}



