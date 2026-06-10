//
//  AssetMetaFile.cpp
//  VWolfPup
//
//  Created by Enrique Moises on 6/3/26.
//

#include "AssetMetaFile.h"
#include "VWolf.h"

#include <yaml-cpp/yaml.h>
#include <fstream>
#include <boost/type_index.hpp>
#include <boost/mpl/for_each.hpp>
#include <chrono>

#include <iostream>

static std::string META_FILE_KEY_NAME = "AssetMetaFile";

namespace VWolfPup {
    template<typename T>
    std::string GetClassName() {
        std::string typeName = boost::typeindex::type_id_with_cvr<T>().pretty_name();

    #ifdef VWOLF_PLATFORM_WINDOWS
        std::string toRemove = "class VWolfPup::";
    #elif defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
        std::string toRemove = "VWolfPup::";
    #endif

        size_t pos = typeName.find(toRemove);
        if (pos != std::string::npos) {
            typeName.erase(pos, toRemove.length());
        }
        return typeName;
    }
}

namespace YAML {
    struct ImporterDeserialier {
    public:
        ImporterDeserialier(const Node& node, VWolfPup::AssetMetaFile& rhs): node(node), rhs(rhs) {}
    public:
        template <typename type>
        void operator()(type) const {
            std::string name = VWolfPup::GetClassName<type>();
            if (node[name]) {
                rhs.importer = new type;
            }
        }
    private:
        const Node& node;
        VWolfPup::AssetMetaFile& rhs;
    };

    bool DeserializeImporter(const Node& node, VWolfPup::AssetMetaFile& rhs) {
        boost::mpl::for_each<AllImporters>(ImporterDeserialier(node, rhs));
        return true;
    }
}

namespace VWolfPup {
    struct ImporterSerialier {
    public:
        ImporterSerialier(YAML::Emitter& out, AssetMetaFile& v): out(out), v(v) {}
        ImporterSerialier(YAML::Emitter& out, const AssetMetaFile& v): out(out), v(v) {}
    public:
        template <typename T>
        void operator()(T) const {

            T* value = dynamic_cast<T*>(v.importer);
            
            if (value != nullptr) {
                out << YAML::Key << GetClassName<T>();
                out << YAML::BeginMap;
                VWolf::SerializeFromBoostOnlyMembers(out, *value);
                out << YAML::EndMap;
            }
                
        }
    private:
        YAML::Emitter& out;
        const AssetMetaFile& v;
    };

    struct ImporterExtensionValidator {
    public:
        ImporterExtensionValidator(std::filesystem::path path, bool& isValid): path(path), isValid(isValid) {}
    public:
        template <typename T>
        void operator()(T) const {
            T importer;
            
            isValid |= importer.CanImport(path);
        }
    private:
        bool& isValid;
        std::filesystem::path path;
    };

    struct ImporterCreator {
    public:
        ImporterCreator(std::filesystem::path path, AssetMetaFile& metaFileParent): path(path), metaFileParent(metaFileParent) {}
    public:
        template <typename T>
        void operator()(T) const {
            T importer;
            
            if (importer.CanImport(path)) {
                metaFileParent.importer = new T;
                VWOLF_CLIENT_INFO("CREATE metafile %s with %s.",
                                  metaFileParent. metafile.string().c_str(),
                                  GetClassName<T>().c_str());
            }
        }
    private:
        AssetMetaFile& metaFileParent;
        std::filesystem::path path;
    };

    std::string AssetMetaFile::META_FILE_EXTENSION = ".vpmeta";

    AssetMetaFile::AssetMetaFile(std::filesystem::path path) {
        this->SetPath(path);
        std::filesystem::file_time_type ftime = std::filesystem::last_write_time(path);
        auto duration = ftime.time_since_epoch();
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
        lastModifiedTime = seconds;
    }

    AssetMetaFile::AssetMetaFile(AssetMetaFile& asmf) {
        this->id = asmf.id;
        this->version = asmf.version;
        this->importer = asmf.importer;
        this->path = asmf.path;
        this->metafile = asmf.metafile;
        this->lastModifiedTime = asmf.lastModifiedTime;
    }

