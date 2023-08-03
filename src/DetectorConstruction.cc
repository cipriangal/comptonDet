/// \file B4/B4d/src/DetectorConstruction.cc
/// \brief Implementation of the B4d::DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4SDManager.hh"
#include "G4SDChargedFilter.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSTrackLength.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

namespace B4d
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal
G4GlobalMagFieldMessenger* DetectorConstruction::fMagFieldMessenger = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Define materials
  DefineMaterials();

  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  // Lead material defined using NIST Manager
  auto nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_PbWO4");
  nistManager->FindOrBuildMaterial("G4_Galactic");
  nistManager->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
  nistManager->FindOrBuildMaterial("G4_C");
  nistManager->FindOrBuildMaterial("G4_Al");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{
  // Geometry parameters
  G4int  nofLayers = 20;
  G4double  absoThickness =  5.*mm;
  G4double  gapThickness =   5.*mm;
  G4double  calorSizeXY  = 20.*cm;

  auto  layerThickness = absoThickness + gapThickness;
  auto  calorThickness = nofLayers * layerThickness;
  auto  worldSizeXY = 1.2 * calorSizeXY;
  auto  worldSizeZ  = 1.5 * calorThickness;

  // Get materials
  auto defaultMaterial = G4Material::GetMaterial("G4_Galactic");
  auto absorberMaterial = G4Material::GetMaterial("G4_PbWO4");
  auto gapMaterial = G4Material::GetMaterial("G4_C");
  auto preRadMaterial = G4Material::GetMaterial("G4_Al");

  if ( ! defaultMaterial || ! absorberMaterial || ! gapMaterial ) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined.";
    G4Exception("DetectorConstruction::DefineVolumes()",
      "MyCode0001", FatalException, msg);
  }

  //
  // World
  //
  auto worldS
    = new G4Box("World",           // its name
                 worldSizeXY/2, worldSizeXY/2, worldSizeZ/2); // its size

  auto worldLV
    = new G4LogicalVolume(
                 worldS,           // its solid
                 defaultMaterial,  // its material
                 "World");         // its name

  auto worldPV = new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(),                         // at (0,0,0)
    worldLV,                                 // its logical volume
    "World",                                 // its name
    nullptr,                                 // its mother  volume
    false,                                   // no boolean operation
    0,                                       // copy number
    fCheckOverlaps);                         // checking overlaps


  //
  // Initial state info
  //
  auto iniS
    = new G4Box("iniS",             // its name
		calorSizeXY/2, calorSizeXY/2, 1*mm); // its size

  auto iniLV
    = new G4LogicalVolume(
			  iniS,             // its solid
			  defaultMaterial,      // its material
			  "iniLV");      // its name

  new G4PVPlacement(nullptr,                   // no rotation
		    G4ThreeVector(0., 0., -worldSizeZ/2 + 2*mm),  //  its position
		    iniLV,                                     // its logical volume
		    "initial",                                     // its name
		    worldLV,                                   // its mother  volume
		    false,                                     // no boolean operation
		    0,                                         // copy number
		    fCheckOverlaps);                           // checking overlaps

  //
  // pre-radiator
  //
  auto preRadS
    = new G4Box("preRadS",             // its name
		calorSizeXY/2, calorSizeXY/2, 1*mm); // its size

  auto preRadLV
    = new G4LogicalVolume(
			  preRadS,             // its solid
			  preRadMaterial,      // its material
			  "preRadLV");      // its name

  new G4PVPlacement(nullptr,                   // no rotation
		    G4ThreeVector(0., 0., -calorThickness/2 - 5*mm),  //  its position
		    preRadLV,                                     // its logical volume
		    "preRadiator",                             // its name
		    worldLV,                                   // its mother  volume
		    false,                                     // no boolean operation
		    0,                                         // copy number
		    fCheckOverlaps);                           // checking overlaps


  //
  // Calorimeter
  //
  auto calorimeterS
    = new G4Box("Calorimeter",     // its name
                 calorSizeXY/2, calorSizeXY/2, calorThickness/2); // its size

  auto calorLV
    = new G4LogicalVolume(
                 calorimeterS,    // its solid
                 defaultMaterial, // its material
                 "Calorimeter");  // its name

  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(),          // at (0,0,0)
    calorLV,                  // its logical volume
    "Calorimeter",            // its name
    worldLV,                  // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    fCheckOverlaps);          // checking overlaps

  //
  // Layer
  //
  auto layerS
    = new G4Box("Layer",           // its name
                 calorSizeXY/2, calorSizeXY/2, layerThickness/2); // its size

  auto layerLV
    = new G4LogicalVolume(
                 layerS,           // its solid
                 defaultMaterial,  // its material
                 "Layer");         // its name

  new G4PVReplica(
                 "Layer",          // its name
                 layerLV,          // its logical volume
                 calorLV,          // its mother
                 kZAxis,           // axis of replication
                 nofLayers,        // number of replica
                 layerThickness);  // witdth of replica

  //
  // Absorber
  //
  auto absorberS
    = new G4Box("Abso",            // its name
                 calorSizeXY/2, calorSizeXY/2, absoThickness/2); // its size

  auto absorberLV
    = new G4LogicalVolume(
                 absorberS,        // its solid
                 absorberMaterial, // its material
                 "AbsoLV");          // its name

  new G4PVPlacement(nullptr,                   // no rotation
    G4ThreeVector(0., 0., -gapThickness / 2),  //  its position
    absorberLV,                                // its logical volume
    "Abso",                                    // its name
    layerLV,                                   // its mother  volume
    false,                                     // no boolean operation
    0,                                         // copy number
    fCheckOverlaps);                           // checking overlaps

  //
  // Gap
  //
  auto gapS
    = new G4Box("Gap",             // its name
                 calorSizeXY/2, calorSizeXY/2, gapThickness/2); // its size

  auto gapLV
    = new G4LogicalVolume(
                 gapS,             // its solid
                 gapMaterial,      // its material
                 "GapLV");      // its name

  new G4PVPlacement(nullptr,                   // no rotation
    G4ThreeVector(0., 0., absoThickness / 2),  //  its position
    gapLV,                                     // its logical volume
    "Gap",                                     // its name
    layerLV,                                   // its mother  volume
    false,                                     // no boolean operation
    0,                                         // copy number
    fCheckOverlaps);                           // checking overlaps

  //
  // Visualization attributes
  //
  worldLV->SetVisAttributes (G4VisAttributes::GetInvisible());

  // auto simpleBoxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  // simpleBoxVisAtt->SetVisibility(true);
  // calorLV->SetVisAttributes(simpleBoxVisAtt);

  //
  // Always return the physical World
  //
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);
  //
  // Scorers
  //

  // declare Absorber as a MultiFunctionalDetector scorer
  //
  auto absDetector = new G4MultiFunctionalDetector("Absorber");
  G4SDManager::GetSDMpointer()->AddNewDetector(absDetector);

  G4VPrimitiveScorer* primitive;
  primitive = new G4PSEnergyDeposit("Edep");
  absDetector->RegisterPrimitive(primitive);

  primitive = new G4PSTrackLength("TrackLength");
  auto charged = new G4SDChargedFilter("chargedFilter");
  primitive ->SetFilter(charged);
  absDetector->RegisterPrimitive(primitive);

  SetSensitiveDetector("AbsoLV",absDetector);

  // declare Gap as a MultiFunctionalDetector scorer
  //
  auto gapDetector = new G4MultiFunctionalDetector("Gap");
  G4SDManager::GetSDMpointer()->AddNewDetector(gapDetector);

  primitive = new G4PSEnergyDeposit("Edep");
  gapDetector->RegisterPrimitive(primitive);

  primitive = new G4PSTrackLength("TrackLength");
  primitive ->SetFilter(charged);
  gapDetector->RegisterPrimitive(primitive);

  SetSensitiveDetector("GapLV",gapDetector);

  //
  // Magnetic field
  //
  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  G4ThreeVector fieldValue;
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);

  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
