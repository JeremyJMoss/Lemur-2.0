#pragma once

/* === Dependencies ===*/

#include <vector>
#include <string>
#include <optional>

/* === Imports === */

#include "Core/Ids.hpp"

/* === Import Directive === */

struct ImportDirective
{
    const std::string moduleName;
    std::optional<ModuleId> moduleId;
};

/* === Module Header === */

struct ModuleHeader
{
    ModuleId id;
    FileId fileId;
    const std::string name;
    std::vector<ImportDirective> imports;
};