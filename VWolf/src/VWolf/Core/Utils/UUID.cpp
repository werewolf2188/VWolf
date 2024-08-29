//
//  UUID.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 8/29/24.
//

#include "vwpch.h"
#include "UUID.h"

namespace VWolf {
    // MARK: Constants
    UUID UUID::Empty = UUID(boost::uuids::nil_generator()());

    // MARK: Constructors
    UUID::UUID(): _uuid(boost::uuids::random_generator()()) {}

    UUID::UUID(const std::string& uuid): _uuid(boost::uuids::string_generator()(uuid)) {}

    #if defined(VWOLF_CORE)
    UUID::UUID(boost::uuids::uuid uuid): _uuid(uuid) {}
    #endif

    UUID::UUID(const UUID& uuidClass): _uuid(uuidClass._uuid) {}

    UUID::UUID(UUID& uuidClass): _uuid(uuidClass._uuid) {}

    UUID::UUID(UUID&& uuidClass): _uuid(uuidClass._uuid) {
        uuidClass._uuid = boost::uuids::nil_generator()();
    }

    UUID::~UUID() {}

    // MARK: Assignment operators
    UUID& UUID::operator=(const UUID& other) {
        this->_uuid = other._uuid;

        return *this;
    }

    // MARK: Operator overloading
    std::ostream& operator<<(std::ostream& os, const UUID& v) {
        os << "UUID(" << std::addressof(v) << ") - {" << v.ToString() << "}";
        return os;
    }

    bool UUID::operator==(const UUID& rhs) {
        return this->_uuid == rhs._uuid;
    }

    bool operator==(const UUID& lhs, const UUID& rhs) {
        return lhs.GetInternalUUID() == rhs.GetInternalUUID();
    }

    bool UUID::operator!=(const UUID& rhs) {
        return this->_uuid != rhs._uuid;
    }

    bool operator!=(const UUID& lhs, const UUID& rhs) {
        return lhs.GetInternalUUID() != rhs.GetInternalUUID();
    }

    // MARK: Instance Methods
    std::string UUID::ToString() const {
        std::stringstream stream;
        stream << _uuid;
        return stream.str();
    }

    bool UUID::IsEmpty() const {
        return _uuid.is_nil();
    }

    const uint8_t* UUID::GetData() const {
        return _uuid.data;
    }

    std::array<uint8_t, 16> UUID::GetArray() const {
        std::array<uint8_t, 16> arra;
        std::memcpy(arra.data(), _uuid.data, sizeof(uint8_t) * 16);
        return arra;
    }

    // MARK: Static functions
    UUID UUID::NewUUID() {
        return UUID();
    }

    UUID UUID::Parse(const std::string& uuid) {
        return UUID(uuid);
    }

    bool UUID::TryParse(const std::string& uuidString, UUID& uuid) {
        try {
            uuid = UUID(uuidString);
            return true;
        } catch(std::exception & ex) {
//            VWOLF_CORE_ERROR("UUID parse failed: %s", ex.what());
            return false;
        }
    }
}
