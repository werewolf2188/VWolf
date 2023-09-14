//
//  LoadSettings.hpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 8/13/23.
//

#pragma once
#include <iostream>
#include "VWolf.h"

#define NUMSHADERS 4
#include <array>

namespace VWolfPup {
    // TODO: These variables will be moved eventually, but they are needed to
    // TODO: understand the initial values of the materials
    static const std::string DEFAULT_SHADER = "BlinnPhong";
    static const std::string DEFAULT_SKYBOX = "Skybox";
    // TODO: I don't know if I should move this or not
    static const std::string DEFAULT_GRID = "Grid";

// ----------------------------------------------- //
    void InitialLoad();
    void CreateDefaultMaterial(VWolf::Material& material);
    void CreateGridMaterial(VWolf::Material& material);
    void CreateDefaultSkyboxMaterial(VWolf::Material& material);
}
