/*
 * InspIRCd -- Internet Relay Chat Daemon
 *
 *   Copyright (C) 2009 Daniel De Graaf <danieldg@inspircd.org>
 *   Copyright (C) 2007 Robin Burchell <robin+git@viroteck.net>
 *
 * This file is part of InspIRCd.  InspIRCd is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* What's changed from upstream?
   I've added the option to start a program provided instead of argv[0]. ~ jcjordyn1(2/3)0
*/

#include "inspircd.h"

/** Handle /RESTART
 */
class CommandRestart : public Command
{
 public:
	/** Constructor for restart.
	 */
	CommandRestart(Module* parent) : Command(parent,"RESTART",1,2) { flags_needed = 'o'; syntax = "<password> [<path>]"; }
	/** Handle command.
	 * @param parameters The parameters to the comamnd
	 * @param pcnt The number of parameters passed to teh command
	 * @param user The user issuing the command
	 * @return A value from CmdResult to indicate command success or failure.
	 */
	CmdResult Handle(const std::vector<std::string>& parameters, User *user);
};

CmdResult CommandRestart::Handle (const std::vector<std::string>& parameters, User *user)
{
	ServerInstance->Logs->Log("COMMAND",DEFAULT,"Restart: %s",user->nick.c_str());
	if (!ServerInstance->PassCompare(user, ServerInstance->Config->restartpass, parameters[0].c_str(), ServerInstance->Config->powerhash))
	{
		if (parameters.size() > 1) {
			ServerInstance->SNO->WriteGlobalSno('a', "RESTART command from %s, restarting server. (Using binary: %s)", user->GetFullRealHost().c_str(), parameters[1].c_str());
		}
		else
		{
			ServerInstance->SNO->WriteGlobalSno('a', "RESTART command from %s, restarting server.", user->GetFullRealHost().c_str());
		}

		ServerInstance->SendError("Server restarting.");

#ifndef _WIN32
		/* XXX: This hack sets FD_CLOEXEC on all possible file descriptors, so they're closed if the execv() below succeeds.
		 * Certainly, this is not a nice way to do things and it's slow when the fd limit is high.
		 *
		 * A better solution would be to set the close-on-exec flag for each fd we create (or create them with O_CLOEXEC),
		 * however there is no guarantee that third party libs will do the same.
		 */
		for (int i = getdtablesize(); --i > 2;)
		{
			int flags = fcntl(i, F_GETFD);
			if (flags != -1)
				fcntl(i, F_SETFD, flags | FD_CLOEXEC);
		}
#endif


		if (parameters.size() > 1) {
			execv(parameters[1].c_str(), ServerInstance->Config->cmdline.argv);
			ServerInstance->SNO->WriteGlobalSno('a', "Failed RESTART - could not execute '%s' (%s)",
				ServerInstance->Config->cmdline.argv[0], strerror(errno));
		}
		else
		{
			execv(ServerInstance->Config->cmdline.argv[0], ServerInstance->Config->cmdline.argv);
			ServerInstance->SNO->WriteGlobalSno('a', "Failed RESTART - could not execute '%s' (%s)",
				ServerInstance->Config->cmdline.argv[0], strerror(errno));
		}
	}
	else
	{
		ServerInstance->SNO->WriteGlobalSno('a', "Failed RESTART Command from %s.", user->GetFullRealHost().c_str());
	}
	return CMD_FAILURE;
}


COMMAND_INIT(CommandRestart)
