/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * The contents of this file are subject to the Netscape Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is Mozilla Communicator client code.
 *
 * The Initial Developer of the Original Code is Netscape Communications
 * Corporation.  Portions created by Netscape are
 * Copyright (C) 1998 Netscape Communications Corporation. All
 * Rights Reserved.
 */

#include "nsSBCharSetProber.h"


/****************************************************************
255: Control characters that usually does not exist in any text
254: Carriage/Return
253: symbol (punctuation) that does not belong to word
252: 0 - 9

*****************************************************************/

//The following result for thai was collected from a limited sample (1M). 

//Character Mapping Table:
unsigned char TIS620CharToOrderMap[] =
{
255,255,255,255,255,255,255,255,255,255,254,255,255,254,255,255,  //00
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  //10
+253,253,253,253,253,253,253,253,253,253,253,253,253,253,253,253,  //20
252,252,252,252,252,252,252,252,252,252,253,253,253,253,253,253,  //30
253,182,106,107,100,183,184,185,101, 94,186,187,108,109,110,111,  //40
188,189,190, 89, 95,112,113,191,192,193,194,253,253,253,253,253,  //50
253, 64, 72, 73,114, 74,115,116,102, 81,201,117, 90,103, 78, 82,  //60
 96,202, 91, 79, 84,104,105, 97, 98, 92,203,253,253,253,253,253,  //70
209,210,211,212,213, 88,214,215,216,217,218,219,220,118,221,222,
223,224, 99, 85, 83,225,226,227,228,229,230,231,232,233,234,235,
236,  5, 30,237, 24,238, 75,  8, 26, 52, 34, 51,119, 47, 58, 57,
 49, 53, 55, 43, 20, 19, 44, 14, 48,  3, 17, 25, 39, 62, 31, 54,
 45,  9, 16,  2, 61, 15,239, 12, 42, 46, 18, 21, 76,  4, 66, 63,
 22, 10,  1, 36, 23, 13, 40, 27, 32, 35, 86,240,241,242,243,244,
 11, 28, 41, 29, 33,245, 50, 37,  6,  7, 67, 77, 38, 93,246,247,
 68, 56, 59, 65, 69, 60, 70, 80, 71, 87,248,249,250,251,252,253,
};




