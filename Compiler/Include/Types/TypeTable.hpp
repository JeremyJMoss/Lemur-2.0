#include <vector>
#include "Types/Type.hpp"

using TypeId = std::size_t;

class TypeTable {
    public:
        TypeId add(Type type) {
            TypeId id = types.size();

            type.setId(id);

            types.push_back(std::move(type));

            return id;
        }

        const Type& get(TypeId id) const {
            return types[id];
        }
    private:
        std::vector<Type> types;
};