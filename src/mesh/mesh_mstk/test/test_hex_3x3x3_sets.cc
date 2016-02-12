#include <UnitTest++.h>

#include <iostream>

#include "../Mesh_MSTK.hh"


#include "Epetra_Map.h"
#include "Epetra_MpiComm.h"
#include "Teuchos_ParameterList.hpp"
#include "Teuchos_ParameterXMLFileReader.hpp"


TEST(MSTK_HEX_3x3x3_SETS)
{

  std::string expcsetnames[15] = {"Bottom LS", "Middle LS", "Top LS", 
                                 "Bottom+Middle Box", "Top Box",
                                 "Sample Point InCell", "Sample Point OnFace",
                                 "Sample Point OnEdge", "Sample Point OnVertex",
                                 "Bottom ColFunc", "Middle ColFunc", "Top ColFunc",
                                 "Cell Set 1", "Cell Set 2", "Cell Set 3"};
  Amanzi::AmanziMesh::Set_ID csetsize;
  
  std::string expfsetnames[7] = {"Face 101", "Face 102", 
				  "Face 10005", "Face 20004", "Face 30004",
                                  "ZLO FACE Plane", "YLO FACE Box"};


  std::string expnsetnames[2] = {"INTERIOR XY PLANE", "TOP BOX"};

			   
  Teuchos::RCP<Epetra_MpiComm> comm(new Epetra_MpiComm(MPI_COMM_WORLD));


  std::string infilename = "test/hex_3x3x3.xml";
  Teuchos::ParameterXMLFileReader xmlreader(infilename);

  Teuchos::ParameterList reg_spec(xmlreader.getParameters());

  Teuchos::RCP<Amanzi::AmanziGeometry::GeometricModel> gm =
      Teuchos::rcp(new Amanzi::AmanziGeometry::GeometricModel(3, reg_spec, comm.get()));

  // Load a mesh consisting of 3x3x3 elements

  Teuchos::RCP<Amanzi::AmanziMesh::Mesh> mesh(new Amanzi::AmanziMesh::Mesh_MSTK("test/hex_3x3x3_sets.exo",comm.get(),3,gm));


  Teuchos::ParameterList::ConstIterator i;
  for (i = reg_spec.begin(); i != reg_spec.end(); i++) {
        const std::string reg_name = reg_spec.name(i);     

    Teuchos::ParameterList reg_params = reg_spec.sublist(reg_name);

    // See if the geometric model has a region by this name
  
    Teuchos::RCP<const Amanzi::AmanziGeometry::Region> reg =
        gm->FindRegion(reg_name);

    CHECK(reg.get());

    // Do their names match ?

    CHECK_EQUAL(reg->name(),reg_name);


    // Get the region info directly from the XML and compare
  
    Teuchos::ParameterList::ConstIterator j = reg_params.begin(); 

    std::string shape = reg_params.name(j);

    if (shape == "Region: Plane") {

      if (reg_name == "ZLO FACE Plane") {

        // Do we have a valid sideset by this name
        
        CHECK(mesh->valid_set_name(reg_name,Amanzi::AmanziMesh::FACE));
        
        int j;
        for (j = 0; j < 7; j++) {
          if (expfsetnames[j] == reg_name) break;
        }
        
        if (j >= 7)
          std::cerr << "Side set " << reg_name << "not found" << "\n";
        CHECK(j < 7);
        
        
        // Verify that we can get the number of entities in the set
        
        int set_size = mesh->get_set_size(reg_name,Amanzi::AmanziMesh::FACE,Amanzi::AmanziMesh::OWNED);
        
        
        // Verify that we can retrieve the set entities
        
        Amanzi::AmanziMesh::Entity_ID_List setents;
        mesh->get_set_entities(reg_name,Amanzi::AmanziMesh::FACE,Amanzi::AmanziMesh::OWNED,&setents);
        
      }
      else if (reg_name == "INTERIOR XY PLANE") {

        // Do we have a valid nodeset by this name
        
        CHECK(mesh->valid_set_name(reg_name,Amanzi::AmanziMesh::NODE));
        
        int j;
        for (j = 0; j < 2; j++) {
          if (expnsetnames[j] == reg_name) break;
        }

        if (j >= 2)
          std::cerr << "Nodeset " << reg_name << "not found" << "\n";
        CHECK(j < 2);
        
        
        // Verify that we can get the number of entities in the set
        
        int set_size = mesh->get_set_size(reg_name,Amanzi::AmanziMesh::NODE,Amanzi::AmanziMesh::USED);
        
        
        // Verify that we can retrieve the set entities
        
        Amanzi::AmanziMesh::Entity_ID_List setents;
        mesh->get_set_entities(reg_name,Amanzi::AmanziMesh::NODE,Amanzi::AmanziMesh::USED,&setents);
        
      }

    }
    else if (shape == "Region: Box") {

      Teuchos::ParameterList box_params = reg_params.sublist(shape);
      Teuchos::Array<double> pmin = box_params.get< Teuchos::Array<double> >("Low Coordinate");
      Teuchos::Array<double> pmax = box_params.get< Teuchos::Array<double> >("High Coordinate");

      if (pmin[0] == pmax[0] || pmin[1] == pmax[1] || pmin[2] == pmax[2])
	{          

	  // This is a reduced dimensionality box - request a faceset or nodeset

          if (reg_name == "YLO FACE Box") {

            // Do we have a valid sideset by this name
            
            CHECK(mesh->valid_set_name(reg_name,Amanzi::AmanziMesh::FACE));
            
            int j;
            for (j = 0; j < 7; j++) {
              if (expfsetnames[j] == reg_name) break;
            }
            
            if (j >= 7)
              std::cerr << "Side set " << reg_name << "not found" << "\n";
            CHECK(j < 7);
            
            
            // Verify that we can get the number of faces in the set
            
            int set_size = mesh->get_set_size(reg_name,Amanzi::AmanziMesh::FACE,Amanzi::AmanziMesh::OWNED);
            CHECK(set_size == 9);
            
            // Verify that we can retrieve the set entities
            
            Amanzi::AmanziMesh::Entity_ID_List setents;
            mesh->get_set_entities(reg_name,Amanzi::AmanziMesh::FACE,Amanzi::AmanziMesh::OWNED,&setents);

            // Also try to extract a node set using this same box and same set name

            CHECK(mesh->valid_set_name(reg_name,Amanzi::AmanziMesh::NODE));

            // Verify that we can the number of nodes in the set
            
            set_size = mesh->get_set_size(reg_name,Amanzi::AmanziMesh::NODE,Amanzi::AmanziMesh::OWNED);
            CHECK(set_size == 16);

            // Verify that we can retrieve the set entities

            setents.clear();
            mesh->get_set_entities(reg_name,Amanzi::AmanziMesh::NODE,Amanzi::AmanziMesh::OWNED,&setents);

          }
          else if (reg_name == "TOP BOX") {

            // Do we have a valid set by this name
            
            CHECK(mesh->valid_set_name(reg_name,Amanzi::AmanziMesh::NODE));
            
            int j;
            for (j = 0; j < 2; j++) {
              if (expnsetnames[j] == reg_name) break;
            }
            
            if (j >= 2)
              std::cerr << "Node set " << reg_name << " not found" << "\n";
            CHECK(j < 2);
            
            
            // Verify that we can get the number of entities in the set
            
            int set_size = mesh->get_set_size(reg_name,Amanzi::AmanziMesh::NODE,Amanzi::AmanziMesh::USED);
            
            
            // Verify that we can retrieve the set entities
            
            Amanzi::AmanziMesh::Entity_ID_List setents;
            mesh->get_set_entities(reg_name,Amanzi::AmanziMesh::NODE,Amanzi::AmanziMesh::USED,&setents);
            
          }

	}
      else 
	{
	  // Do we have a valid cellset by this name
	  
	  CHECK(mesh->valid_set_name(reg_name,Amanzi::AmanziMesh::CELL));
	  
	  // Find the expected cell set info corresponding to this name 
	  
	  int j;
	  for (j = 0; j < 9; j++)
	    if (reg_name == expcsetnames[j]) break;
	  
          if (j >= 9)
            std::cerr << "Cell set " << reg_name << " not found" << "\n";
	  CHECK(j < 9);
	  
	  // Verify that we can get the number of entities in the set
	  
	  int set_size = mesh->get_set_size(reg_name,Amanzi::AmanziMesh::CELL,Amanzi::AmanziMesh::OWNED);
	  
	  // Verify that we can retrieve the set entities
	  
	  Amanzi::AmanziMesh::Entity_ID_List setents;
	  mesh->get_set_entities(reg_name,Amanzi::AmanziMesh::CELL,Amanzi::AmanziMesh::OWNED,&setents);
	}
    }
    else if (shape == "Region: Point") {

      // Do we have a valid cell set by this name
      
      CHECK(mesh->valid_set_name(reg_name,Amanzi::AmanziMesh::CELL));
      
      int j;
      for (j = 0; j < 9; j++) {
        if (expcsetnames[j] == reg_name) break;
      }
      
      if (j >= 9) 
        std::cerr << "Cell set corresponding to region " << reg_name << " not found" << "\n";
      CHECK(j < 9);
            
            
      // Verify that we can get the number of entities in the set
      
      int set_size = mesh->get_set_size(reg_name,Amanzi::AmanziMesh::CELL,Amanzi::AmanziMesh::USED);
      
      
      // Verify that we can retrieve the set entities
      
      Amanzi::AmanziMesh::Entity_ID_List setents;
      mesh->get_set_entities(reg_name,Amanzi::AmanziMesh::CELL,Amanzi::AmanziMesh::USED,&setents);
      
    }
    else if (shape == "Region: Labeled Set") {

      Teuchos::ParameterList lsparams = reg_params.sublist(shape);

      // Find the entity type in this parameter list

      std::string entity_type = lsparams.get<std::string>("Entity");

      if (entity_type == "Face") {

	// Do we have a valid sideset by this name

	CHECK(mesh->valid_set_name(reg_name,Amanzi::AmanziMesh::FACE));

        // Find the expected face set info corresponding to this name

        int j;
        for (j = 0; j < 7; j++)
          if (reg_name == expfsetnames[j]) break;

        if (j >= 7)
          std::cerr << "Side set " << reg_name << "not found" << "\n";
        CHECK(j < 7);
	
	// Verify that we can get the number of entities in the set
	
	int set_size = mesh->get_set_size(reg_name,Amanzi::AmanziMesh::FACE,Amanzi::AmanziMesh::OWNED);
		

	// Verify that we can retrieve the set entities
	
        Amanzi::AmanziMesh::Entity_ID_List setents;
	mesh->get_set_entities(reg_name,Amanzi::AmanziMesh::FACE,Amanzi::AmanziMesh::OWNED,&setents);

      }
      else if (entity_type == "Cell") {

	// Do we have a valid cellset  by this name

	CHECK(mesh->valid_set_name(reg_name,Amanzi::AmanziMesh::CELL));
	
        // Find the expected face set info corresponding to this name

        int j;
        for (j = 0; j < 15; j++)
          if (reg_name == expcsetnames[j]) break;

        if (j >= 15)
          std::cerr << "Cell set " << reg_name << " not found" << "\n";
        CHECK(j < 15);
	
	// Verify that we can get the number of entities in the set
	
	int set_size = mesh->get_set_size(reg_name,Amanzi::AmanziMesh::CELL,Amanzi::AmanziMesh::OWNED);

	// Verify that we can retrieve the set entities
	
        Amanzi::AmanziMesh::Entity_ID_List setents;
	mesh->get_set_entities(reg_name,Amanzi::AmanziMesh::CELL,Amanzi::AmanziMesh::OWNED,&setents);
      }

    }
    else if (shape == "Region: Color Function") {

      // Do we have a valid sideset by this name
      
      CHECK(mesh->valid_set_name(reg_name,Amanzi::AmanziMesh::CELL));
      
      // Find the expected face set info corresponding to this name
      
      int j;
      for (j = 0; j < 15; j++)
        if (reg_name == expcsetnames[j]) break;
      
      if (j >= 15)
        std::cerr << "Cell set corresponding to region " << reg_name << " not found" << "\n";
      CHECK(j < 15);
      
      // Verify that we can get the number of entities in the set
      
      int set_size = mesh->get_set_size(reg_name,Amanzi::AmanziMesh::CELL,Amanzi::AmanziMesh::OWNED);
      
      // Verify that we can retrieve the set entities
      
      Amanzi::AmanziMesh::Entity_ID_List setents;
      mesh->get_set_entities(reg_name,Amanzi::AmanziMesh::CELL,Amanzi::AmanziMesh::OWNED,&setents);
      
    }
  }
}

