#include "Modules/ModuleManager.h"

class FLibreFPVEditorModule : public FDefaultGameModuleImpl {
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	void OnDuplicateActorsBegin();
	void OnEditPasteActorsBegin();
	void OnEditCopyActorsBegin();
};
