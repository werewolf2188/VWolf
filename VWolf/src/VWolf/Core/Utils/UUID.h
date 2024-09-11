//
//  UUID.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 8/29/24.
//

#pragma once

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

namespace VWolf {
    struct UUID {
    private:
        UUID();
        UUID(const std::string& uuid);
#if defined(VWOLF_CORE)
        UUID(boost::uuids::uuid uuid);
#endif
    public:
        UUID(const UUID& uuidClass);
        UUID(UUID& uuidClass);
        UUID(UUID&& uuidClass);
        ~UUID();
    public:
        static UUID Empty;
    public:
        UUID& operator=(const UUID& other);
    public:
        bool operator==(const UUID& rhs);
        bool operator!=(const UUID& rhs);
    public:
        std::string ToString() const;
        bool IsEmpty() const;
        const uint8_t* GetData() const;
        std::array<uint8_t, 16> GetArray() const;
    public:
        static UUID NewUUID();
        static UUID Parse(const std::string& uuid);
        static bool TryParse(const std::string& uuidString, UUID& uuid);
    private:
        boost::uuids::uuid _uuid;
#if defined(DEBUG) || defined(VWOLF_CORE)
    public:
        inline boost::uuids::uuid& GetInternalUUID() { return _uuid; }
        inline const boost::uuids::uuid& GetInternalUUID() const { return _uuid; }
#endif
        friend std::ostream& operator<<(std::ostream& os, const UUID& v);
        friend bool operator==(const UUID& lhs, const UUID& rhs);
        friend bool operator!=(const UUID& lhs, const UUID& rhs);
    };


}
