#include "inspircd.h"

class KickOnCensor : public SimpleChannelModeHandler
{
public:
	KickOnCensor(Module *Creator) : SimpleChannelModeHandler(Creator, "kick_on_censor", 'G') {}
};

class ModuleKickOnCensor : public Module
{
	KickOnCensor koc;
public:
	ModuleKickOnCensor() :
		koc(this)
	{}

	~ModuleKickOnCensor()
	{}

	void init()
	{
		ServerInstance->Modules->AddService(koc);
	}

	virtual Version GetVersion()
	{
		return Version("Provides channel mode +G to kick instead of block censored messages.", VF_VENDOR);
	}
};

MODULE_INIT(ModuleKickOnCensor)
