//
//  AssetMetaFile.hpp
//  VWolfPup
//
//  Created by Enrique Moises on 6/3/26.
//

#pragma once
#include <filesystem>
#include "VWolf.h"
#include "Importers/Importers.h"

namespace VWolfPup {
    class AssetMetaFile;
}

namespace YAML {
    bool DeserializeImporter(const Node& node, VWolfPup::AssetMetaFile& rhs);

    struct ImporterDeserialier;
}

namespace VWolfPup {
    struct AssetMetaFile: public VWolf::Object {
    public:
        AssetMetaFile(): Object(VWolf::UUID::NewUUID()) {};
        AssetMetaFile(std::filesystem::path path);
        AssetMetaFile(const AssetMetaFile& asmf);
        AssetMetaFile(AssetMetaFile&& asmf);
        ~AssetMetaFile();
    public:
        bool Exists() { return std::filesystem::exists(metafile); }
        uint32_t LoadPriority() const { return importer->LoadPriority(path); }
    public:
        bool Create();
        bool Import();
    public:
        AssetMetaFile& operator=(const AssetMetaFile& other);
        AssetMetaFile& operator=(AssetMetaFile&& other);
    public:
        static std::string META_FILE_EXTENSION;
    public:
        static bool IsMetafile(std::filesystem::path path);
        static AssetMetaFile Load(std::filesystem::path path);
    private:
        int version = 1;
    private:
        std::filesystem::path metafile, path;
        AssetImporter* importer;
        long long lastModifiedTime;
    private:
        void SetPath(std::filesystem::path path);
    private:
        BOOST_DESCRIBE_CLASS(AssetMetaFile, (VWolf::Object), (), (id), (version, lastModifiedTime))
        VWOLF_SERIALIZATION_FRIENDS(AssetMetaFile)
        friend struct ImporterSerialier;
        friend struct ImporterExtensionValidator;
        friend struct ImporterCreator;
        friend struct YAML::ImporterDeserialier;
        friend bool YAML::DeserializeImporter(const YAML::Node& node, VWolfPup::AssetMetaFile& rhs);
    };
}

namespace YAML {
    template<>
    struct convert<VWolfPup::AssetMetaFile> {
        static bool decode(const Node& node, VWolfPup::AssetMetaFile& rhs)
        {
            bool deserializeGenericProperties = VWolf::DeserializeFromBoostDescribe(node, rhs);
            bool deserializeImporter = DeserializeImporter(node, rhs);
            return deserializeGenericProperties && deserializeImporter;
        }
    };
}
