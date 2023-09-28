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

#include <CppEtfer/UtilFunctions.hpp>
#include <CppEtfer/EtfType.hpp>
#include <stdexcept>
#include <cstring>
#include <string>

namespace cpp_etfer {

	class structural_iterator {
	  public:
		template<typename value_type, typename value_type_02> friend struct parse_impl;

		using iterator_category = std::forward_iterator_tag;
		using value_type		= uint8_t;
		using pointer			= value_type*;
		using size_type			= uint64_t;

		inline structural_iterator(pointer rootIndexNew, uint64_t stringLengthNew) {
			stringLength = stringLengthNew;
			currentIndex = rootIndexNew;
			rootIndex	 = currentIndex;
			uint8_t formatVersionNew{};
			readBitsFromBuffer(formatVersionNew);
			if (formatVersionNew != formatVersion) {
				throw std::runtime_error{ "structural_iterator::structural_iterator() Error: Incorrect format version specified." };
			}
		}

		inline value_type operator*() const {
			return *currentIndex;
		}

		inline structural_iterator& operator++() {
			++currentIndex;
			return *this;
		}

		inline structural_iterator operator+(uint64_t subtractAmount) {
			structural_iterator newIterator{ *this };
			for (uint64_t x = 0; x < subtractAmount; ++x) {
				++newIterator.currentIndex;
			}
			return newIterator;
		}

		inline structural_iterator& operator+=(uint64_t subtractAmount) {
			for (uint64_t x = 0; x < subtractAmount; ++x) {
				++currentIndex;
			}
			return *this;
		}

		inline structural_iterator operator-(uint64_t subtractAmount) {
			structural_iterator newIterator{ *this };
			for (uint64_t x = 0; x < subtractAmount; ++x) {
				--newIterator.currentIndex;
			}
			return newIterator;
		}

		inline size_type getCurrentIndex() const {
			return currentIndex - rootIndex;
		}

		inline void skipToNextValue(uint64_t currentLength) {
			for (uint64_t x = 0; x < currentLength; ++x) {
				++currentIndex;
			}
			return;
		}

		inline bool operator==(const structural_iterator&) const {
			return checkForNullIndex() || checkForstringOverRun();
		}

		inline bool operator==(uint8_t other) const {
			if (checkForNullIndex() || checkForstringOverRun()) {
				return false;
			}
			return **this == other;
		}

	  protected:
		size_type stringLength{};
		pointer currentIndex{};
		pointer rootIndex{};

		inline bool checkForNullIndex() const {
			return !currentIndex;
		}

		inline bool areWeAtAValue(etf_type value) const {
			return **this == static_cast<uint8_t>(value);
		}

		inline uint64_t getCurrentLength() {
			uint8_t type{};
			readBitsFromBuffer(type);
			switch (static_cast<etf_type>(type)) {
				case etf_type::Atom_Ext: {
					uint16_t newValue{};
					readBitsFromBuffer(newValue);
					std::string newString{};
					for (uint64_t x = 0; x < newValue; ++x) {
						uint8_t valueNew{};
						readBitsFromBuffer(valueNew);
						newString.push_back(valueNew);
					}
					return newValue + 3;
				}
				case etf_type::String_Ext: {
					uint16_t newValue{};
					readBitsFromBuffer(newValue);
					std::string newString{};
					for (uint64_t x = 0; x < newValue; ++x) {
						uint8_t valueNew{};
						readBitsFromBuffer(valueNew);
						newString.push_back(valueNew);
					}
					return newValue + 5;
				}
				case etf_type::Small_Big_Ext: {
					uint8_t digits{};
					readBitsFromBuffer<uint8_t>(digits);
					uint8_t sign{};
					readBitsFromBuffer<uint8_t>(sign);


					if (digits > 8) {
						throw std::runtime_error{ "structural_iterator::getCurrentLength() Error: Big integers larger than 8 bytes not supported." };
					}

					uint64_t value = 0;
					uint64_t bits  = 1;
					for (uint8_t x = 0; x < digits; ++x) {
						uint8_t digit{};
						readBitsFromBuffer<uint8_t>(digit);
						value += digit * bits;
						bits <<= 8;
					}
					return static_cast<uint64_t>(digits) + 3;
				}
				case etf_type::New_Float_Ext: {
					for (uint64_t x = 0; x < 8; ++x) {
						uint8_t valueNew{};
						readBitsFromBuffer(valueNew);
					}
					return 9;
				}
				case etf_type::Binary_Ext: {
					uint32_t newValue{};
					readBitsFromBuffer(newValue);
					std::string newString{};
					for (uint64_t x = 0; x < newValue; ++x) {
						uint8_t valueNew{};
						readBitsFromBuffer(valueNew);
						newString.push_back(valueNew);
					}
					return newValue + 5;
				}
				case etf_type::Integer_Ext: {
					uint32_t valueNew{};
					readBitsFromBuffer(valueNew);
					return 5;
				}
				case etf_type::Small_Integer_Ext: {
					uint8_t valueNew{};
					readBitsFromBuffer(valueNew);
					return 2;
				}
				case etf_type::Small_Atom_Ext: {
					uint8_t newValue{};
					readBitsFromBuffer(newValue);
					std::string newString{};
					for (uint64_t x = 0; x < newValue; ++x) {
						uint8_t valueNew{};
						readBitsFromBuffer(valueNew);
						newString.push_back(valueNew);
					}
					return newValue + 2;
				}
				case etf_type::Nil_Ext: {
					return 1;
				}
				case etf_type::List_Ext: {
					uint32_t newValue01{};
					uint64_t newValue02{};
					readBitsFromBuffer(newValue01);
					for (uint64_t x = 0; x < newValue01; ++x) {
						newValue02 += getCurrentLength();
					}
					return newValue02 + 6;
				}
				case etf_type::Map_Ext: {
					uint32_t newValue01{};
					uint64_t newValue02{};
					readBitsFromBuffer(newValue01);
					for (uint64_t x = 0; x < newValue01; ++x) {
						newValue02 += getCurrentLength();
						newValue02 += getCurrentLength();
					}
					return newValue02 + 5;
				}
				default: {
					throw std::runtime_error{ "structural_iterator::getCurrentLength() Error: Unknown data type in ETF, the type: " + std::to_string(type) };
				}
			}
		}

		inline bool checkForstringOverRun() const {
			auto currentIndexTemp = getCurrentIndex();
			return currentIndexTemp < 0 || currentIndexTemp >= stringLength;
		}

		/// @brief Read bits from the data buffer and convert to return_type.
		/// @tparam return_type The type to convert the read data to.
		/// @return The converted value.
		template<typename return_type> inline void readBitsFromBuffer(return_type& newValue) {
			auto currentIndexNew = getCurrentIndex();
			if (currentIndexNew + sizeof(return_type) > stringLength) {
				throw std::out_of_range{ "structural_iterator::readBitsFromBuffer() Error: Read past end of the buffer." };
			}
			std::memcpy(&newValue, currentIndex, sizeof(return_type));
			currentIndex += sizeof(return_type);
			newValue = static_cast<return_type>(reverseByteOrder(newValue));
			return;
		}
	};

}