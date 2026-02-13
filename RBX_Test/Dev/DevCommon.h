/**********************************************************************

	Nexcom Co., Ltd.
	Copyright 2021. All Rights Reserved.

	Filename		: DevCommon.h
	Version			: 00.00
	Description		: 
	Tracebility		: 
	Programmer	    :
	Last Updated	: 2026. 01. 21.

**********************************************************************/

/*
 * Modification History
 * --------------------
 * 
 * 
*/


#ifndef DEVCOMMON_H
#define DEVCOMMON_H

/* ************************** [[   include  ]]  *********************************************************** */
#include "main.h"


/* ************************** [[   define   ]]  *********************************************************** */



/* ************************** [[   enum or struct   ]]  *************************************************** */
typedef union
{
    Uint32 		u32;
	float32 	f32;

    struct
    {
	    Uint16 B0:8u;
	    Uint16 B1:8u;
	    Uint16 B2:8u;
	    Uint16 B3:8u;
    } byte;
}onConv32;


typedef union
{
    Uint16 u16;

    struct
    {
	    Uint16 B0:8u;
	    Uint16 B1:8u;
    } byte;

	struct
	{
		Uint16 b00:1u;
		Uint16 b01:1u;
		Uint16 b02:1u;
		Uint16 b03:1u;
		Uint16 b04:1u;
		Uint16 b05:1u;
		Uint16 b06:1u;
		Uint16 b07:1u;
		Uint16 b08:1u;
		Uint16 b09:1u;
		Uint16 b10:1u;
		Uint16 b11:1u;
		Uint16 b12:1u;
		Uint16 b13:1u;
		Uint16 b14:1u;
		Uint16 b15:1u;
	} bit;
}onConv16;


/* ************************** [[   global   ]]  *********************************************************** */


/* ************************** [[  function  ]]  *********************************************************** */



#endif	// #ifndef DEVOMMON_H



