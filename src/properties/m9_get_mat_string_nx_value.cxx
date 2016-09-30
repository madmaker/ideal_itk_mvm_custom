/*
 * pm8_get_source_value.cxx
 *
 *  Created on: Apr 29, 2015
 *      Author: infodba
 */
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <iterator>

#include <base_utils/ResultCheck.hxx>
#include <tc/tc.h>
#include <tccore/grm.h>
#include <tccore/method.h>
#include <tccore/tctype.h>
#include <tccore/aom_prop.h>
#include <tccore/item.h>
#include <tc/preferences.h>
#include <sa/sa.h>
#include <tccore/aom.h>
#include <ug_va_copy.h>

#include "../misc.hxx"
#include "../process_error.hxx"
#include "m9_get_mat_string_nx_value.hxx"

void removeMultipleSpaces(char**);
char* parse(char*);
bool debug2 = false;

/*
 * Если в качестве сырья (при помощи отношения M9_StockRel) для M9_CompanyPartRevision указан CommercialPart,
 * тогда M9_CompanyPartRevision.m9_MatStringNX = <TИзделие-заготовка!CommercialPart Revision.object_name>
 *
 * Если в качестве сырья (при помощи отношения M9_StockRel) для M9_CompanyPartRevision указан M9_CompanyPart,
 * тогда M9_CompanyPartRevision.m9_MatStringNX = <TИзделие-заготовка!M9_CompanyPartRevision.item_id>
 *
 * Если в качестве сырья (при помощи отношения M9_StockRel) для M9_CompanyPartRevision ничего не указано, тогда
 * если M9_CompanyPartRevision.m9_CADMaterial содержит в начале строки значение из преф. M9_SortamentTypes, тогда m9_CADMaterial нужно разбить на One, Two и Three, где One – значение из преф. плюс пробел, Two - значение из строки между One и “/”, Three - значение из строки после “/”; затем присвоить M9_CompanyPartRevision.m9_MatStringNX = One <VTwo!Three>
 * если M9_CompanyPartRevision.m9_CADMaterial не содержит в начале строки значение из преф. M9_SortamentTypes, тогда присвоить M9_CompanyPartRevision.m9_MatStringNX = m9_CADMaterial
 */

int m9_get_mat_string_nx_value(METHOD_message_t *mess, va_list args) {
	try {
		if(debug2) puts("+++ INTO m9_get_mat_string_nx_value");
		va_list vargs;
		va_copy(vargs, args);
		ResultCheck erc;
		char **rvalue;
		char* object_name;
		char* typeOfPart;
		tag_t tProp = va_arg(vargs, tag_t);
		rvalue = va_arg(vargs, char**);
		va_end(vargs);

		tag_t M9_StockRel_rel_t = NULL_TAG;
		tag_t M9_CompanyPartRev_type_t = NULL_TAG;
		tag_t CommercialPartRev_type_t = NULL_TAG;

		// Данная программа должна отрабатывать только для M9_CompanyPart с m9_TypeOfPart=Деталь
		tag_t item_t;
		erc = ITEM_ask_item_of_rev(mess->object_tag, &item_t);
		erc = AOM_ask_value_string(item_t, "m9_TypeOfPart",
				&typeOfPart);
		if(strcmp("Деталь", typeOfPart)){
			if(debug2) puts("--- Sorry, but I work with Part only");
			MEM_free(typeOfPart);
			return ITK_ok;
		}

		// Получаем таг_т для всех используемых в программе отношений
		erc = GRM_find_relation_type("M9_StockRel", &M9_StockRel_rel_t);

		// Получаем таг_т для всех используемых в программе типов объектов
		erc = TCTYPE_find_type("M9_CompanyPart", NULL,
				&M9_CompanyPartRev_type_t);
		erc = TCTYPE_find_type("CommercialPart", NULL,
				&CommercialPartRev_type_t);

		// Получаем связанный отношением M9_StockRel объект
		if(debug2) puts("Trying to find related by M9_StockRel");
		tag_t ref_source = NULLTAG;
		int num_rels = 0;
		auto_itk_mem_free<tag_t> source_rels;
		erc = GRM_list_secondary_objects_only(mess->object_tag,
				M9_StockRel_rel_t, &num_rels,
				source_rels.operator tag_t **());

		if (num_rels > 0) {
			if(debug2) puts("Found something");
			ref_source = source_rels.operator tag_t *()[0];
			if (ref_source != NULLTAG) {
				tag_t child_type_tag = NULL_TAG;
				erc = TCTYPE_ask_object_type(ref_source, &child_type_tag);
				//check if we get BO Item or it's descendant type

				logical is_company_part_type = false;
				logical is_commercial_part_type = false;
				erc = TCTYPE_is_type_of(child_type_tag,
						M9_CompanyPartRev_type_t, &is_company_part_type);
				erc = TCTYPE_is_type_of(child_type_tag,
						CommercialPartRev_type_t, &is_commercial_part_type);

				if (is_commercial_part_type) {
					if(debug2) puts("type: Commercial Part");
					erc = AOM_ask_value_string(ref_source, "object_name",
							&object_name);
					*rvalue = (char*) MEM_alloc(
							strlen(object_name)
									+ strlen("<TИзделие-заготовка!>") + 1);
					strcpy(*rvalue, "<TИзделие-заготовка!");
					strcat(*rvalue, object_name);
					strcat(*rvalue, ">");
				} else if (is_company_part_type) {
					if(debug2) puts("type: Company Part");
					erc = AOM_ask_value_string(ref_source, "item_id",
							&object_name);
					*rvalue = (char*) MEM_alloc(
							strlen(object_name)
									+ strlen("<TИзделие-заготовка!>") + 1);
					strcpy(*rvalue, "<TИзделие-заготовка!");
					strcat(*rvalue, object_name);
					strcat(*rvalue, ">");
				}
			}
		} else {
			if(debug2) puts("Did not found anything");
			erc = AOM_ask_value_string(mess->object_tag, "m9_CADMaterial",
					&object_name);
			if(object_name!=NULL){
				removeMultipleSpaces(&object_name);
				char* result = parse(object_name);
				*rvalue = (char*) MEM_alloc(strlen(result) + 1);
				strcpy(*rvalue, result);
				MEM_free(result);
			}
		}
		if(object_name) MEM_free(object_name);
		if(typeOfPart) MEM_free(typeOfPart);
		if(debug2) puts("--- ENDOF m9_get_mat_string_nx_value");
	} catch (...) {
		return sisw::process_error(true, true, false);
	}
	return ITK_ok;
}

