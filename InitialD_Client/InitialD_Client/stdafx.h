// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <stdlib.h>  
#include <tchar.h>
#include <thread>
#include <ctime>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <iphlpapi.h>

// Link with Iphlpapi.lib
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib,"ws2_32.lib") //Winsock Library



// TODO: reference additional headers your program requires here
#include "..\..\InitialD_Server\InitialD_Server\data_types.h"
