#include "Define.h"
#include <vector>
#include <string>

// Includes list
@MODULES_DECLARATIONS@

#include "ModulesLoader.h"

/// Exposed in script modules to register all scripts to the ScriptMgr.
void AddModulesScripts()
{
    // Modules
    @MODULES_CALLS@
}
