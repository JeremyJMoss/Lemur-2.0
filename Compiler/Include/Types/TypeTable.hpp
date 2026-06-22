#include <vector>
#include "Types/Type.hpp"

using TypeId = std::size_t;

class TypeTable {
    public:
        TypeId add(Type type) {
            type.setId(types.size());

            types.push_back(std::move(type));

            return type.m_id;
        }

        Type& get(TypeId id) {
            return types[id];
        }

        const Type& get(TypeId id) const {
            return types[id];
        }
    private:
        std::vector<Type> types;
};