#include <iostream>
#include <string>
#include <stdarg.h>

#include <base_utils/ResultCheck.hxx>
#include <tc/tc.h>
#include <tccore/grm.h>
#include <tccore/method.h>
#include <tccore/tctype.h>
#include <tccore/aom_prop.h>
#include <sa/sa.h>
#include <tccore/aom.h>
#include <ug_va_copy.h>

#include "../misc.hxx"
#include "../process_error.hxx"
#include "m9_map_kit_to_bomline.hxx"

#define NUM_ENTRIES 1

int m9_map_kit_to_bomline(METHOD_message_t *mess, va_list args) {

	tag_t kit_t;
	char* kit_id;
	char* kit_name = "";

	try {
		va_list vargs;
		va_copy(vargs, args);
		char** rvalue;
		tag_t prop_tag = va_arg(vargs, tag_t);
		rvalue = va_arg(vargs, char**);
		va_end(vargs);

		ResultCheck erc;
		int index = 1;
		int entryCount;
		char** entryNames;
		char** entryValues;
		tag_t query_t = NULL_TAG;
		int resultCount;

		//ResultCheck erc = AOM_ask_value_string(mess->object_tag, "M9_KITs", &kit_id);
		erc = AOM_ask_value_string(mess->object_tag, "M9_KITs", &kit_id);
		if(strcmp(kit_id, "")==0){
			//std::cout << "Empty KIT id" << std::endl;
			return ITK_ok;
		}
		//std::cout << "Searching for " << kit_id << std::endl;
		char entryNamesArray[NUM_ENTRIES][QRY_uid_name_size_c+1]	= { "MVM_Cod1C" };

		entryNames = (char**)malloc( NUM_ENTRIES * sizeof *entryNames );
		for( index=0; index<NUM_ENTRIES; index++ ){
			entryNames[index] = (char*)malloc( strlen(entryNamesArray[index] + 1) );
			if( entryNames[index] )
				strcpy( entryNames[index], entryNamesArray[index] );
		}

		entryValues = (char**) MEM_alloc(NUM_ENTRIES * sizeof(char *));
		entryValues[0] = (char *)MEM_alloc( strlen( kit_id ) + 1);
		strcpy(entryValues[0], kit_id );
		MEM_free(kit_id);

		auto_itk_mem_free<tag_t> results_found;
		erc = QRY_find("IdealPlmSearchKIT", &query_t);
		if(query_t==NULL_TAG){
			std::cout << "Query IdealPlmSearchKIT not found!" << std::endl;
			return ITK_ok;
		}
		erc = QRY_execute(query_t, NUM_ENTRIES, entryNames, entryValues, &resultCount, results_found.operator tag_t **());

		if(resultCount==0){
			//std::cout << "WARNING: Found nothing!" << std::endl;
			return ITK_ok;
		}

		// Берём первый попавшийся объект
		tag_t found_kit = results_found.operator tag_t *()[0];
		erc = AOM_ask_value_string(found_kit, "object_name", &kit_name);
		//std::cout << "Found: " << kit_name << std::endl;

		*rvalue = (char*) MEM_alloc(strlen(kit_name) + 1);
		strcpy(*rvalue, kit_name);
		MEM_free(kit_name);
		return ITK_ok;
	} catch (...) {
		return sisw::process_error(true, true, false);
	}
	return ITK_ok;
}
