#pragma once

/* === Dependencies ===*/

#include <vector>
#include <string>

/* === Imports === */

#include "Core/Ids.hpp"
#include "ImportDirective.hpp"

/* === Module Header === */

struct ModuleHeader
{
    ModuleId id;
    FileId fileId;
    std::string name;
    std::vector<ImportDirective> imports;
};