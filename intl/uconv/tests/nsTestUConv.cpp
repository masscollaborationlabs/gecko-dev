/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * The contents of this file are subject to the Netscape Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License.  You may obtain a copy of the License at
 * http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Mozilla Communicator client code.
 *
 * The Initial Developer of the Original Code is Netscape Communications
 * Corporation.  Portions created by Netscape are Copyright (C) 1998
 * Netscape Communications Corporation.  All Rights Reserved.
 */

#define NS_IMPL_IDS

#include <stdio.h>
#include "nsRepository.h"
#include "nsISupports.h"
#include "nsICharsetConverterManager.h"
#include "nsConverterCID.h"
#include "nsUCVJACID.h"
#include "nsUCvLatinCID.h"

#ifdef XP_UNIX

#define UCONV_DLL         "libuconv.so"
#define UCVLATIN_DLL      "libucvlatin.so"
#define UCVJA_DLL         "libucvja.so"
#else /* XP_UNIX */
#ifdef XP_MAC
#define UCONV_DLL       "UCONV_DLL"
#define UCVLATIN_DLL    "UCVLATIN_DLL"
#define UCVJA_DLL       "UCVJA_DLL"
#else /* XP_MAC */
#define UCONV_DLL       "uconv.dll"
#define UCVLATIN_DLL    "ucvlatin.dll"
#define UCVJA_DLL       "ucvja.dll"
#endif 
#endif

#define TABLE_SIZE1     5

nsICharsetConverterManager * ccMan = NULL;

nsresult setupRegistry()
{
  nsresult res;

  res = nsRepository::RegisterFactory(kCharsetConverterManagerCID, UCONV_DLL, PR_FALSE, PR_FALSE);
  if (NS_FAILED(res) && (NS_ERROR_FACTORY_EXISTS != res)) return res;

  res = nsRepository::RegisterFactory(kLatin1ToUnicodeCID, UCVLATIN_DLL, PR_FALSE, PR_FALSE);
  if (NS_FAILED(res) && (NS_ERROR_FACTORY_EXISTS != res)) return res;

  res = nsRepository::RegisterFactory(kCP1253ToUnicodeCID, UCVLATIN_DLL, PR_FALSE, PR_FALSE);
  if (NS_FAILED(res) && (NS_ERROR_FACTORY_EXISTS != res)) return res;

  res = nsRepository::RegisterFactory(kISO88597ToUnicodeCID, UCVLATIN_DLL, PR_FALSE, PR_FALSE);
  if (NS_FAILED(res) && (NS_ERROR_FACTORY_EXISTS != res)) return res;

  res = nsRepository::RegisterFactory(kSJIS2UnicodeCID, UCVJA_DLL, PR_FALSE, PR_FALSE);
  if (NS_FAILED(res) && (NS_ERROR_FACTORY_EXISTS != res)) return res;

  return NS_OK;
}

nsresult init()
{
  nsresult res;

  res = setupRegistry();
  if (NS_FAILED(res)) {
    printf("Error setting up registry: 0x%x",res);
    return res;
  }

  return NS_OK;
}

nsresult done()
{
  if (ccMan != NULL) NS_RELEASE(ccMan);
  return NS_OK;
}

nsresult testCharsetConverterManager()
{
  printf("\n[T1] CharsetConverterManager\n");

  nsresult res = nsRepository::CreateInstance(kCharsetConverterManagerCID,
      NULL, kICharsetConverterManagerIID, (void **)&ccMan);
  if (NS_FAILED(res)) {
    printf("ERROR 0x%x: Cannot instantiate.\n",res);
    return res;
  } else {
    printf("Instantiated.\n");
  }

  nsString ** cs;
  PRInt32 ct;

  res = ccMan->GetEncodableCharsets(&cs, &ct);
  if (NS_FAILED(res)) {
    printf("ERROR 0x%x: GetEncodableCharsets()\n",res);
  } else {
    printf("Unicode encoders (%d): ", ct);
    for (int i=0;i<ct;i++) {
      char * cstr = cs[i]->ToNewCString();
      printf("%s ",cstr);
      delete [] cstr;
    }
    printf("\n");
  }
  delete [] cs;

  res = ccMan->GetDecodableCharsets(&cs, &ct);
  if (NS_FAILED(res)) {
    printf("ERROR 0x%x: GetDecodableCharsets()\n",res);
  } else {
    printf("Unicode decoders (%d): ", ct);
    for (int i=0;i<ct;i++) {
      char * cstr = cs[i]->ToNewCString();
      printf("%s ",cstr);
      delete [] cstr;
    }
    printf("\n");
  }
  delete [] cs;

  return NS_OK;
}