    AssetMetaFile::AssetMetaFile(const AssetMetaFile& asmf) {
        this->id = asmf.id;
        this->version = asmf.version;
        this->importer = asmf.importer;
        this->path = asmf.path;
        this->metafile = asmf.metafile;
        this->lastModifiedTime = asmf.lastModifiedTime;
    }

    AssetMetaFile::AssetMetaFile(AssetMetaFile&& asmf) {
        this->id = asmf.id;
        this->version = asmf.version;
        this->importer = asmf.importer;
        this->path = asmf.path;
        this->metafile = asmf.metafile;
        this->lastModifiedTime = asmf.lastModifiedTime;
        
        asmf.version = 0;
        asmf.id = VWolf::UUID::Empty;
        asmf.importer = nullptr;
        asmf.lastModifiedTime = 0;
    }

    AssetMetaFile::~AssetMetaFile() {
        this->version = 0;
        this->id = VWolf::UUID::Empty;
        this->importer = nullptr;
        this->lastModifiedTime = 0;
    }

    bool AssetMetaFile::IsMetafile(std::filesystem::path path) {
        if (std::filesystem::is_directory(path)) return false;
        return (path.extension().string() == AssetMetaFile::META_FILE_EXTENSION);
    }

    AssetMetaFile AssetMetaFile::Load(std::filesystem::path path) {
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(path.string());
        }
        catch (YAML::ParserException e)
        {
            VWOLF_CLIENT_ERROR("Failed to load .scene file '%s'\n     %s", path.string().c_str(), e.what());
            return AssetMetaFile(path);
        }
        
        if (!data[META_FILE_KEY_NAME])
            return AssetMetaFile(path);
        
        AssetMetaFile result = data[META_FILE_KEY_NAME].as<AssetMetaFile>();
        result.SetPath(path);
        
        return result;
    }

    AssetMetaFile& AssetMetaFile::operator=(const AssetMetaFile& other) {
        this->id = other.id;
        this->version = other.version;
        this->importer = other.importer;
        this->path = other.path;
        this->metafile = other.metafile;
        this->lastModifiedTime = other.lastModifiedTime;
        
        return *this;
    }

    AssetMetaFile& AssetMetaFile::operator=(AssetMetaFile&& other) {
        this->id = other.id;
        this->version = other.version;
        this->importer = other.importer;
        this->path = other.path;
        this->metafile = other.metafile;
        this->lastModifiedTime = other.lastModifiedTime;
        
        other.version = 0;
        other.id = VWolf::UUID::Empty;
        other.importer = nullptr;
        other.lastModifiedTime = 0;
        
        return *this;
    }

    bool AssetMetaFile::Create() {
        
        if (Exists()) return false;
        
        boost::mpl::for_each<AllImporters>(ImporterCreator(path, *this));
        
        bool canCreate = false;
        boost::mpl::for_each<AllImporters>(ImporterExtensionValidator(path, canCreate));
        
        if (canCreate) {
            YAML::Emitter out;
            out << *this;
            std::ofstream fout(metafile.string());
            fout << out.c_str();
        }
        
        return true;
    }

    bool AssetMetaFile::Import() {
        return importer->Import(path, this->id);
    }

    void AssetMetaFile::SetPath(std::filesystem::path path) {
        if (path.extension() == AssetMetaFile::META_FILE_EXTENSION) {
            metafile = path;
            this->path = path.parent_path() / path.stem();
        } else {
            this->path = path;
            metafile = path;
            metafile += AssetMetaFile::META_FILE_EXTENSION;
        }
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, AssetMetaFile& v) {
        out << YAML::BeginMap;
        out << YAML::Key << META_FILE_KEY_NAME;
        out << YAML::BeginMap;
        VWolf::SerializeFromBoostOnlyMembers(out, v);
        boost::mpl::for_each<AllImporters>(ImporterSerialier(out, v));
        out << YAML::EndMap;
        out << YAML::EndMap;
        return out;
    }
    
    YAML::Emitter& operator<<(YAML::Emitter& out, const AssetMetaFile& v) {
        out << YAML::BeginMap;
        out << YAML::Key << META_FILE_KEY_NAME;
        out << YAML::BeginMap;
        VWolf::SerializeFromBoostOnlyMembers(out, v);
        boost::mpl::for_each<AllImporters>(ImporterSerialier(out, v));
        out << YAML::EndMap;
        out << YAML::EndMap;
        return out;
    }
}
