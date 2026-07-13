#pragma once

/* === Imports === */

#include <vector>

using ModuleId = std::size_t;
using FileId = std::size_t;

/* === Import Directive === */

struct ImportDirective
{
    std::string moduleName;
    std::optional<ModuleId> moduleId;
};

/* === Module Info === */

struct ModuleHeader
{
    ModuleId id;
    FileId fileId;
    std::string name;
    std::vector<ImportDirective> imports;
};