nsresult testLatin1Decoder()
{
  printf("\n[T2] Latin1ToUnicode\n");

  // create converter
  nsIUnicodeDecoder * dec;
  nsAutoString str("iso-8859-1");
  nsresult res = ccMan->GetUnicodeDecoder(&str,&dec);
  if (NS_FAILED(res)) {
    printf("ERROR 0x%x: Cannot instantiate.\n",res);
    return res;
  } else {
    printf("Instantiated.\n");
  }

  //test converter

  PRInt32 srcL = TABLE_SIZE1;
  PRInt32 destL = TABLE_SIZE1;
  char src [TABLE_SIZE1] = {(char)0,(char)255,(char)13,(char)127,(char)128,};
  PRUnichar dest [TABLE_SIZE1];

  res=dec->Convert(dest, 0, &destL, src, 0, &srcL);
  if (NS_FAILED(res)) {
    printf("ERROR 0x%x: Convert().\n",res);
  } else {
    printf("Read %d, write %d.\n",srcL,destL);
    printf("Converted:");

    PRBool failed = PR_FALSE;
    for (int i=0;i<TABLE_SIZE1;i++) {
      printf(" %d->%x", src[i], dest[i]);
      if (dest[i] != ((PRUint8)src[i])) failed = PR_TRUE;
    }
    printf("\n");

    if (failed) {
      printf("Test FAILED!!!\n");
    } else {
      printf("Test Passed.\n");
    }
  }

  NS_RELEASE(dec);

  return NS_OK;

}


#define ISO88597TABLE_SIZE 16
nsresult testISO88597Decoder()
{
  printf("\n[T3] ISO-8859-7 To Unicode\n");

  // create converter
  nsIUnicodeDecoder * dec;
  nsAutoString str("iso-8859-7");
  nsresult res = ccMan->GetUnicodeDecoder(&str,&dec);
  if (NS_FAILED(res)) {
    printf("ERROR 0x%x: Cannot instantiate.\n",res);
    return res;
  } else {
    printf("Instantiated.\n");
  }

  //test converter

  PRInt32 srcL = ISO88597TABLE_SIZE;
  PRInt32 destL = ISO88597TABLE_SIZE;
  // some randon C0, ASCII, C1 and ISO-8859-7 code
  static char src [ISO88597TABLE_SIZE + 1] = 
  {
    "\x09\x0d\x20\x40"
    "\x80\x98\xa3\xaf"
    "\xa7\xb1\xb3\xc9"
    "\xd9\xe3\xf4\xff"
  };
  static PRUnichar expect [ISO88597TABLE_SIZE] = 
  {
   0x0009, 0x000d, 0x0020, 0x0040, 
   0xfffd, 0xfffd, 0x00a3, 0x2015,
   0x00a7, 0x00b1, 0x00b3, 0x0399,
   0x03a9, 0x03b3, 0x03c4, 0xfffd

  };
  PRUnichar dest [ISO88597TABLE_SIZE];

  res=dec->Convert(dest, 0, &destL, src, 0, &srcL);
  if (NS_FAILED(res)) {
    printf("ERROR 0x%x: Convert().\n",res);
  } else {
    printf("Read %d, write %d.\n",srcL,destL);
    printf("Converted:");

    PRBool failed = PR_FALSE;
    for (int i=0;i<ISO88597TABLE_SIZE;i++) {
      printf("%x convert to %x. expect value is %x\n" , src[i], dest[i], expect[i]);
      if (dest[i] != expect[i]) failed = PR_TRUE;
    }
    printf("\n");

    if (failed) {
      printf("Test FAILED!!!\n");
    } else {
      printf("Test Passed.\n");
    }
  }

  NS_RELEASE(dec);

  return NS_OK;
}


