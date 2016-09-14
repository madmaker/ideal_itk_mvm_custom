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
 *   This file contains the declaration for the Extension Pm8_DocRelAutoCreation
 *
 */
 
#ifndef M9_DOCREL_AUTOCREATION_HXX
#define M9_DOCREL_AUTOCREATION_HXX
#include <tccore/method.h>
#include <aom.h>
#include <item.h>
#include <tccore/aom_prop.h>
#include <tccore/grm.h>
#include <tccore/tctype.h>
#include <iostream>
#ifdef __cplusplus
         extern "C"{
#endif
                 
int m9_DocRel_autocreation(METHOD_message_t* msg, va_list args);
                 
#ifdef __cplusplus
                   }
#endif
                
#endif  // M9_DOCREL_AUTOCREATION_HXX
