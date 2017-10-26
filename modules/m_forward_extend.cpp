#include "inspircd.h"

class NoForward : public SimpleChannelModeHandler
{
public:
	NoForward(Module *Creator) : SimpleChannelModeHandler(Creator, "noforward", 'x') { }

	ModeAction OnModeChange(User *user, User *, Channel *channel, std::string &, bool adding)
	{
		if (channel != NULL)
		{
			if (adding)
			{
				if (channel->IsModeSet('x'))
					return MODEACTION_DENY;

				if (channel->IsModeSet('W'))
				{
					ConfigTag *tag = ServerInstance->Config->ConfValue("server");
					std::string servname = tag->getString("name");
					user->WriteServ("NOTICE %s :*** Mode +W and +x may not be set on a channel at the same time.", user->nick.c_str());
					channel->WriteChannelWithServ(servname, "MODE %s -W", channel->name.c_str());
					channel->SetMode('W', false);
					channel->SetMode('x', true);
				}
				else
				{
					channel->SetMode('x', true);
				}
			}
			else
			{
				if (channel->IsModeSet('x'))
					channel->SetMode('x', false);
				else
					return MODEACTION_DENY;
			}
		}
		return MODEACTION_ALLOW;
	}
};

class FreeForward : public SimpleChannelModeHandler
{
public:
	FreeForward(Module *Creator) : SimpleChannelModeHandler(Creator, "freeforward", 'W') { }

	ModeAction OnModeChange(User *user, User *, Channel *channel, std::string &, bool adding)
	{
		if (channel != NULL)
		{
			if (adding)
			{
				if (channel->IsModeSet('W'))
					return MODEACTION_DENY;

				if (channel->IsModeSet('x'))
				{
					ConfigTag *tag = ServerInstance->Config->ConfValue("server");
					std::string servname = tag->getString("name");
					user->WriteServ("NOTICE %s :*** Mode +x and +W may not be set on a channel at the same time.", user->nick.c_str());
					channel->WriteChannelWithServ(servname, "MODE %s -x", channel->name.c_str());
					channel->SetMode('x', false);
					channel->SetMode('W', true);
				}
				else
				{
					channel->SetMode('W', true);
				}
			}
			else
			{
				if (channel->IsModeSet('W'))
					channel->SetMode('W', false);
				else
					return MODEACTION_DENY;
			}
		}
		return MODEACTION_ALLOW;
	}
};

class ModuleForwardExtend : public Module
{
	NoForward nf;
	FreeForward ff;
public:
	ModuleForwardExtend() :
		nf(this),
		ff(this)
	{ }

	void init()
	{
		ServerInstance->Modules->AddService(nf);
		ServerInstance->Modules->AddService(ff);
	}

	~ModuleForwardExtend()
	{ }

	virtual Version GetVersion()
	{
		return Version("Provides channel modes to enhance channel forwarding features", VF_VENDOR);
	}
};

MODULE_INIT(ModuleForwardExtend)