#define SJIS_TEST_SRC_SIZE 40
#define SJIS_TEST_DEST_SIZE 24
nsresult testSJISDecoder()
{
/*
[Shift_JIS=4a][Shift_JIS=61][Shift_JIS=70][Shift_JIS=61][Shift_JIS=6e][Shift_JIS=65][Shift_JIS=73][Shift_JIS=65]
[Shift_JIS=8abf][Shift_JIS=8e9a]
[Shift_JIS=834a][Shift_JIS=835e][Shift_JIS=834a][Shift_JIS=8369]
[Shift_JIS=82d0][Shift_JIS=82e7][Shift_JIS=82aa][Shift_JIS=82c8]
[Shift_JIS=8250][Shift_JIS=8251][Shift_JIS=8252][Shift_JIS=8260][Shift_JIS=8261][Shift_JIS=8262]
U+004A U+0061 U+0070 U+0061 U+006E U+0065 U+0073 U+0065
U+6F22 U+5B57
U+30AB U+30BF U+30AB U+30CA
U+3072 U+3089 U+304C U+306A
U+FF11 U+FF12 U+FF13 U+FF21 U+FF22 U+FF23 
 */
  static const char src[SJIS_TEST_SRC_SIZE + 1] = {
   "Japanese" /* English */
   "\x8a\xbf\x8e\x9a" /* Kanji */
   "\x83\x4a\x83\x5e\x83\x4a\x83\x69" /* Kantakana */
   "\x82\xd0\x82\xe7\x82\xaa\x82\xc8" /* Hiragana */
   "\x82\x50\x82\x51\x82\x52\x82\x60\x82\x61\x82\x62" /* full width 123ABC */
  };
  static const PRUnichar expect[SJIS_TEST_DEST_SIZE] = {
   0x004A, 0x0061, 0x0070, 0x0061, 0x006E, 0x0065, 0x0073, 0x0065,
   0x6f22, 0x5b57,
   0x30ab, 0x30bf, 0x30ab, 0x30ca,
   0x3072, 0x3089, 0x304c, 0x306a,
   0xff11, 0xff12, 0xff13, 0xff21, 0xff22, 0xff23
  };
  PRUnichar dest[SJIS_TEST_DEST_SIZE + 44];
  printf("\n[T3] SJIS2Unicode\n");

  // create converter
  nsIUnicodeDecoder * dec;
  nsAutoString str("Shift_JIS");
  nsresult res = ccMan->GetUnicodeDecoder(&str,&dec);
  if (NS_FAILED(res)) {
    printf("ERROR 0x%x: Cannot instantiate.\n",res);
    return res;
  } else {
    printf("Instantiated.\n");
  }

  //test converter

  PRInt32 srcL = SJIS_TEST_SRC_SIZE;
  PRInt32 destL = SJIS_TEST_DEST_SIZE+ 44;
  PRBool failed = PR_FALSE;
  printf("Test Normal Case\n");

  res=dec->Convert(dest, 0, &destL, src, 0, &srcL);
  if (NS_FAILED(res)) {
    printf("ERROR 0x%x: Convert().\n",res);
	failed = PR_TRUE;
  } else {
    printf("Read %d, write %d.\n",srcL,destL);
    printf("Converted:");


    if(destL != SJIS_TEST_DEST_SIZE)
    {
      printf("Dest Size bad. Should be %d but got %d!!\n", 
             SJIS_TEST_DEST_SIZE, destL);
      failed = PR_TRUE;
    }
    for (int i=0;i< destL ;i++) {
      if (dest[i] != expect[i] ) failed = PR_TRUE;
    }
    printf("\n");
    if (failed) {
      printf("Test FAILED!!!\n");
    } else {
      printf("Test Passed.\n");
    }
	failed = PR_FALSE;

    // 	Test NS_PARTIAL_MORE_INPUT case
    printf("Test NS_PARTIAL_MORE_INPUT Case\n");
	srcL= SJIS_TEST_SRC_SIZE - 1;
	destL = SJIS_TEST_DEST_SIZE+ 44;
    res=dec->Convert(dest, 0, &destL, src, 0, &srcL);

    if (NS_PARTIAL_MORE_INPUT != res) {
      printf("ERROR 0x%x: Convert().\n",res);
	  failed = PR_TRUE;

    } else {
	  if((SJIS_TEST_SRC_SIZE -2 ) != srcL )
	  {
        printf("Src Size bad. Should be %d but got %d!!\n", 
             SJIS_TEST_SRC_SIZE-2, srcL);
        failed = PR_TRUE;
	  }
	  
      if(destL != (SJIS_TEST_DEST_SIZE-1))
      {
        printf("Dest Size bad. Should be %d but got %d!!\n", 
               SJIS_TEST_DEST_SIZE-1, destL);
        failed = PR_TRUE;
      }
	  for (int i=0;i< destL ;i++) {
        if (dest[i] != expect[i] ) failed = PR_TRUE;
	  }
	}
    if (failed) {
      printf("Test FAILED!!!\n");
    } else {
      printf("Test Passed.\n");
    }
	failed = PR_FALSE;

    // 	Test NS_PARTIAL_MORE_OUTPUT case
    printf("Test NS_PARTIAL_MORE_OUTNPUT Case\n");

 	srcL= SJIS_TEST_SRC_SIZE;
	destL = SJIS_TEST_DEST_SIZE-1;
    res=dec->Convert(dest, 0, &destL, src, 0, &srcL);

    if (NS_PARTIAL_MORE_OUTPUT != res) {
      printf("ERROR 0x%x: Convert().\n",res);
	  failed = PR_TRUE;

    } else {
	  if((SJIS_TEST_SRC_SIZE -2 ) != srcL )
	  {
        printf("Src Size bad. Should be %d but got %d!!\n", 
             SJIS_TEST_SRC_SIZE-2, srcL);
        failed = PR_TRUE;
	  }
	  
      if(destL != (SJIS_TEST_DEST_SIZE-1))
      {
        printf("Dest Size bad. Should be %d but got %d!!\n", 
               SJIS_TEST_DEST_SIZE-1, destL);
        failed = PR_TRUE;
      }
	  for (int i=0;i< destL ;i++) {
        if (dest[i] != expect[i] ) failed = PR_TRUE;
	  }
	}

	// 	Test NS_ERROR_ILLEGAL_INPUT case
    //printf("Test NS_ERROR_ILLEGAL_INPUT Case\n");
    // To Be Implement

    if (failed) {
      printf("Test FAILED!!!\n");
    } else {
      printf("Test Passed.\n");
    }
  }

  NS_RELEASE(dec);

  return NS_OK;
}

