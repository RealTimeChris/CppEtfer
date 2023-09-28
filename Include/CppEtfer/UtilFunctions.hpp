/*
	MIT License

	Copyright (c) 2023 RealTimeChris

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify, merge,
	publish, distribute, sublicense, and/or sell copies of the Software, and to permit
	persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or
	substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
	FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
	OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
	DEALINGS IN THE SOFTWARE.
*/
/// https://github.com/RealTimeChris/jsonifier
/// Feb 3, 2023
#pragma once

#include <cstdint>
#include <bit>

namespace cpp_etfer {

	/*
	* We've written these to maximize portability without requiring all of the #ifdefs etc.
	*/
	template<typename return_type> inline return_type ntohsNew(return_type value) {
		return static_cast<return_type>(((static_cast<uint16_t>(value) & 0x00FF) << 8) | ((static_cast<uint16_t>(value) & 0xFF00) >> 8));
	}

	template<typename return_type> inline return_type ntohlNew(return_type value) {
		return static_cast<return_type>(((static_cast<uint32_t>(value) & 0x000000FF) << 24) | ((static_cast<uint32_t>(value) & 0x0000FF00) << 8) |
			((static_cast<uint32_t>(value) & 0x00FF0000) >> 8) | ((static_cast<uint32_t>(value) & 0xFF000000) >> 24));
	}

	template<typename return_type> inline return_type ntohllNew(return_type value) {
		return static_cast<return_type>(((static_cast<uint64_t>(value) & 0x00000000000000FFULL) << 56) | ((static_cast<uint64_t>(value) & 0x000000000000FF00ULL) << 40) |
			((static_cast<uint64_t>(value) & 0x0000000000FF0000ULL) << 24) | ((static_cast<uint64_t>(value) & 0x00000000FF000000ULL) << 8) |
			((static_cast<uint64_t>(value) & 0x000000FF00000000ULL) >> 8) | ((static_cast<uint64_t>(value) & 0x0000FF0000000000ULL) >> 24) |
			((static_cast<uint64_t>(value) & 0x00FF000000000000ULL) >> 40) | ((static_cast<uint64_t>(value) & 0xFF00000000000000ULL) >> 56));
	}

	/// @brief Reverses the byte order of a value if needed, based on the endianness.
	/// @tparam return_type The type of the value to reverse.
	/// @param net The value to reverse.
	/// @return The reversed value.
	template<typename return_type> inline return_type reverseByteOrder(return_type net) {
		if constexpr (std::endian::native == std::endian::little) {
			switch (sizeof(return_type)) {
				case 2: {
					return ntohsNew(net);
				}
				case 4: {
					return ntohlNew(net);
				}
				case 8: {
					return ntohllNew(net);
				}
				default: {
					return net;
				}
			}
		} else {
			return net;
		}
	}

	/// @brief Stores the bits of a number into a character array.
	/// @tparam return_type The type of the number.
	/// @param to The character array to store the bits.
	/// @param num The number whose bits are to be stored.
	template<typename return_type> inline void storeBits(uint8_t* to, return_type num) {
		const uint8_t byteSize{ 8 };
		num = reverseByteOrder(num);

		// Store the bits of the number in the character array
		for (uint64_t x = 0; x < sizeof(return_type); ++x) {
			to[x] = static_cast<uint8_t>(num >> (byteSize * x));
		}
	}

}