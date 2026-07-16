#pragma once

/* === Depedencies === */

#include <string>
#include <optional>

/* === Imports === */

#include "Core/Ids.hpp"

/* === Import Directive === */

struct ImportDirective
{
    std::string moduleName;
    std::optional<ModuleId> moduleId;
};