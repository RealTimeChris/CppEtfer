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

#include <iostream>

namespace cpp_etfer {

	/// @brief Enumeration for different ETF value types.
	enum class etf_type : uint8_t {
		New_Float_Ext	  = 70,
		Small_Integer_Ext = 97,
		Integer_Ext		  = 98,
		Atom_Ext		  = 100,
		Nil_Ext			  = 106,
		String_Ext		  = 107,
		List_Ext		  = 108,
		Binary_Ext		  = 109,
		Small_Big_Ext	  = 110,
		Small_Atom_Ext	  = 115,
		Map_Ext			  = 116,
	};

	std::ostream& operator<<(std::ostream& os, etf_type type) {
		switch (type) {
			case etf_type::New_Float_Ext: {
				os << "New Float Ext" << std::endl;
				break;
			}
			case etf_type::Small_Integer_Ext: {
				os << "Small Integer Ext" << std::endl;
				break;
			}
			case etf_type::Integer_Ext: {
				os << "Integer Ext" << std::endl;
				break;
			}
			case etf_type::Atom_Ext: {
				os << "Atom Ext" << std::endl;
				break;
			}
			case etf_type::Nil_Ext: {
				os << "Nil Ext" << std::endl;
				break;
			}
			case etf_type::String_Ext: {
				os << "String Ext" << std::endl;
				break;
			}
			case etf_type::List_Ext: {
				os << "List Ext" << std::endl;
				break;
			}
			case etf_type::Binary_Ext: {
				os << "Binary Ext" << std::endl;
				break;
			}
			case etf_type::Small_Big_Ext: {
				os << "Small Big Ext" << std::endl;
				break;
			}
			case etf_type::Small_Atom_Ext: {
				os << "Small Atom Ext" << std::endl;
				break;
			}
			case etf_type::Map_Ext: {
				os << "Map Ext" << std::endl;
				break;
			}
			default: {
				os << "Unknown Type" << std::endl;
				break;
			}
		}
		return os;
	}

	constexpr uint8_t formatVersion{ 131 };	

}