//Model Table: 
//total sequences: 100%
//first 512 sequences: 92.6386%
//first 1024 sequences:7.3177%
//rest  sequences:     1.0230%
//negative sequences:  0.0436% 
char ThaiLangModel[] = 
{
0,1,3,3,3,3,0,0,3,3,0,3,3,0,3,3,3,3,3,3,3,3,0,0,3,3,3,0,3,3,3,3,
0,3,3,0,0,0,1,3,0,3,3,2,3,3,0,1,2,3,3,3,3,0,2,0,2,0,0,3,2,1,2,2,
3,0,3,3,2,3,0,0,3,3,0,3,3,0,3,3,3,3,3,3,3,3,3,0,3,2,3,0,2,2,2,3,
0,2,3,0,0,0,0,1,0,1,2,3,1,1,3,2,2,0,1,1,0,0,1,0,0,0,0,0,0,0,1,1,
3,3,3,2,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,3,3,2,3,2,3,3,2,2,2,
3,1,2,3,0,3,3,2,2,1,2,3,3,1,2,0,1,3,0,1,0,0,1,0,0,0,0,0,0,0,1,1,
3,3,2,2,3,3,3,3,1,2,3,3,3,3,3,2,2,2,2,3,3,2,2,3,3,2,2,3,2,3,2,2,
3,3,1,2,3,1,2,2,3,3,1,0,2,1,0,0,3,1,2,1,0,0,1,0,0,0,0,0,0,1,0,1,
3,3,3,3,3,3,2,2,3,3,3,3,2,3,2,2,3,3,2,2,3,2,2,2,2,1,1,3,1,2,1,1,
3,2,1,0,2,1,0,1,0,1,1,0,1,1,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,
3,3,3,2,3,2,3,3,2,2,3,2,3,3,2,3,1,1,2,3,2,2,2,3,2,2,2,2,2,1,2,1,
2,2,1,1,3,3,2,1,0,1,2,2,0,1,3,0,0,0,1,1,0,0,0,0,0,2,3,0,0,2,1,1,
3,3,2,3,3,2,0,0,3,3,0,3,3,0,2,2,3,1,2,2,1,1,1,0,2,2,2,0,2,2,1,1,
0,2,1,0,2,0,0,2,0,1,0,0,1,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,0,
3,3,2,3,3,2,0,0,3,3,0,2,3,0,2,1,2,2,2,2,1,2,0,0,2,2,2,0,2,2,1,1,
0,2,1,0,2,0,0,2,0,1,1,0,1,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,
3,3,2,3,2,3,2,0,2,2,1,3,2,1,3,2,1,2,3,2,2,3,0,2,3,2,2,1,2,2,2,2,
1,2,2,0,0,0,0,2,0,1,2,0,1,1,1,0,1,0,3,1,1,0,0,0,0,0,0,0,0,0,1,0,
3,3,2,3,3,2,3,2,2,2,3,2,2,3,2,2,1,2,3,2,2,3,1,3,2,2,2,3,2,2,2,3,
3,2,1,3,0,1,1,1,0,2,1,1,1,1,1,0,1,0,1,1,0,0,0,0,0,0,0,0,0,2,0,0,
1,0,0,3,0,3,3,3,3,3,0,0,3,0,2,2,3,3,3,3,3,0,0,0,1,1,3,0,0,0,0,2,
0,0,1,0,0,0,0,0,0,0,2,3,0,0,0,3,0,2,0,0,0,0,0,3,0,0,0,0,0,0,0,0,
2,0,3,3,3,3,0,0,2,3,0,0,3,0,3,3,2,3,3,3,3,3,0,0,3,3,3,0,0,0,3,3,
0,0,3,0,0,0,0,2,0,0,2,1,1,3,0,0,1,0,0,2,3,0,1,0,0,0,0,0,0,0,1,0,
3,3,3,3,2,3,3,3,3,3,3,3,1,2,1,3,3,2,2,1,2,2,2,3,1,1,2,0,2,1,2,1,
2,2,1,0,0,0,1,1,0,1,0,1,1,0,0,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0,
3,0,2,1,2,3,3,3,0,2,0,2,2,0,2,1,3,2,2,1,2,1,0,0,2,2,1,0,2,1,2,2,
0,1,1,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
3,3,3,3,2,1,3,3,1,1,3,0,2,3,1,1,3,2,1,1,2,0,2,2,3,2,1,1,1,1,1,2,
3,0,0,1,3,1,2,1,2,0,3,0,0,0,1,0,3,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,
3,3,1,1,3,2,3,3,3,1,3,2,1,3,2,1,3,2,2,2,2,1,3,3,1,2,1,3,1,2,3,0,
2,1,1,3,2,2,2,1,2,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,
3,3,2,3,2,3,3,2,3,2,3,2,3,3,2,1,0,3,2,2,2,1,2,2,2,1,2,2,1,2,1,1,
2,2,2,3,0,1,3,1,1,1,1,0,1,1,0,2,1,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,
3,3,3,3,2,3,2,2,1,1,3,2,3,2,3,2,0,3,2,2,1,2,0,2,2,2,1,2,2,2,2,1,
3,2,1,2,2,1,0,2,0,1,0,0,1,1,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,1,
3,3,3,3,3,2,3,1,2,3,3,2,2,3,0,1,1,2,0,3,3,2,2,3,0,1,1,3,0,0,0,0,
3,1,0,3,3,0,2,0,2,1,0,0,3,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
3,3,3,2,3,2,3,3,0,1,3,1,1,2,1,2,1,1,3,1,1,0,2,3,1,1,1,1,1,1,1,1,
3,1,1,2,2,2,2,1,1,1,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
3,2,2,1,1,2,1,3,3,2,3,2,2,3,2,2,3,1,2,2,1,2,0,3,2,1,2,2,2,2,2,1,
3,2,1,2,2,2,1,1,1,1,0,0,1,1,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,
3,3,3,3,3,3,3,3,1,3,3,0,2,1,0,3,2,0,0,3,1,0,1,1,0,1,0,0,0,0,0,1,
1,0,0,1,0,3,2,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
3,0,2,2,2,3,0,0,1,3,0,3,2,0,3,2,2,3,3,3,3,3,1,0,2,2,2,0,2,2,1,2,
0,2,3,0,0,0,0,1,0,1,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
3,0,2,3,1,3,3,2,3,3,0,3,3,0,3,2,2,3,2,3,3,3,0,0,2,2,3,0,1,1,1,3,
0,0,3,0,0,0,2,2,0,1,3,0,1,2,2,2,3,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
3,2,3,3,2,0,3,3,2,2,3,1,3,2,1,3,2,0,1,2,2,0,2,3,2,1,0,3,0,0,0,0,
3,0,0,2,3,1,3,0,0,3,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
3,1,3,2,2,2,1,2,0,1,3,1,1,3,1,3,0,0,2,1,1,1,1,2,1,1,1,0,2,1,0,1,
1,2,0,0,0,3,1,1,0,0,0,0,1,0,1,0,0,1,0,1,0,0,0,0,0,3,1,0,0,0,1,0,
3,3,3,3,2,2,2,2,2,1,3,1,1,1,2,0,1,1,2,1,2,1,3,2,0,0,3,1,1,1,1,1,
3,1,0,2,3,0,0,0,3,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,2,3,0,3,3,0,2,0,0,0,0,0,0,0,3,0,0,1,0,0,0,0,0,0,0,0,0,0,0,
0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,2,3,1,3,0,0,1,2,0,0,2,0,3,3,2,3,3,3,2,3,0,0,2,2,2,0,0,0,2,2,
0,0,1,0,0,0,0,3,0,0,0,0,2,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,
0,0,0,3,0,2,0,0,0,0,0,0,0,0,0,0,1,2,3,1,3,3,0,0,1,0,3,0,0,0,0,0,
0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
3,3,1,2,3,1,2,3,1,0,3,0,2,2,1,0,2,1,1,2,0,1,0,0,1,1,1,1,0,1,0,0,
1,0,0,0,0,1,1,0,3,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
3,3,3,3,2,1,0,1,1,1,3,1,2,2,2,2,2,2,1,1,1,1,0,3,1,0,1,3,1,1,1,1,
1,1,0,2,0,1,3,1,1,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,2,0,1,
3,0,2,2,1,3,3,2,3,3,0,1,1,0,2,2,1,2,1,3,3,1,0,0,3,2,0,0,0,0,2,1,
0,1,0,0,0,0,1,2,0,1,1,3,1,1,2,2,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,
0,0,3,0,0,1,0,0,0,3,0,0,3,0,3,1,0,1,1,1,3,2,0,0,0,3,0,0,0,0,2,0,
0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,2,0,0,0,0,0,0,0,0,0,
3,3,1,3,2,1,3,3,1,2,2,0,1,2,1,0,1,2,0,0,0,0,0,3,0,0,0,3,0,0,0,0,
3,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
3,0,1,2,0,3,3,3,2,2,0,1,1,0,1,3,0,0,0,2,2,0,0,0,0,3,1,0,1,0,0,0,
0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
3,0,2,3,1,2,0,0,2,1,0,3,1,0,1,2,0,1,1,1,1,3,0,0,3,1,1,0,2,2,1,1,
0,2,0,0,0,0,0,1,0,1,0,0,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
3,0,0,3,1,2,0,0,2,2,0,1,2,0,1,0,1,3,1,2,1,0,0,0,2,0,3,0,0,0,1,0,
0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
3,0,1,1,2,2,0,0,0,2,0,2,1,0,1,1,0,1,1,1,2,1,0,0,1,1,1,0,2,1,1,1,
0,1,1,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,1,
0,0,0,2,0,1,3,1,1,1,1,0,0,0,0,3,2,0,1,0,0,0,1,2,0,0,0,1,0,0,0,0,
0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,3,3,3,3,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,0,2,3,2,2,0,0,0,1,0,0,0,0,2,3,2,1,2,2,3,0,0,0,2,3,1,0,0,0,1,1,
0,0,1,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,0,
3,3,2,2,0,1,0,0,0,0,2,0,2,0,1,0,0,0,1,1,0,0,0,2,1,0,1,0,1,1,0,0,
0,1,0,2,0,0,1,0,3,0,1,0,0,0,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
3,3,1,0,0,1,0,0,0,0,0,1,1,2,0,0,0,0,1,0,0,1,3,1,0,0,0,0,1,1,0,0,
0,1,0,0,0,0,3,0,0,0,0,0,0,3,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,
3,3,1,1,1,1,2,3,0,0,2,1,1,1,1,1,0,2,1,1,0,0,0,2,1,0,1,2,1,1,0,1,
2,1,0,3,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,3,1,0,0,0,0,0,0,0,3,0,0,0,3,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,
0,0,0,2,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
3,3,2,0,0,0,0,0,0,1,2,1,0,1,1,0,2,0,0,1,0,0,2,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,2,0,0,0,1,3,0,1,0,0,0,2,0,0,0,0,0,0,0,1,2,0,0,0,0,0,
3,3,0,0,1,1,2,0,0,1,2,1,0,1,1,1,0,1,1,0,0,2,1,1,0,1,0,0,1,1,1,0,
0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
2,2,2,1,0,0,0,0,1,0,0,0,0,3,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,
2,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
2,3,0,0,1,1,0,0,0,2,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,1,0,1,2,0,1,2,0,0,1,1,0,2,0,1,0,0,1,0,0,0,0,1,0,0,0,2,0,0,0,0,
1,0,0,1,0,1,1,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,1,0,0,0,0,0,0,0,1,1,0,1,1,0,2,1,3,0,0,0,0,1,1,0,0,0,0,0,0,0,3,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
2,0,1,0,1,0,0,2,0,0,2,0,0,1,1,2,0,0,1,1,0,0,0,1,0,0,0,1,1,0,0,0,
1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,
1,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,1,1,0,0,0,
2,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
2,0,0,0,0,2,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,3,0,0,0,
2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,1,0,0,0,0,
1,0,0,0,0,0,0,0,0,1,0,0,0,0,2,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,1,1,0,0,2,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};


SequenceModel TIS620ThaiModel = 
{
  TIS620CharToOrderMap,
  ThaiLangModel,
  PR_FALSE,
  "TIS-620"
};
