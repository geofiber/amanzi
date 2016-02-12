/* -*-  mode: c++; c-default-style: "google"; indent-tabs-mode: nil -*- */
// -------------------------------------------------------------
/**
 * @file   FrameworkTraits.hh
 * @author William A. Perkins
 * @date Mon Aug  1 13:47:02 2011
 * 
 * @brief  
 * 
 * 
 */
// -------------------------------------------------------------
// -------------------------------------------------------------
// Created March 10, 2011 by William A. Perkins
// Last Change: Mon Aug  1 13:47:02 2011 by William A. Perkins <d3g096@PE10900.pnl.gov>
// -------------------------------------------------------------
#ifndef _FrameworkTraits_hh_
#define _FrameworkTraits_hh_

#include <Epetra_MpiComm.h>
#include <Teuchos_RCP.hpp>
#include <Teuchos_ParameterList.hpp>

#include "MeshFramework.hh"
#include "MeshFileType.hh"
#include "Mesh.hh"

#include "GeometricModel.hh"

#include "errors.hh"
#include "exceptions.hh"
#include "VerboseObject.hh"

namespace Amanzi {
namespace AmanziMesh {

/// Is the specified framework available
extern bool framework_available(const Framework& f);

/// General routine to see if a format can be read by particular framework
extern bool framework_reads(const Framework& f, const Format& fmt, const bool& parallel);

/// Read a mesh
extern Teuchos::RCP<Mesh> 
framework_read(const Epetra_MpiComm *comm, const Framework& f, 
               const std::string& fname,
               const Teuchos::RCP<const AmanziGeometry::GeometricModel>& gm =  Teuchos::null,
               const Teuchos::RCP<const VerboseObject>& verbobj = Teuchos::null,
               const bool request_faces = true,
               const bool request_edges = false);

/// General routine to see if a mesh can be generated by a particular framework
extern bool framework_generates(const Framework& f, const bool& parallel, const unsigned int& dimension);

/// General routine to see if a mesh can be extracted from another mesh by a particular framework
extern bool framework_extracts(const Framework& f, const bool& parallel, const unsigned int& dimension);

/// Generate a hexahedral mesh
extern Teuchos::RCP<Mesh> 
framework_generate(const Epetra_MpiComm *comm, const Framework& f, 
                   const double& x0, const double& y0, const double& z0,
                   const double& x1, const double& y1, const double& z1,
                   const unsigned int& nx, const unsigned int& ny, 
                   const unsigned int& nz,
                   const Teuchos::RCP<const AmanziGeometry::GeometricModel>& gm = Teuchos::null,
                   const Teuchos::RCP<const VerboseObject>& verbobj = Teuchos::null,
                   const bool request_faces = true,
                   const bool request_edges = false);

/// Generate a quadrilateral mesh
extern Teuchos::RCP<Mesh> 
framework_generate(const Epetra_MpiComm *comm, const Framework& f, 
                   const double& x0, const double& y0,
                   const double& x1, const double& y1,
                   const unsigned int& nx, const unsigned int& ny,
                   const Teuchos::RCP<const AmanziGeometry::GeometricModel>& gm = Teuchos::null,
                   const Teuchos::RCP<const VerboseObject>& verbobj = Teuchos::null,
                   const bool request_faces = true,
                   const bool request_edges = false);

extern Teuchos::RCP<Mesh> 
framework_generate(const Epetra_MpiComm *comm, const Framework& f, 
                   Teuchos::ParameterList &parameter_list,
                   const Teuchos::RCP<const AmanziGeometry::GeometricModel>& gm = Teuchos::null,
                   const Teuchos::RCP<const VerboseObject>& verbobj = Teuchos::null,
                   const bool request_faces = true,
                   const bool request_edges = false);

extern Teuchos::RCP<Mesh>
framework_extract(const Epetra_MpiComm *comm, const Framework& f,
                  const Mesh *inmesh,
                  const std::vector<std::string>& setnames,
                  const Entity_kind setkind,
                  const bool flatten = false,
                  const bool extrude = false,
                  const bool request_faces = true,
                  const bool request_edges = false);

} // namespace AmanziMesh
} // namespace Amanzi

#endif
