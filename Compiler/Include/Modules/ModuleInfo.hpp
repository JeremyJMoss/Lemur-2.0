#pragma once

/* === Imports === */

#include "Modules/ModuleHeader.hpp"
#include "Modules/ExportTable.hpp"

/* === Module Info === */

struct ModuleInfo {
    ModuleHeader header;
    ExportTable exports;
};