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

CNetworkedVariableManager *netVarMngr;

SigScan clientScan = SigScan("client.dll");

int CREATEMOVE_OFFSET = 21;

Hook::VMT *ClientModeSharedHooks;

int m_fFlagsOffset;

typedef bool ( __thiscall *hCreateMoveFn )(void *, float, CUserCmd *);
bool __fastcall hCreateMove(void *_this, void *, float inputSampleTime, CUserCmd *cmd) {

	int *lply = (int *) list->GetClientEntity(client->GetLocalPlayer())->GetBaseEntity(); 

	if (!m_fFlagsOffset)
		m_fFlagsOffset = netVarMngr->GetOffset("DT_BasePlayer", "m_fFlags");

	int flags = *(int *)((char *)(lply) + m_fFlagsOffset);

	if ( cmd->buttons & IN_JUMP && !(flags & FL_ONGROUND))
			cmd->buttons &= ~IN_JUMP;

	return ClientModeSharedHooks->GetOriginal<hCreateMoveFn>(CREATEMOVE_OFFSET)( _this, inputSampleTime, cmd );
}
GMOD_MODULE_OPEN() {
	baseClient = GetInterface<IBaseClientDLL *>("client.dll", CLIENT_DLL_INTERFACE_VERSION);
	if ( !baseClient )
		Warning("Couldn't get client.dll interface");

	client = GetInterface<IVEngineClient *>("engine.dll", VENGINE_CLIENT_INTERFACE_VERSION);
	if ( !client )
		Warning("Couldn't get EngineClient interface");

	list = GetInterface<IClientEntityList *>("client.dll", VCLIENTENTITYLIST_INTERFACE_VERSION);
	if ( !list )
		Warning("Couldn't get EntityList interface");


	void *g_ClientModeShared = **( void *** ) clientScan.Scan("\x8B\x0D????\x50\x8B\x01\x8B\x50\x24", 2);

	netVarMngr = new CNetworkedVariableManager(baseClient->GetAllClasses());

	ClientModeSharedHooks = new Hook::VMT(g_ClientModeShared);
	ClientModeSharedHooks->Hook(CREATEMOVE_OFFSET, hCreateMove);

	return 0;
}

GMOD_MODULE_CLOSE() {
	return 0;
}