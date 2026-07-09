#pragma once

/* === Imports === */

#include <vector>

using ModuleId = std::size_t;
using FileId = std::size_t;

/* === Module Header === */

struct ModuleHeader
{
    std::string moduleName;
    std::vector<std::string> imports;
};

/* === Module Info === */

struct ModuleInfo
{
    ModuleId id;
    FileId fileId;
    std::string name;
    std::vector<ModuleId> imports;
};