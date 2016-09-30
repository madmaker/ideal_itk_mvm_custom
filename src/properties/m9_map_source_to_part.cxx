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
#include "m9_map_source_to_part.hxx"

#define NUM_ENTRIES 1

/*
 * Для M9_CompanyPartRevision создано runtime-свойство m9_Source (string 256).
 * Нижеперечисленные условия работают при условии что M9_CompanyPartRevision.m9_TypeOfPart=Part
 *
 * Если M9_CompanyPartRevision.GRM(Item.M9_StockRel). object_string не пусто (в общем есть Item (а если точнее CommercialPart или M9_CompanyPart), привязанный отношением M9_StockRel) – то берем M9_CompanyPartRevision.GRM(Item.M9_StockRel). m9_Cod1C.
 * Если M9_CompanyPartRevision.GRM(Item.M9_StockRel). object_string непусто, при этом M9_CompanyPartRevision.GRM(Item.M9_StockRel). m9_Cod1C пусто, то M9_CompanyPartRevision.m9_Source=empty
 * Если M9_CompanyPartRevision.GRM(Item.M9_StockRel). object_string пусто, то ищем с помощью query IdealPlmSearchMat1Material (переменная MVM_Cod1C) Item (тип Item я сама укажу), у которого Item.object_name= M9_CompanyPartRevision.m9_CADMaterial
 * Если объект не найден, то  M9_CompanyPartRevision. m9_Source=empty
 * Если объект найден и Item.m9_Cod1C не пуст, то M9_CompanyPartRevision. m9_Source=Item.m9_Cod1C
 * Если объект найден и Item.m9_Cod1C пуст, то M9_CompanyPartRevision. m9_Source=empty
 */

int m9_map_source_to_part(METHOD_message_t *mess, va_list args) {

	char* partType;
	char* result;

	try {
		va_list vargs;
		va_copy(vargs, args);
		char** rvalue;
		tag_t prop_tag = va_arg(vargs, tag_t);
		rvalue = va_arg(vargs, char**);
		va_end(vargs);

		int index = 1;
		int entryCount;
		char** entryNames;
		char** entryValues;
		tag_t query_t = NULL_TAG;
		//tag_t stockRel_type_t = NULL_TAG;
		tag_t stockRel_t = NULL_TAG;
		int resultCount;

		ResultCheck erc;
		erc = erc = GRM_find_relation_type("M9_StockRel",
				 &stockRel_t);

		//std::cout << "--- Mapping source to a part" << std::endl;
		erc = AOM_ask_value_string(mess->object_tag, "m9_TypeOfPart", &partType);
		if(strcmp(partType, "Деталь")!=0){
			//std::cout << "Not applicable for this type of part" << std::endl;
			return ITK_ok;
		}

		int num_related = 0;
		auto_itk_mem_free<tag_t> stock_rels;
		erc = GRM_list_secondary_objects_only(mess->object_tag,
				stockRel_t, &num_related,
				stock_rels.operator tag_t **());

		if(num_related>0){
			//std::cout << "Found related part" << std::endl;
			tag_t related_t = stock_rels.operator tag_t *()[0];
			char* related_string;
			erc = AOM_ask_value_string(related_t, "m9_Cod1C", &related_string);
			if(strcmp(related_string,"")==0){
				//std::cout << "Empty 1CCode" << std::endl;
				result = "empty";
			} else {
				//std::cout << "Not empty 1CCode" << std::endl;
				result = (char*)MEM_alloc(strlen(related_string)+1);
				strcpy(result, related_string);
			}
		} else {
			//std::cout << "No related parts" << std::endl;
			char* material_s;
			erc = AOM_ask_value_string(mess->object_tag, "m9_CADMaterial", &material_s);

			char entryNamesArray[NUM_ENTRIES][QRY_uid_name_size_c+1]	= { "MVM_Cod1C" };
			entryNames = (char**)malloc( NUM_ENTRIES * sizeof *entryNames );
			for( index=0; index<NUM_ENTRIES; index++ ){
				entryNames[index] = (char*)malloc( strlen(entryNamesArray[index] + 1) );
				if( entryNames[index] )
					strcpy( entryNames[index], entryNamesArray[index] );
			}

			entryValues = (char**) MEM_alloc(NUM_ENTRIES * sizeof(char *));
			entryValues[0] = (char *)MEM_alloc( strlen( material_s ) + 1);
			strcpy(entryValues[0], material_s );

			auto_itk_mem_free<tag_t> results_found;
			erc = QRY_find("IdealPlmSearchMat1Material", &query_t);
			erc = QRY_execute(query_t, NUM_ENTRIES, entryNames, entryValues, &resultCount, results_found.operator tag_t **());
			if(resultCount>0){
				//std::cout << "QUERY found something" << std::endl;
				char* m9_Cod1C;
				erc = AOM_ask_value_string(results_found.operator tag_t *()[0], "m9_Cod1C", &m9_Cod1C);
				if(strcmp(m9_Cod1C,"")==0){
					//std::cout << "Empty 1CCode" << std::endl;
					result = "empty";
				} else {
					//std::cout << "Not empty 1CCode" << std::endl;
					strcpy(result, m9_Cod1C);
				}
			} else {
				//std::cout << "QUERY found nothing" << std::endl;
				result = "empty";
			}
		}

		*rvalue = (char*) MEM_alloc(strlen(result) + 1);
		strcpy(*rvalue, result);
		return ITK_ok;
	} catch (...) {
		return sisw::process_error(true, true, false);
	}
	return ITK_ok;
}
