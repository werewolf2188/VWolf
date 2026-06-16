//
//  Importers.h
//  VWolfPup
//
//  Created by Enrique Moises on 6/7/26.
//

#pragma once

#include "AssetImporter.h"
#include "ShaderImporter.h"
#include "MaterialImporter.h"
#include "ModelImporter.h"
#include "DefaultImporter.h"

#include <boost/mpl/list.hpp>

using AllImporters = boost::mpl::list<
                        VWolfPup::ShaderImporter,
                        VWolfPup::MaterialImporter,
                        VWolfPup::ModelImporter,
                        VWolfPup::DefaultImporter
                    >;