nsresult testAsciiEncoder()
{
  printf("\n[T5] HackyAsciiEncoder\n");

  // create converter
  nsIUnicodeEncoder * enc;
  nsresult res = ccMan->GetUnicodeEncoder(NULL,&enc);
  if (NS_FAILED(res)) {
    printf("ERROR 0x%x: Cannot instantiate.\n",res);
    return res;
  } else {
    printf("Instantiated.\n");
  }

  //test converter

  PRInt32 srcL = TABLE_SIZE1;
  PRInt32 destL = TABLE_SIZE1;
  PRUnichar src [TABLE_SIZE1] = {(PRUnichar)0,(PRUnichar)255,(PRUnichar)13,(PRUnichar)127,(PRUnichar)128};
  unsigned char dest [TABLE_SIZE1];

  res=enc->Convert(src, 0, &srcL, (char *)dest, 0, &destL);
  if (NS_FAILED(res)) {
    printf("ERROR 0x%x: Convert().\n",res);
  } else {
    printf("Read %d, write %d.\n",srcL,destL);
    printf("Converted:");

    PRBool failed = PR_FALSE;
    for (int i=0;i<TABLE_SIZE1;i++) {
      printf(" %x->%d", src[i], dest[i]);
      if (dest[i] != ((PRUint8)src[i])) failed = PR_TRUE;
    }
    printf("\n");

    if (failed) {
      printf("Test FAILED!!!\n");
    } else {
      printf("Test Passed.\n");
    }
  }

  NS_RELEASE(enc);
  return NS_OK;

}


nsresult run()
{
  nsresult res;

  res = testCharsetConverterManager();
  if (NS_FAILED(res)) return res;

  res = testLatin1Decoder();
  if (NS_FAILED(res)) return res;

  res = testSJISDecoder();
  if (NS_FAILED(res)) return res;

  res = testISO88597Decoder();
  if (NS_FAILED(res)) return res;

  res = testAsciiEncoder();
  if (NS_FAILED(res)) return res;

  return NS_OK;
}

int main(int argc, char **argv)
{
  nsresult res;

  printf("*** Unicode Converters Test ***\n");

  res = init();
  if (NS_FAILED(res)) return -1;
  run();
  done();

  printf("\n***---------  Done  --------***\n");
  return 0;
}
