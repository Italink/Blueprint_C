#pragma once

#include "Modules/ModuleManager.h"

class FBlueprint_CModule : public IModuleInterface
{
protected:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
