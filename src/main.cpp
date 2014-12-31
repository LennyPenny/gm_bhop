#include "GarrysMod/Lua/Interface.h"
#include "sourcestuff.h"

#include "inetchannel.h"
#include "icliententity.h"
#include "icliententitylist.h"

#include "in_buttons.h"

#define CLIENT_DLL

#include "cbase.h"

#include "getInterface.h"
#include "SigScan.h"
#include "Hook/VMT.h"
#include "Hook/VTable.h"
#include "netvars/netvars.h"

using namespace GarrysMod::Lua;

IBaseClientDLL *baseClient;
IVEngineClient *client;
IClientEntityList *list;

CNetworkedVariableManager netVarMngr;

const int CREATEMOVE_OFFSET = 21;

Hook::VMT *clientModeSharedHooks;

int flagsOffset;

typedef bool ( __thiscall *hookedCreateMoveFn )(void *, float, CUserCmd *);
bool __fastcall hookedCreateMove(void *_this, void *, float inputSampleTime, CUserCmd *cmd) {

	int *lply = (int *) list->GetClientEntity(client->GetLocalPlayer())->GetBaseEntity(); 

	if (!flagsOffset)
		flagsOffset = netVarMngr.GetOffset("DT_BasePlayer", "m_fFlags");

	int flags = *(int *)((char *)(lply) + flagsOffset);

	if ( cmd->buttons & IN_JUMP && !(flags & FL_ONGROUND))
			cmd->buttons &= ~IN_JUMP;

	return clientModeSharedHooks->GetOriginal<hookedCreateMoveFn>(CREATEMOVE_OFFSET)( _this, inputSampleTime, cmd );
}
GMOD_MODULE_OPEN() {
	baseClient = GetInterface<IBaseClientDLL *>("client.dll", CLIENT_DLL_INTERFACE_VERSION);
	if (!baseClient)
		return 0;

	client = GetInterface<IVEngineClient *>("engine.dll", VENGINE_CLIENT_INTERFACE_VERSION);
	if (!client)
		return 0;

	list = GetInterface<IClientEntityList *>("client.dll", VCLIENTENTITYLIST_INTERFACE_VERSION);
	if (!list)
		return 0;


	void *clientModeShared = **( void *** ) SigScan("client.dll").Scan("\x8B\x0D????\x50\x8B\x01\x8B\x50\x24", 2);

	netVarMngr = CNetworkedVariableManager(baseClient->GetAllClasses());

	clientModeSharedHooks = new Hook::VMT(clientModeShared);
	clientModeSharedHooks->Hook(CREATEMOVE_OFFSET, hookedCreateMove);

	return 0;
}

GMOD_MODULE_CLOSE() {
	clientModeSharedHooks->Remove();
	return 0;
}