/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Sat Apr 21 11:40:33 2001
 */
/* Compiler settings for siobjectmodel.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID LIBID_SIOBJECTMODELLIB = {0x928C0EB8,0xF6D2,0x11d3,{0xB1,0x1F,0x00,0xA0,0xC9,0x82,0xA1,0x12}};


const IID IID_Application = {0xE8E457CD,0xF6D3,0x11d3,{0xB1,0x1F,0x00,0xA0,0xC9,0x82,0xA1,0x12}};


const IID IID_SIObject = {0xD60F50AC,0xE3E6,0x11d3,{0x81,0x1D,0x00,0xA0,0xC9,0xAC,0x19,0xA9}};


const IID IID_Project = {0x52969A80,0x493B,0x11d4,{0x80,0xF8,0x00,0xA0,0x24,0x18,0x9A,0x69}};


const IID IID_SIObjectCollection = {0x7C4B6BA1,0xC480,0x11d4,{0x81,0x77,0x00,0xA0,0x24,0x18,0x9A,0x69}};


#ifdef __cplusplus
}
#endif

