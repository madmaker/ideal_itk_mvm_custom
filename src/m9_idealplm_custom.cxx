#include <iostream>
#include <string>
#include <base_utils/ResultCheck.hxx>
#include <tc/tc.h>
#include <tccore/custom.h>
#include "process_error.hxx"
#include "m9_idealplm_custom.hxx"
#include "properties/m9_properties.hxx"
#include "actions/m9_actions.hxx"

//cppcheck-suppress unusedFunction
extern "C" __declspec (dllexport) int m9_idealplm_custom_register_callbacks() {
	try {
		ResultCheck erc;
		erc = CUSTOM_register_exit("m9_idealplm_custom", "USER_register_properties",
				m9_idealplm_custom_register_properties);

		erc = CUSTOM_register_exit("m9_idealplm_custom", "USER_init_module",
				(CUSTOM_EXIT_ftn_t) m9_idealplm_custom_register_actions);

	} catch (...) {
		return sisw::process_error(true, true, false);
	}
	return ITK_ok;
}
