#include <iostream>
#include <string>
#include <base_utils/ResultCheck.hxx>
#include <tc/tc.h>
#include <property/prop_msg.h>
#include <tccore/custom.h>
#include <tccore/method.h>
#include "../process_error.hxx"
#include "m9_properties.hxx"
#include "m9_map_kit_to_bomline.hxx"
#include "m9_map_source_to_part.hxx"
#include "m9_get_document_type_value.hxx"
#include "m9_get_mat_string_nx_value.hxx"
#include "m9_get_TypeOfPart_EN.hxx"

/**
 *
 */
int m9_idealplm_custom_register_properties(int * decision, va_list args) {
	try {
		printf("\n");
		printf("REGISTRERING PROPERTIES...");
		printf("\n");

		ResultCheck erc;

		*decision = ALL_CUSTOMIZATIONS;
		METHOD_id_t meth = { 0 };

		erc = METHOD_register_prop_method("BOMLine", "m9_KITName",
				PROP_ask_value_string_msg, m9_map_kit_to_bomline, NULL, &meth);

		erc = METHOD_register_prop_method("M9_CompanyPartRevision", "m9_Source",
				PROP_ask_value_string_msg, m9_map_source_to_part, NULL, &meth);

		erc = METHOD_register_prop_method("M9_CompanyPartRevision", "m9_MatStringNX",
				PROP_ask_value_string_msg, m9_get_mat_string_nx_value, NULL, &meth);

		erc = METHOD_register_prop_method("M9_KD", "m9_DocumentType",
				PROP_ask_value_string_msg, m9_get_document_type_value, NULL, &meth);

		erc = METHOD_register_prop_method("Part", "m9_TypeOfPart_EN",
				PROP_ask_value_string_msg, m9_get_TypeOfPart_en, NULL, &meth);

	} catch (...) {
		return sisw::process_error(true, true, false);
	}
	return ITK_ok;
}