void removeMultipleSpaces(char** input) {
	int i, j;
	int count = 1;
	j = 0;
	char* new_s = (char*) MEM_alloc(strlen(*input) + 1);
	for (i = 0; i < strlen(*input); i++) {
		if ((*input)[i] == ' ') {
			new_s[j] = (*input)[i];
			j++;
			while ((*input)[i + count] == ' ') {
				count++;
			}
			i += count - 1;
			count = 1;
		} else {
			new_s[j] = (*input)[i];
			j++;
		}
	}
	new_s[j] = '\0';
	*input = (char*) MEM_alloc(strlen(new_s)+1);
	strcpy(*input, new_s);
}

char* parse(char* input) {
	int i;
	char* result;

	size_t currentMaxLength = 0;
	int preferencePos = -1;
	int currentLength = 0;
	int firstSpacePos = 0;
	int similarity = 0;

	int n_values = 0;
	char **preference = NULL;
	PREF_initialize();
	PREF_ask_char_values("M9_SortamentTypes", &n_values, &preference);//TODO

	for (i = 0; i < n_values; i++) {
		if(debug2) puts(preference[i]);
		int prefLength = strlen(preference[i]);
		int contains = ( !strncmp(input, preference[i], prefLength) && input[strlen(preference[i])]==' ' );
		if (currentMaxLength > prefLength) {
			break;
		}
		if ((currentMaxLength < prefLength) && contains) {
			if(debug2) printf("Current max pref: %s", preference[i]);
			currentMaxLength = prefLength;
			preferencePos = i;
		}
	}

	int hasDelimiter = (strchr(input, '/')!=NULL)?1:0;
	if (preferencePos != -1 && hasDelimiter) {
		char* temp = (char*) MEM_alloc(strlen(input)+1);
		strcpy(temp, input);
		result = (char*) MEM_alloc(strlen(input)+4+1); //reserved 4 chars for "<V", ">", " "

		strcpy(result, preference[preferencePos]);
		strcat(result, " <V");

		char* secondPart = strtok((temp+strlen(preference[preferencePos])+1),"/");
		strcat(result, secondPart);
		strcat(result, "!");
		strcat(result, (temp + strlen(preference[preferencePos]) + strlen(secondPart) + 2)); //incremented by 2 to skip a space and a slash

		strcat(result, ">");
		MEM_free(temp);
	} else {
		result = (char*) MEM_alloc(strlen(input)+1);
		strcpy(result, input);
	}
	MEM_free(preference);
	preference = NULL;
	return result;
}
