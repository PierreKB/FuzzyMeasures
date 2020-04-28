#ifndef BIN_SET_REP_H
#define BIN_SET_REP_H

#include <cstdint>

//Binary representation of a set
using BINSETREP = uint32_t;

namespace bsr
{
	inline BINSETREP meet(BINSETREP A, BINSETREP B) { return A & B; }
	inline BINSETREP join(BINSETREP A, BINSETREP B) { return A | B; }
	inline BINSETREP complement(BINSETREP A) { return ~A; }

	inline unsigned int cardinal(BINSETREP A)
	{
		//Return the cardinal by counting the number of set bits
		A = A - ((A >> 1) & 0x55555555);
		A = (A & 0x33333333) + ((A >> 2) & 0x33333333);
		return (((A + (A >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
	}
}

#endif
