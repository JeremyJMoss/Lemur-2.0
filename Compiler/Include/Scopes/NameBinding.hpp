using SymbolId = std::size_t;
using OverloadSetId = std::size_t;

enum class NameBindingKind {
    Symbol,
    OverloadSet
};

struct NameBinding {
    NameBindingKind m_kind;

    SymbolId m_symbolId;
    OverloadSetId m_overloadSetId;
};