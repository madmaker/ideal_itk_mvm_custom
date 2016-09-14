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
 *   This file contains the implementation for the Extension Pm8_DocRelAutoDeletion
 *
 */
#include "m9_DocRel_autodeletion.hxx"
#include <base_utils/ResultCheck.hxx>
#include "../process_error.hxx"

int m9_DocRel_autodeletion(METHOD_message_t *msg, va_list args) {
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

		char* doc_rel_name = "M9_DocRel";
		tag_t doc_rel_type = NULL_TAG;
		tag_t user_data = NULL_TAG;
		tag_t doc_rel_tag = NULL_TAG;
		erc = TCTYPE_find_type(doc_rel_name, NULL, &doc_rel_type);
		erc = GRM_find_relation(secondary_object, primary_object_item, doc_rel_type,
				&doc_rel_tag);
		if (doc_rel_tag != NULL_TAG) {
			erc = GRM_delete_relation(doc_rel_tag);
		} else {
			printf("M9_DocRel TYPE NOT FOUND!!!");
		}
	} catch (...) {
		return sisw::process_error(true, true, false);
	}
	return ITK_ok;
}
