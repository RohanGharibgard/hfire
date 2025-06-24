#include "HashFuncs.h"

int SimpleDivisionIntHashFunc(const void * key, int tbl_sz)		{
	int ikey = *((const int *)key);
	
	/* values of tbl_sz should not be power of 2
	   otherwise becomes p lowest order bits of key */
	ikey %= tbl_sz;
	if ( ikey < 0 )  
		ikey += tbl_sz;
	return ikey;
	}
	
int OldFashionedStringHashFunc(const void * key, int tbl_sz)	{
	const char * cptr = key;
	int h = 0;
	
	for( ; *cptr != '\0'; cptr++)	{
		h = (HASH_FUNCS_OLD_FASHIONED_POW_VALUE * h + *cptr) % tbl_sz;
		}
		
	return h;
	}
	
int UniversalStringHashFunc(const void * key, int tbl_sz)	{
	const char * cptr = key;
	int h;
	/* use a weak rand num generator to vary coeff */
	int a = 31415;
	int b = 27183;
	
	for(h = 0; *cptr != '\0'; cptr++, a = a*b % (tbl_sz-1))	{
		h = (a * h + *cptr) % tbl_sz;
		}
		
	return (h < 0) ? (h + tbl_sz) : h;
	}
	
int WeinbergerStringHashFunc(const void * key, int tbl_sz)	{
	const char * cptr = key;
	int val = 0;
	int tmp = 0;
			
	/* hash the key by performing a number of bit operations on it */
	while (*cptr != '\0') {

		val = (val << 4) + (*cptr);

		if (tmp == (val & 0xf0000000)) {
			val = val ^ (tmp >> 24);
			val = val ^ tmp;
			}
		
		cptr++;
		}

	val %= tbl_sz;
	if ( val < 0 )
		val += tbl_sz;
	return val;
	}
	
int ShiftFoldingIntHashFunc(const void * key, int tbl_sz)	{
	int ikey = *((const int *)key);
	int bits_in_key = HASH_FUNCS_GET_NUM_BITS(ikey);
	int shift = 0;
	int bit_shift; 
	int bit_mask = 0x0007;			/* 0000 0000 0000 0111 */
	int i;	
	
	bit_shift = bits_in_key - 3;
	for(i = 0; i < bits_in_key; i += 3)	{
		/* right align bits bringing 0s in on the high side */	
		shift = ikey>>bit_shift;
		/* turn any bits in position left of 3rd to 0 */ 		
		shift &= bit_mask;
		/* increment ikey by shift */		
		ikey += shift;
		/* prepare to shift the next 3 bits over */		
		bit_shift = (bit_shift -= 3) < 0 ? 0 : bit_shift;
		}
	
	ikey %= tbl_sz;
	if ( ikey < 0 )  
		ikey += tbl_sz;
	return ikey;
	}
	
int ShiftFoldingStringHashFunc(const void * key, int tbl_sz)	{
	int s2n = StringToNumberForHashFunc((const char *)key);
	return ShiftFoldingIntHashFunc(&s2n, tbl_sz);
	}
	
int MiddleBitsIntHashFunc(const void * key, int tbl_sz) 	{
	int ikey = *((const int *)key);
	int bits_in_key = HASH_FUNCS_GET_NUM_BITS(ikey);	
	int bits_in_sz = HASH_FUNCS_GET_NUM_BITS(tbl_sz);
	int offset;
	
	/* total number of bits in ikey */
	offset = (bits_in_key - bits_in_sz ) / 2;
	if ( (bits_in_key - offset) < bits_in_sz ) {
		offset = 0;
		}

	/* take the middle bits out of the number */
	ikey = ikey / (int) pow(2, offset);
	ikey = ikey % (int) pow(2, bits_in_sz);

	ikey %= tbl_sz;
	if ( ikey < 0 )  
		ikey += tbl_sz;
	return ikey;
	}
 
int MiddleBitsStringHashFunc(const void * key, int tbl_sz)	{
	int s2n = StringToNumberForHashFunc((const char *)key);
	return MiddleBitsIntHashFunc(&s2n, tbl_sz);
	}
	
int StringToNumberForHashFunc(const char * c) 	{
	int s2n = 0;

	while( *c != 0 )	{
		s2n = s2n * HASH_FUNCS_STR_TO_NUM_POW_VALUE + *c;
		*c++;
		}

	return s2n;
	}
	
/* end of HashFuncs.c */
