//
//  MaterialImporter.cpp
//  VWolfPup
//
//  Created by Enrique Moises on 6/6/26.
//

#include "MaterialImporter.h"
#include "../LoadSettings.h"

namespace VWolfPup {
    uint32_t MaterialImporter::LoadPriority(std::filesystem::path path) const {
        return 2;
    }

    bool MaterialImporter::Import(std::filesystem::path path, VWolf::UUID _id) {
        auto mat = VWolf::Material::Load(path, _id);
        VWolf::Ref<Defaults> _defaults = Defaults::Get();
        
        VWolf::Ref<VWolf::Material> newMat = VWolf::MaterialLibrary::GetMaterial(mat->GetName());
        if (_defaults->GetDefaultMaterialName() == mat->GetName()) {
            mat->SetAsDefault();
        } else if (_defaults->GetDefaultSkyBoxMaterialName() == mat->GetName()) {
            VWolf::Ref<VWolf::Material> newMat2 = VWolf::MaterialLibrary::GetMaterial(_defaults->GetDefaultSkyBoxMaterialName());
            mat->SetTexture("skybox",
                            VWolf::Texture::LoadCubemap({ "assets/skybox/right.png",
                                                          "assets/skybox/left.png",
                                                          "assets/skybox/top.png",
                                                          "assets/skybox/bottom.png",
                                                          "assets/skybox/front.png",
                                                          "assets/skybox/back.png" }));
        }
        return true;
    }


    const std::vector<Extension> MaterialImporter::Extensions() const {
        return {
            Extension("Material", ".vwolfmat")
        };
    }

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_NO_NAME(MaterialImporter);
}
