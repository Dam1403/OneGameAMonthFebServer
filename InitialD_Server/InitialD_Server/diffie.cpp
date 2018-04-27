#include "stdafx.h"


bool initialized = false;

std::map<LPCTSTR, HCRYPTPROV> provider_map;



int diffie_send_gp(LPCTSTR exchange_id)
{
	HCRYPTPROV prov_handle;
	
	bool acq_result = CryptAcquireContext(
		&prov_handle,
		exchange_id,
		NULL,
		PROV_DH_SCHANNEL,
		0
	);

	if (!acq_result) { return 1; }
	provider_map.emplace(std::make_pair(exchange_id, prov_handle));
	HCRYPTKEY key;
	CryptGenKey(prov_handle, CALG_DH_EPHEM,CRYPT_PREGEN,&key);

}


