#include <vector>
#include "Types/Type.hpp"

using TypeId = std::size_t;

class TypeTable {
    public:
        TypeId add(Type symbol) {
            TypeId id = types.size();

            symbol.id = id;

            types.push_back(std::move(symbol));

            return id;
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