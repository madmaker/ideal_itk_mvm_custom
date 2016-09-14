//@<COPYRIGHT>@
//==================================================
//Copyright $2015.
//Siemens Product Lifecycle Management Software Inc.
//All Rights Reserved.
//==================================================
//@<COPYRIGHT>@

/* 
 * @file 
 *
 *   This file contains the implementation for the Extension Pm8_DocRelAutoCreation
 *
 */
#include "m9_DocRel_autocreation.hxx"
#include <base_utils/ResultCheck.hxx>
#include "../process_error.hxx"

int m9_DocRel_autocreation(METHOD_message_t *msg, va_list args) {
	try {
		ResultCheck erc;
		tag_t primary_object = NULL_TAG;
		tag_t secondary_object = NULL_TAG;
		tag_t primary_object_item = NULL_TAG;
		char* primary_name;
		char* secondary_name;

		erc = GRM_ask_primary(msg->object_tag, &primary_object);
		erc = GRM_ask_secondary(msg->object_tag, &secondary_object);
		erc = ITEM_ask_item_of_rev(primary_object, &primary_object_item);
		//AOM_ask_value_string(primary_object, "object_name", &primary_name);
		//AOM_ask_value_string(secondary_object, "object_name", &secondary_name);
		//std::cout << primary_name << std::endl;
		//std::cout << secondary_name << std::endl;

		/*char* company_part_rev_name = "Pm8_CompanyPart Revision";
		 char* commercial_part_rev_name = "CommercialPart Revision";
		 char* kd_rev_name = "KD Revision";
		 tag_t company_part_rev_type = NULL_TAG;
		 tag_t commercial_part_rev_type = NULL_TAG;
		 tag_t kd_rev_type = NULL_TAG;
		 TCTYPE_find_type(commercial_part_rev_name, NULL, &kd_rev_type);
		 TCTYPE_find_type(company_part_rev_name, NULL, &company_part_rev_type);
		 TCTYPE_find_type(commercial_part_rev_name, NULL, &commercial_part_rev_type);
		 TCTYPE_ask_object_type(company_part_rev_name, &company_part_rev_type);
		 TCTYPE_ask_object_type(commercial_part_rev_name, &commercial_part_rev_type);
		 tag_t primary_type = NULL_TAG;
		 tag_t secondary_type = NULL_TAG;
		 TCTYPE_ask_object_type(primary_object, &primary_type);
		 TCTYPE_ask_object_type(secondary_object, &secondary_type);
		 check if we get BO Item or it's descendant type
		 logical is_primary_type_kd = false;
		 logical is_secondary_type_comp = false;
		 logical is_secondary_type_comm = false;
		 TCTYPE_is_type_of(primary_type, relatedObjectType, &is_primary_type_kd);
		 TCTYPE_is_type_of(secondary_type, relatedObjectType, &is_secondary_type_comp);
		 TCTYPE_is_type_of(secondary_type, relatedObjectType, &is_secondary_type_comm);*/

		char* doc_rel_name = "M9_DocRel";
		tag_t doc_rel_type = NULL_TAG;
		tag_t user_data = NULL_TAG;
		tag_t doc_rel_tag = NULL_TAG;
		erc = TCTYPE_find_type(doc_rel_name, NULL, &doc_rel_type);
		if(doc_rel_type==NULL_TAG){
			printf("M9_DocRel TYPE NOT FOUND!!!");
			return ITK_ok;
		}
		erc = GRM_create_relation(secondary_object, primary_object_item, doc_rel_type,
				user_data, &doc_rel_tag);
		erc = GRM_save_relation(doc_rel_tag);
	} catch (...) {
		return sisw::process_error(true, true, false);
	}
	return ITK_ok;

}
