#include <UnitTest++.h>

#include <iostream>

#include "../Mesh_maps_moab.hh"
#include "../../mesh_data/Entity_kind.hh"


#include "Epetra_Map.h"
#include "Epetra_MpiComm.h"

#include "mpi.h"


TEST(MOAB_HEX_3x3x2)
{

  int i, j, k, err, nc, nf, nv;
  unsigned int faces[6], nodes[8];
  int facedirs[6];
  double ccoords[24], fcoords[12];

  int NV = 18;
  int NF = 20;
  int NC = 4;
  double xyz[18][3] = {{-0.5,-0.5, 0.25},
		       {-0.5,-0.5,-0.25},
		       {-0.5, 0,  -0.25},
		       {-0.5, 0,   0.25},
		       { 0,  -0.5, 0.25},
		       { 0,  -0.5,-0.25},
		       { 0,   0,  -0.25},
		       { 0,   0,   0.25},
		       {-0.5, 0.5,-0.25},
		       {-0.5, 0.5, 0.25},
		       { 0,   0.5,-0.25},
		       { 0,   0.5, 0.25},
		       { 0.5,-0.5, 0.25},
		       { 0.5,-0.5,-0.25},
		       { 0.5, 0,  -0.25},
		       { 0.5, 0,   0.25},
		       { 0.5, 0.5,-0.25},
		       { 0.5, 0.5, 0.25}};
  unsigned int cellnodes[4][8] = {{0,1,2,3,4,5,6,7},
				  {3,2,8,9,7,6,10,11},
				  {4,5,6,7,12,13,14,15},
				  {7,6,10,11,15,14,16,17}};
  unsigned int facenodes[20][4] = {{3,0,4,7},
				   {9,3,7,11},
				   {7,4,12,15},
				   {11,7,15,17},
				   {1,2,6,5},
				   {2,8,10,6},
				   {5,6,14,13},
				   {6,10,16,14},
				   {0,1,5,4},
				   {4,5,13,12},
				   {0,3,2,1},
				   {3,9,8,2},
				   {8,9,11,10},
				   {10,11,17,16},
				   {12,13,14,15},
				   {15,14,16,17},
				   {2,3,7,6},
				   {4,5,6,7},
				   {7,6,10,11},
				   {6,7,15,14}};


  // Load a single hex from the hex1.exo file

  Mesh_maps_moab mesh("test/hex_3x3x2_ss.exo",MPI_COMM_WORLD);


  nv = mesh.count_entities(Mesh_data::NODE,OWNED);
  CHECK_EQUAL(NV,nv);

  for (i = 0; i < nv; i++) {
    double coords[3];

    mesh.node_to_coordinates(i,coords,coords+6);
    CHECK_ARRAY_EQUAL(xyz[i],coords,3);
  }


  nf = mesh.count_entities(Mesh_data::FACE,OWNED);
  CHECK_EQUAL(NF,nf);
  
  nc = mesh.count_entities(Mesh_data::CELL,OWNED);
  CHECK_EQUAL(NC,nc);
    
  for (i = 0; i < nc; i++) {
    mesh.cell_to_faces(i,faces,faces+6);
    mesh.cell_to_face_dirs(i,facedirs,facedirs+6);

    for (j = 0; j < 6; j++) {
      mesh.face_to_nodes(faces[j],nodes,nodes+4);
      mesh.face_to_coordinates(faces[j],fcoords,fcoords+12);
      
      for (k = 0; k < 4; k++) {
	CHECK_EQUAL(facenodes[faces[j]][k],nodes[k]);
	CHECK_ARRAY_EQUAL(xyz[facenodes[faces[j]][k]],&(fcoords[3*k]),3);
      }
    }

  
    mesh.cell_to_nodes(i,nodes,nodes+8);
    mesh.cell_to_coordinates(i,ccoords,ccoords+24);
    
    for (j = 0; j < 8; j++) {
      CHECK_EQUAL(cellnodes[i][j],nodes[j]);
      CHECK_ARRAY_EQUAL(xyz[cellnodes[i][j]],&(ccoords[3*j]),3);
    }
  }


  // Verify the sidesets

  int ns;
  ns = mesh.num_sets(Mesh_data::FACE);
  CHECK_EQUAL(7,ns);

  unsigned int setids[7], expsetids[7]={1,101,102,103,104,105,106};
  mesh.get_set_ids(Mesh_data::FACE,setids,setids+7);
  
  CHECK_ARRAY_EQUAL(expsetids,setids,7);

  unsigned int setsize, expsetsizes[7] = {16,4,4,2,2,2,2};
  unsigned int expsetfaces[7][16] = {{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
				     {0,1,2,3,0,0,0,0,0,0,0,0,0,0,0,0},
				     {4,5,6,7,0,0,0,0,0,0,0,0,0,0,0,0},
				     {8,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				     {10,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				     {12,13,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				     {14,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};


  for (i = 0; i < ns; i++) {
    unsigned int setfaces[16];

    setsize = mesh.get_set_size(setids[i],Mesh_data::FACE,OWNED);
    CHECK_EQUAL(expsetsizes[i],setsize);


    mesh.get_set(setids[i],Mesh_data::FACE, OWNED, setfaces, setfaces+setsize);
    
    CHECK_ARRAY_EQUAL(expsetfaces[i],setfaces,setsize);
  }


}

