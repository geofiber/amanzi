/*
  WhetStone, Version 2.2
  Release name: naka-to.

  Copyright 2010-201x held jointly by LANS/LANL, LBNL, and PNNL. 
  Amanzi is released under the three-clause BSD License. 
  The terms of use and "as is" disclaimer for this license are 
  provided in the top-level COPYRIGHT file.

  Author: Konstantin Lipnikov (lipnikov@lanl.gov)
*/

#include "MFD3D_BernardiRaugel.hh"
#include "MFD3D_CrouzeixRaviart.hh"
#include "MFD3D_CrouzeixRaviartAnyOrder.hh"
#include "MFD3D_CrouzeixRaviartSerendipity.hh"
#include "MFD3D_Diffusion.hh"
#include "DG_Modal.hh"
#include "MFD3D_Generalized_Diffusion.hh"
#include "MFD3D_Electromagnetics.hh"
#include "MFD3D_Elasticity.hh"
#include "MFD3D_Lagrange.hh"
#include "MFD3D_LagrangeSerendipity.hh"

namespace Amanzi {
namespace WhetStone {

RegisteredFactory<MFD3D_BernardiRaugel> MFD3D_BernardiRaugel::factory_("BernardiRaugel");
RegisteredFactory<MFD3D_CrouzeixRaviart> MFD3D_CrouzeixRaviart::factory_("CrouzeixRaviart");
RegisteredFactory<MFD3D_CrouzeixRaviartAnyOrder> MFD3D_CrouzeixRaviartAnyOrder::factory_("CrouzeixRaviart high order");
RegisteredFactory<MFD3D_CrouzeixRaviartSerendipity> MFD3D_CrouzeixRaviartSerendipity::factory_("CrouzeixRaviart serendipity");
RegisteredFactory<MFD3D_Diffusion> MFD3D_Diffusion::factory_("diffusion");
RegisteredFactory<MFD3D_Generalized_Diffusion> MFD3D_Generalized_Diffusion::factory_("diffusion generalized");
RegisteredFactory<MFD3D_Elasticity> MFD3D_Elasticity::factory_("elasticity");
RegisteredFactory<MFD3D_Electromagnetics> MFD3D_Electromagnetics::factory_("electromagnetics");
RegisteredFactory<MFD3D_Lagrange> MFD3D_Lagrange::factory_("Lagrange");
RegisteredFactory<MFD3D_LagrangeSerendipity> MFD3D_LagrangeSerendipity::factory_("Lagrange serendipity");

RegisteredFactory<DG_Modal> DG_Modal::factory_("dg modal");

}  // namespace WhetStone
}  // namespace Amanzi

