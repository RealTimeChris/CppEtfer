/*
	MIT License

	Copyright 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/// Sep 28, 2023
/// https://github.com/RealTimeChris/CppEtfer
/// \file CppEtfer.hpp

#pragma once

#include <CppEtfer/Concepts.hpp>

#include <unordered_map>
#include <string_view>
#include <stdexcept>
#include <iostream>
#include <numeric>
#include <cstring>
#include <vector>
#include <string>
#include <bit>

namespace CppEtfer {

	/*
	* We've written these to maximize portability without requiring all of the #ifdefs etc.
	*/
	template<typename return_type> inline return_type ntohsNew(return_type value) {
		return static_cast<return_type>(((value & 0x00FF) << 8) | ((value & 0xFF00) >> 8));
	}

	template<typename return_type> inline return_type ntohlNew(return_type value) {
		return static_cast<return_type>(((value & 0x000000FF) << 24) | ((value & 0x0000FF00) << 8) | ((value & 0x00FF0000) >> 8) | ((value & 0xFF000000) >> 24));
	}

	template<typename return_type> inline return_type ntohllNew(return_type value) {
		return static_cast<return_type>(((value & 0x00000000000000FFULL) << 56) | ((value & 0x000000000000FF00ULL) << 40) | ((value & 0x0000000000FF0000ULL) << 24) |
			((value & 0x00000000FF000000ULL) << 8) | ((value & 0x000000FF00000000ULL) >> 8) | ((value & 0x0000FF0000000000ULL) >> 24) |
			((value & 0x00FF000000000000ULL) >> 40) | ((value & 0xFF00000000000000ULL) >> 56));
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
		}
		else {
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

	/// @brief Enumeration for different ETF value types.
	enum class etf_type : uint8_t {
		New_Float_Ext = 70,
		Small_Integer_Ext = 97,
		Integer_Ext = 98,
		Atom_Ext = 100,
		Nil_Ext = 106,
		String_Ext = 107,
		List_Ext = 108,
		Binary_Ext = 109,
		Small_Big_Ext = 110,
		Small_Atom_Ext = 115,
		Map_Ext = 116,
	};

	constexpr uint8_t formatVersion{ 131 };

	/// @brief Class for parsing ETF data into JSON format.
	class etf_parser {
	public:

		/// @brief Parse ETF data to JSON format.
		/// @param dataToParse The ETF data to be parsed.
		/// @return The JSON representation of the parsed data.
		template<string_t string_type> inline std::string_view parseEtfToJson(string_type&& dataToParse) {
			if (dataBuffer.size() < dataToParse.size()) {
				dataBuffer.resize(dataToParse.size());
			}
			std::memcpy(dataBuffer.data(), dataToParse.data(), dataToParse.size());
			dataSize = dataToParse.size();
			finalString.clear();
			currentSize = 0;
			offSet = 0;
			if (readBitsFromBuffer<uint8_t>() != formatVersion) {
				throw std::runtime_error{ "etf_parser::parseEtfToJson() Error: Incorrect format version specified." };
			}
			singleValueETFToJson();
			return std::string_view{ finalString.data(), currentSize };
		}

	protected:
		std::basic_string<uint8_t> dataBuffer{};///< Pointer to ETF data buffer.
		std::basic_string<char> finalString{};///< The final JSON string.
		uint64_t currentSize{};///< Current size of the JSON string.
		uint64_t dataSize{};///< Size of the ETF data.
		uint64_t offSet{};///< Current offset in the ETF data.

		/// @brief Read bits from the data buffer and convert to return_type.
		/// @tparam return_type The type to convert the read data to.
		/// @return The converted value.
		template<typename return_type> inline return_type readBitsFromBuffer() {
			if (offSet + sizeof(return_type) > dataSize) {
				throw std::out_of_range{ "etf_parser::readBitsFromBuffer() Error: readBitsFromBuffer() past end of the buffer." };
			}
			return_type newValue{};
			std::memcpy(&newValue, dataBuffer.data() + offSet, sizeof(return_type));
			offSet += sizeof(return_type);
			newValue = reverseByteOrder(newValue);
			return newValue;
		}

		/// @brief Write characters to the final JSON string.
		/// @param data Pointer to the data to be written.
		/// @param length Number of characters to write.
		template<typename value_type> inline void writeCharacters(const value_type* data, uint64_t length) {
			if (finalString.size() < currentSize + length) {
				finalString.resize((finalString.size() + length) * 2);
			}
			std::memcpy(finalString.data() + currentSize, data, length);
			currentSize += length;
		}

		/// @brief Write characters from the buffer to the final JSON string.
		/// @param length Number of characters to write from the buffer.
		inline void writeCharactersFromBuffer(uint32_t length) {
			if (!length) {
				writeCharacters("\"\"", 2);
				return;
			}
			if (offSet + static_cast<uint64_t>(length) > dataSize) {
				throw std::out_of_range{ "etf_parser::writeCharactersFromBuffer() Error: Read past end of buffer." };
			}
			if (finalString.size() < currentSize + length) {
				finalString.resize((finalString.size() + length) * 2);
			}
			const uint8_t* stringNew = dataBuffer.data() + offSet;
			offSet += length;
			if (length >= 3 && length <= 5) {
				if (length == 3 && stringNew[0] == 'n' && stringNew[1] == 'i' && stringNew[2] == 'l') {
					writeCharacters("null", 4);
					return;
				}
				else if (length == 4 && stringNew[0] == 'n' && stringNew[1] == 'u' && stringNew[2] == 'l' && stringNew[3] == 'l') {
					writeCharacters("null", 4);
					return;
				}
				else if (length == 4 && stringNew[0] == 't' && stringNew[1] == 'r' && stringNew[2] == 'u' && stringNew[3] == 'e') {
					writeCharacters("true", 4);
					return;
				}
				else if (length == 5 && stringNew[0] == 'f' && stringNew[1] == 'a' && stringNew[2] == 'l' && stringNew[3] == 's' && stringNew[4] == 'e') {
					writeCharacters("false", 5);
					return;
				}
			}
			writeCharacter<'"'>();
			for (uint64_t x = 0; x < length; ++x) {
				switch (stringNew[x]) {
				case '\\': {
					switch (stringNew[++x]) {
					case '\"':
						writeCharacter<'\"'>();
						break;
					case '\\':
						writeCharacter<'\\'>();
						break;
					case 'b':
						writeCharacter<'\b'>();
						break;
					case 'f':
						writeCharacter<'\f'>();
						break;
					case 'n':
						writeCharacter<'\n'>();
						break;
					case 'r':
						writeCharacter<'\r'>();
						break;
					case 't':
						writeCharacter<'\t'>();
						break;
					default: {
						writeCharacter(stringNew[x]);
						break;
					}
					}
					break;
				}
				case '"': {
					writeCharacter<'\\'>();
					writeCharacter<'\"'>();
					break;
				}
				default: {
					writeCharacter(stringNew[x]);
					break;
				}
				}
			}
			writeCharacter<'"'>();
		}

		/// @brief Write a character to the final JSON string.
		/// @param value The character to write.
		inline void writeCharacter(const char value) {
			if (finalString.size() < currentSize + 1) {
				finalString.resize((finalString.size() + 1) * 2);
			}
			finalString[currentSize++] = value;
		}

		/// @brief Write a character to the final JSON string.
		/// @tparam value The character to write.
		template<const char charToWrite> inline void writeCharacter() {
			if (finalString.size() < currentSize + 1) {
				finalString.resize((finalString.size() + 1) * 2);
			}
			finalString[currentSize++] = charToWrite;
		}

		/// @brief Parse a single ETF value and convert to JSON.
		void singleValueETFToJson() {
			if (offSet > dataSize) {
				throw std::out_of_range{ "etf_parser::singleValueETFToJson() Error: Read past end of buffer." };
			}
			uint8_t type = readBitsFromBuffer<int8_t>();
			switch (static_cast<etf_type>(type)) {
			case etf_type::New_Float_Ext: {
				return parseNewFloatExt();
			}
			case etf_type::Small_Integer_Ext: {
				return parseSmallIntegerExt();
			}
			case etf_type::Integer_Ext: {
				return parseIntegerExt();
			}
			case etf_type::Atom_Ext: {
				return parseAtomExt();
			}
			case etf_type::Nil_Ext: {
				return parseNilExt();
			}
			case etf_type::String_Ext: {
				return parseStringExt();
			}
			case etf_type::List_Ext: {
				return parseListExt();
			}
			case etf_type::Binary_Ext: {
				return parseBinaryExt();
			}
			case etf_type::Small_Big_Ext: {
				return parseSmallBigExt();
			}
			case etf_type::Small_Atom_Ext: {
				return parseSmallAtomExt();
			}
			case etf_type::Map_Ext: {
				return parseMapExt();
			}
			default: {
				throw std::runtime_error{ "etf_parser::singleValueETFToJson() Error: Unknown data type in ETF, the type: " + std::to_string(type) };
			}
			}
		}

		/// @brief Parse ETF data representing a list and convert to JSON array.
		inline void parseListExt() {
			uint32_t length = readBitsFromBuffer<uint32_t>();
			writeCharacter<'['>();
			if (static_cast<uint64_t>(offSet) + length > dataSize) {
				throw std::out_of_range{ "etf_parser::parseListExt() Error: Read past end of buffer." };
			}
			for (uint16_t x = 0; x < length; ++x) {
				singleValueETFToJson();
				if (x < length - 1) {
					writeCharacter<','>();
				}
			}
			readBitsFromBuffer<uint8_t>();
			writeCharacter<']'>();
		}

		/// @brief Parse ETF data representing a small integer and convert to JSON number.
		inline void parseSmallIntegerExt() {
			auto string = std::to_string(readBitsFromBuffer<uint8_t>());
			writeCharacters(string.data(), string.size());
		}

		/// @brief Parse ETF data representing an integer and convert to JSON number.
		inline void parseIntegerExt() {
			auto string = std::to_string(readBitsFromBuffer<uint32_t>());
			writeCharacters(string.data(), string.size());
		}

		/// @brief Parse ETF data representing a string and convert to JSON string.
		inline void parseStringExt() {
			writeCharacter<'"'>();
			uint16_t length = readBitsFromBuffer<uint16_t>();
			if (static_cast<uint64_t>(offSet) + length > dataSize) {
				throw std::out_of_range{ "etf_parser::parseStringExt() Error: Read past end of buffer." };
			}
			for (uint16_t x = 0; x < length; ++x) {
				parseSmallIntegerExt();
			}
			writeCharacter<'"'>();
		}

		/// @brief Parse ETF data representing a new float and convert to JSON number.
		inline void parseNewFloatExt() {
			uint64_t value = readBitsFromBuffer<uint64_t>();
			double newDouble{};
			std::memcpy(&newDouble, &value, sizeof(double));
			std::string valueNew = std::to_string(newDouble);
			writeCharacters(valueNew.data(), valueNew.size());
		}

		/// @brief Parse ETF data representing a small big integer and convert to JSON number.
		inline void parseSmallBigExt() {
			writeCharacter<'"'>();
			auto digits = readBitsFromBuffer<uint8_t>();
			uint8_t sign = readBitsFromBuffer<uint8_t>();


			if (digits > 8) {
				throw std::runtime_error{ "etf_parser::parseSmallBigExt() Error: Big integers larger than 8 bytes not supported." };
			}

			uint64_t value = 0;
			uint64_t bits = 1;
			for (uint8_t x = 0; x < digits; ++x) {
				uint64_t digit = readBitsFromBuffer<uint8_t>();
				value += digit * bits;
				bits <<= 8;
			}

			if (sign == 0) {
				auto string = std::to_string(value);
				writeCharacters(string.data(), string.size());
				writeCharacter<'"'>();
			}
			else {
				auto string = std::to_string(-(static_cast<int64_t>(value)));
				writeCharacters(string.data(), string.size());
				writeCharacter<'"'>();
			}
		}

		/// @brief Parse ETF data representing an atom and convert to JSON string.
		inline void parseAtomExt() {
			writeCharactersFromBuffer(readBitsFromBuffer<uint16_t>());
		}

		/// @brief Parse ETF data representing a binary and convert to JSON string.
		inline void parseBinaryExt() {
			writeCharactersFromBuffer(readBitsFromBuffer<uint32_t>());
		}

		/// @brief Parse ETF data representing a nil value and convert to JSON null.
		inline void parseNilExt() {
			writeCharacters("[]", 2);
		}

		/// @brief Parse ETF data representing a small atom and convert to JSON string.
		inline void parseSmallAtomExt() {
			writeCharactersFromBuffer(readBitsFromBuffer<uint8_t>());
		}

		/// @brief Parse ETF data representing a map and convert to JSON object.
		inline void parseMapExt() {
			uint32_t length = readBitsFromBuffer<uint32_t>();
			writeCharacter<'{'>();
			for (uint32_t x = 0; x < length; ++x) {
				singleValueETFToJson();
				writeCharacter<':'>();
				singleValueETFToJson();
				if (x < length - 1) {
					writeCharacter<','>();
				}
			}
			writeCharacter<'}'>();
		}
	};

	/// @brief Enumeration for different JSON value types.
	enum class json_type : uint8_t { null_t = 0, object_t = 1, array_t = 2, string_t = 3, float_t = 4, uint_t = 5, int_t = 6, bool_t = 7 };

	/// @brief Class for serializing data into the ETF format.
	class etf_serializer {
	  public:		
		template<typename value_type> using allocator = std::allocator<value_type>;
		template<typename value_type> using allocator_traits = std::allocator_traits<allocator<value_type>>;
		using object_type = std::unordered_map<std::string, etf_serializer>;
		using array_type = std::vector<etf_serializer>;
		using string_type = std::string;
		using float_type = double;
		using uint_type = uint64_t;
		using int_type = int64_t;
		using bool_type = bool;

		/// @brief Default constructor.
		inline etf_serializer() = default;

		/// @brief Move assignment operator.
		/// @param data The data to be moved into this object.
		/// @return A reference to this object after the move.
		inline etf_serializer& operator=(etf_serializer&& data) noexcept {
			destroyImpl();
			stringReal = std::move(data.stringReal);
			type	   = data.type;
			data.type  = json_type::null_t;
			switch (type) {
				case json_type::object_t: {
					objectValue		 = data.objectValue;
					data.objectValue = nullptr;
					break;
				}
				case json_type::array_t: {
					arrayValue		= data.arrayValue;
					data.arrayValue = nullptr;
					break;
				}
				case json_type::string_t: {
					stringValue		 = data.stringValue;
					data.stringValue = nullptr;
					break;
				}
				case json_type::float_t: {
					floatValue		= data.floatValue;
					data.floatValue = nullptr;
					break;
				}
				case json_type::int_t: {
					intValue	  = data.intValue;
					data.intValue = nullptr;
					break;
				}
				case json_type::uint_t: {
					uintValue	   = data.uintValue;
					data.uintValue = nullptr;
					break;
				}
				case json_type::bool_t: {
					boolValue	   = data.boolValue;
					data.boolValue = nullptr;
					break;
				}
				case json_type::null_t: {
					break;
				}
			}
			return *this;
		}

		/// @brief Move constructor.
		/// @param data The data to be moved into this object.
		inline etf_serializer(etf_serializer&& data) noexcept {
			*this = std::move(data);
		}

		/// @brief Copy assignment operator.
		/// @param data The data to be copied into this object.
		/// @return A reference to this object after the copy.
		inline etf_serializer& operator=(const etf_serializer& data) {
			destroyImpl();
			switch (data.type) {
				case json_type::object_t: {
					setValue<json_type::object_t>(data.getObject());
					break;
				}
				case json_type::array_t: {
					setValue<json_type::array_t>(data.getArray());
					break;
				}
				case json_type::string_t: {
					setValue<json_type::string_t>(data.getString());
					break;
				}
				case json_type::float_t: {
					setValue<json_type::float_t>(data.getFloat());
					break;
				}
				case json_type::uint_t: {
					setValue<json_type::uint_t>(data.getUint());
					break;
				}
				case json_type::int_t: {
					setValue<json_type::int_t>(data.getInt());
					break;
				}
				case json_type::bool_t: {
					setValue<json_type::bool_t>(data.getBool());
					break;
				}
				case json_type::null_t: {
					break;
				}
			}
			stringReal = data.stringReal;
			return *this;
		}

		/// @brief Copy constructor.
		/// @param data The data to be copied into this object.
		inline etf_serializer(const etf_serializer& data) {
			*this = data;
		}

		/// @brief Template operator= overload for assigning values of string type.
		/// @tparam value_type The type of value to assign.
		/// @param data The data to be assigned.
		/// @return A reference to this object after the assignment.
		template<string_t value_type> inline etf_serializer& operator=(value_type&& data) {
			setValue<json_type::string_t>(std::forward<value_type>(data));
			return *this;
		}

		/// @brief Template constructor for assigning values of string type.
		/// @tparam value_type The type of value to assign.
		/// @param data The data to be assigned.
		template<string_t value_type> inline etf_serializer(value_type&& data) {
			*this = std::forward<value_type>(data);
		}

		/// @brief Template operator= overload for assigning values of string type from a character array.
		/// @tparam StrLength The length of the character array.
		/// @param str The character array to be assigned.
		/// @return A reference to this object after the assignment.
		template<uint64_t StrLength> inline etf_serializer& operator=(const char (&str)[StrLength]) {
			setValue<json_type::string_t>(str);
			return *this;
		}

		/// @brief Template constructor for assigning values of string type from a character array.
		/// @tparam StrLength The length of the character array.
		/// @param str The character array to be assigned.
		template<uint64_t StrLength> inline etf_serializer(const char (&str)[StrLength]) {
			*this = str;
		}

		/// @brief Template operator= overload for assigning values of floating-point type.
		/// @tparam value_type The type of value to assign.
		/// @param data The data to be assigned.
		/// @return A reference to this object after the assignment.
		template<float_t value_type> inline etf_serializer& operator=(value_type data) {
			setValue<json_type::float_t>(std::forward<value_type>(data));
			return *this;
		}

		/// @brief Template constructor for assigning values of floating-point type.
		/// @tparam value_type The type of value to assign.
		/// @param data The data to be assigned.
		template<float_t value_type> inline etf_serializer(value_type data) {
			*this = std::forward<value_type>(data);
		}

		/// @brief Template operator= overload for assigning values of integer type (signed or unsigned).
		/// @tparam value_type The type of value to assign.
		/// @param data The data to be assigned.
		/// @return A reference to this object after the assignment.
		template<integer_t value_type> inline etf_serializer& operator=(value_type data) {
			if constexpr (signed_t<value_type>) {
				setValue<json_type::int_t>(std::forward<value_type>(data));
			} else if constexpr (unsigned_t<value_type>) {
				setValue<json_type::uint_t>(std::forward<value_type>(data));
			}
			return *this;
		}

		/// @brief Template constructor for assigning values of integer type.
		/// @tparam value_type The type of value to assign.
		/// @param data The data to be assigned.
		template<integer_t value_type> inline etf_serializer(value_type data) {
			*this = std::forward<value_type>(data);
		}

		/// @brief Template operator= overload for assigning values of boolean type.
		/// @tparam value_type The type of value to assign.
		/// @param data The data to be assigned.
		/// @return A reference to this object after the assignment.
		template<bool_t value_type> inline etf_serializer& operator=(value_type data) {
			setValue<json_type::bool_t>(std::forward<value_type>(data));
			return *this;
		}

		/// @brief Template constructor for assigning values of boolean type.
		/// @tparam value_type The type of value to assign.
		/// @param data The data to be assigned.
		template<bool_t value_type> inline etf_serializer(value_type data) {
			*this = std::forward<value_type>(data);
		}

		/// @brief Template operator= overload for assigning values of enum type.
		/// @tparam value_type The type of value to assign.
		/// @param data The data to be assigned.
		/// @return A reference to this object after the assignment.
		template<enum_t value_type> inline etf_serializer& operator=(value_type&& data) {
			setValue<json_type::int_t>(static_cast<int64_t>(std::forward<value_type>(data)));
			return *this;
		}

		/// @brief Template constructor for assigning values of enum type.
		/// @tparam value_type The type of value to assign.
		/// @param data The data to be assigned.
		template<enum_t value_type> inline etf_serializer(value_type&& data) {
			*this = std::forward<value_type>(data);
		}

		/// @brief Template operator= overload for assigning values of enum type.
		/// @tparam value_type The type of value to assign.
		/// @param data The data to be assigned.
		/// @return A reference to this object after the assignment.
		template<null_t value_type> inline etf_serializer& operator=(value_type&& data) {
			setValue<json_type::null_t>();
			return *this;
		}

		/// @brief Template constructor for assigning values of enum type.
		/// @tparam value_type The type of value to assign.
		/// @param data The data to be assigned.
		template<null_t value_type> inline etf_serializer(value_type&& data) {
			*this = std::forward<value_type>(data);
		}

		/// @brief Operator= overload for assigning values of json_type.
		/// @param data The JSON type to be assigned.
		/// @return A reference to this object after the assignment.
		inline etf_serializer& operator=(json_type data) {
			switch (data) {
				case json_type::object_t: {
					setValue<json_type::object_t>();
					break;
				}
				case json_type::array_t: {
					setValue<json_type::array_t>();
					break;
				}
				case json_type::string_t: {
					setValue<json_type::string_t>();
					break;
				}
				case json_type::float_t: {
					setValue<json_type::float_t>();
					break;
				}
				case json_type::uint_t: {
					setValue<json_type::uint_t>();
					break;
				}
				case json_type::int_t: {
					setValue<json_type::int_t>();
					break;
				}
				case json_type::bool_t: {
					setValue<json_type::bool_t>();
					break;
				}
				case json_type::null_t: {
					setValue<json_type::null_t>();
					break;
				}
			}
			return *this;
		}

		/// @brief Constructor for assigning values of json_type.
		/// @param data The JSON type to be assigned.
		inline etf_serializer(json_type data) {
			*this = data;
		}

		/// @brief Get the JSON type of this object.
		/// @return The JSON type of this object.
		inline json_type getType() const {
			return type;
		}

		/// @brief Conversion operator to std::basic_string<uint8_t>.
		/// @return A UTF-8 string representation of this object.
		inline operator std::basic_string<uint8_t>() {
			stringReal.clear();
			appendVersion();
			serializeJsonToEtfString(*this);
			return stringReal;
		}

		/// @brief Operator[] overload for accessing object elements by key.
		/// @param key The key to access.
		/// @return A reference to the element with the specified key.
		inline etf_serializer& operator[](const typename object_type::key_type& key) {
			if (type == json_type::null_t) {
				setValue<json_type::object_t>();
			}

			if (type == json_type::object_t) {
				return getObject().operator[](key);
			}
			throw std::runtime_error{ "Sorry, but this value's type is not object." };
		}

		/// @brief Template operator[] overload for accessing object elements by key.
		/// @tparam object_type The type of the object.
		/// @param key The key to access.
		/// @return A reference to the element with the specified key.
		template<typename object_type> inline etf_serializer& operator[](typename object_type::key_type&& key) {
			if (type == json_type::null_t) {
				setValue<json_type::object_t>();
			}

			if (type == json_type::object_t) {
				return getObject().operator[](std::forward<typename object_type::key_type>(key));
			}
			throw std::runtime_error{ "Sorry, but this value's type is not object." };
		}

		/// @brief Operator[] overload for accessing elements in an array by index.
		/// @param index The index to access.
		/// @return A reference to the element at the specified index.
		inline etf_serializer& operator[](uint64_t index) {
			if (type == json_type::null_t) {
				setValue<json_type::array_t>();
			}

			if (type == json_type::array_t) {
				if (index >= getArray().size()) {
					getArray().resize(index + 1);
				}

				return getArray().at(index);
			}
			throw std::runtime_error{ "Sorry, but this value's type is not array." };
		}

		/// @brief Emplace a new element at the back of the array.
		/// @param other The element to be emplaced.
		inline void emplaceBack(etf_serializer&& other) {
			if (type == json_type::null_t) {
				setValue<json_type::array_t>();
			}

			if (type == json_type::array_t) {
				getArray().emplace_back(std::move(other));
				return;
			}
			throw std::runtime_error{ "Sorry, but this value's type is not array." };
		}

		/// @brief Emplace a new element at the back of the array.
		/// @param other The element to be emplaced.
		inline void emplaceBack(const etf_serializer& other) {
			if (type == json_type::null_t) {
				setValue<json_type::array_t>();
			}

			if (type == json_type::array_t) {
				getArray().emplace_back(other);
				return;
			}
			throw std::runtime_error{ "Sorry, but this value's type is not array." };
		}

		/// @brief Operator== overload for comparing two etf_serializer objects for equality.
		/// @param lhs The left-hand side of the comparison.
		/// @return True if the objects are equal, false otherwise.
		inline bool operator==(const etf_serializer& lhs) const {
			if (lhs.type != type) {
				return false;
			}
			switch (type) {
				case json_type::object_t: {
					if (!compareValues<json_type::object_t>(lhs)) {
					return false;
					}
					break;
				}
				case json_type::array_t: {
					if (!compareValues<json_type::array_t>(lhs)) {
					return false;
					}
					break;
				}
				case json_type::string_t: {
					if (!compareValues<json_type::string_t>(lhs)) {
					return false;
					}
					break;
				}
				case json_type::float_t: {
					if (!compareValues<json_type::float_t>(lhs)) {
					return false;
					}
					break;
				}
				case json_type::uint_t: {
					if (!compareValues<json_type::uint_t>(lhs)) {
					return false;
					}
					break;
				}
				case json_type::int_t: {
					if (!compareValues<json_type::int_t>(lhs)) {
					return false;
					}
					break;
				}
				case json_type::bool_t: {
					if (!compareValues<json_type::bool_t>(lhs)) {
					return false;
					}
					break;
				}
				case json_type::null_t: {
					break;
				}
			}
			return true;
		}

		/// @brief Get a reference to the object contained within this etf_serializer.
		/// @return A reference to the contained object.
		inline object_type& getObject() const {
			if (this->type != json_type::object_t) {
				throw std::runtime_error{ "Sorry, but this value's type is not object!" };
			}
			return *objectValue;
		}

		/// @brief Get a reference to the array contained within this etf_serializer.
		/// @return A reference to the contained array.
		inline array_type& getArray() const {
			if (this->type != json_type::array_t) {
				throw std::runtime_error{ "Sorry, but this value's type is not array!" };
			}
			return *arrayValue;
		}

		/// @brief Get a reference to the string contained within this etf_serializer.
		/// @return A reference to the contained string.
		inline string_type& getString() const {
			if (this->type != json_type::string_t) {
				throw std::runtime_error{ "Sorry, but this value's type is not string!" };
			}
			return *stringValue;
		}

		/// @brief Get a reference to the float contained within this etf_serializer.
		/// @return A reference to the contained float.
		inline float_type& getFloat() const {
			if (this->type != json_type::float_t) {
				throw std::runtime_error{ "Sorry, but this value's type is not float!" };
			}
			return *floatValue;
		}

		/// @brief Get a reference to the unsigned integer contained within this etf_serializer.
		/// @return A reference to the contained unsigned integer.
		inline uint_type& getUint() const {
			if (this->type != json_type::uint_t) {
				throw std::runtime_error{ "Sorry, but this value's type is not uint!" };
			}
			return *uintValue;
		}

		/// @brief Get a reference to the signed integer contained within this etf_serializer.
		/// @return A reference to the contained signed integer.
		inline int_type& getInt() const {
			if (this->type != json_type::int_t) {
				throw std::runtime_error{ "Sorry, but this value's type is not int!" };
			}
			return *intValue;
		}

		/// @brief Get a reference to the boolean contained within this etf_serializer.
		/// @return A reference to the contained boolean.
		inline bool_type& getBool() const {
			if (this->type != json_type::bool_t) {
				throw std::runtime_error{ "Sorry, but this value's type is not bool!" };
			}
			return *boolValue;
		}

		/// @brief Destructor for etf_serializer.
		inline ~etf_serializer() {
			destroyImpl();
		}

	  protected:
		std::basic_string<uint8_t> stringReal{};///< The string that stores the serialized JSON.
		json_type type{ json_type::null_t };///< The JSON type stored in the etf_serializer.
		union {
			std::unordered_map<std::string, etf_serializer>* objectValue;///< Pointer to the stored object.
			std::vector<etf_serializer>* arrayValue;///< Pointer to the stored array.
			std::string* stringValue;///< Pointer to the stored string.
			double* floatValue;///< Pointer to the stored float.
			uint64_t* uintValue;///< Pointer to the stored unsigned integer.
			int64_t* intValue;///< Pointer to the stored signed integer.
			bool* boolValue;///< Pointer to the stored boolean.
		};

		/// @brief Serialize an etf_serializer object to an ETF string.
		/// @param dataToParse The etf_serializer object to be serialized.
		inline void serializeJsonToEtfString(const etf_serializer& dataToParse) {
			switch (dataToParse.type) {
				case json_type::object_t: {
					return writeEtfObject(dataToParse.getObject());
				}
				case json_type::array_t: {
					return writeEtfArray(dataToParse.getArray());
				}
				case json_type::string_t: {
					return writeEtfString(dataToParse.getString());
				}
				case json_type::float_t: {
					return writeEtfFloat(dataToParse.getFloat());
				}
				case json_type::uint_t: {
					return writeEtfUint(dataToParse.getUint());
				}
				case json_type::int_t: {
					return writeEtfInt(dataToParse.getInt());
				}
				case json_type::bool_t: {
					return writeEtfBool(dataToParse.getBool());
				}
				case json_type::null_t: {
					return writeEtfNull();
				}
			}
		}

		/// @brief Serialize an object_type to an ETF object.
		/// @param data The object_type to be serialized.
		inline void writeEtfObject(const object_type& data) {
			appendMapHeader(static_cast<uint32_t>(data.size()));
			for (auto& [key, valueNew]: data) {
				appendBinaryExt(key, static_cast<uint32_t>(key.size()));
				serializeJsonToEtfString(valueNew);
			}
		}

		/// @brief Serialize an array_type to an ETF array.
		/// @param data The array_type to be serialized.
		inline void writeEtfArray(const array_type& data) {
			appendListHeader(static_cast<uint32_t>(data.size()));
			for (auto& valueNew: data) {
				serializeJsonToEtfString(valueNew);
			}
			appendNilExt();
		}

		/// @brief Serialize a string_type to an ETF binary string.
		/// @param data The string_type to be serialized.
		inline void writeEtfString(const string_type& data) {
			appendBinaryExt(data, static_cast<uint32_t>(data.size()));
		}

		/// @brief Serialize a uint_type to an ETF unsigned integer.
		/// @param data The uint_type to be serialized.
		inline void writeEtfUint(const uint_type data) {
			if (data <= std::numeric_limits<uint8_t>::max() && data >= std::numeric_limits<uint8_t>::min()) {
				appendUint8(static_cast<uint8_t>(data));
			} else if (data <= std::numeric_limits<uint32_t>::max() && data >= std::numeric_limits<uint32_t>::min()) {
				appendUint32(static_cast<uint32_t>(data));
			} else {
				appendUint64(data);
			}
		}

		/// @brief Serialize an int_type to an ETF signed integer.
		/// @param data The int_type to be serialized.
		inline void writeEtfInt(const int_type data) {
			if (data <= std::numeric_limits<int8_t>::max() && data >= std::numeric_limits<int8_t>::min()) {
				appendInt8(static_cast<int8_t>(data));
			} else if (data <= std::numeric_limits<int32_t>::max() && data >= std::numeric_limits<int32_t>::min()) {
				appendInt32(static_cast<int32_t>(data));
			} else {
				appendInt64(data);
			}
		}

		/// @brief Serialize a float_type to an ETF float.
		/// @param data The float_type to be serialized.
		inline void writeEtfFloat(const float_type data) {
			appendNewFloatExt(data);
		}

		/// @brief Serialize a bool_type to an ETF boolean.
		/// @param data The bool_type to be serialized.
		inline void writeEtfBool(const bool_type data) {
			appendBool(data);
		}

		/// @brief Serialize a null value to ETF null.
		inline void writeEtfNull() {
			appendNil();
		}

		/// @brief Write a sequence of bytes to the `stringReal` member.
		/// @tparam value_type The data type of the bytes.
		/// @param data A pointer to the data to be written.
		/// @param length The length of the data.
		template<typename value_type> inline void writeString(const value_type* data, uint64_t length) {
			auto oldSize = stringReal.size();
			stringReal.resize(stringReal.size() + length);
			std::memcpy(stringReal.data() + oldSize, data, length);
		}

		/// @brief Append a binary extension to the `stringReal` member.
		/// @param bytes The binary data to be appended.
		/// @param sizeNew The size of the binary data.
		inline void appendBinaryExt(const std::string& bytes, uint32_t sizeNew) {
			uint8_t newBuffer[5]{ static_cast<uint8_t>(etf_type::Binary_Ext) };
			storeBits(newBuffer + 1, sizeNew);
			writeString(newBuffer, std::size(newBuffer));
			writeString(bytes.data(), bytes.size());
		}

		/// @brief Append a new float extension to the `stringReal` member.
		/// @param newFloat The double value to be appended as a new float extension.
		inline void appendNewFloatExt(const double newFloat) {
			uint8_t newBuffer[9]{ static_cast<uint8_t>(etf_type::New_Float_Ext) };
			uint64_t newValue{};
			std::memcpy(&newValue, &newFloat, sizeof(newFloat));
			storeBits(newBuffer + 1, newValue);
			writeString(newBuffer, std::size(newBuffer));
		}

		/// @brief Append a list header to the `stringReal` member.
		/// @param sizeNew The size of the list.
		inline void appendListHeader(const uint32_t sizeNew) {
			uint8_t newBuffer[5]{ static_cast<uint8_t>(etf_type::List_Ext) };
			storeBits(newBuffer + 1, sizeNew);
			writeString(newBuffer, std::size(newBuffer));
		}

		/// @brief Append a map header to the `stringReal` member.
		/// @param sizeNew The size of the map.
		inline void appendMapHeader(const uint32_t sizeNew) {
			uint8_t newBuffer[5]{ static_cast<uint8_t>(etf_type::Map_Ext) };
			storeBits(newBuffer + 1, sizeNew);
			writeString(newBuffer, std::size(newBuffer));
		}

		/// @brief Append a uint64_t value to the `stringReal` member.
		/// @param valueNew The uint64_t value to be appended.
		inline void appendUint64(uint64_t valueNew) {
			uint8_t newBuffer[11]{ static_cast<uint8_t>(etf_type::Small_Big_Ext) };
			uint8_t encodedBytes{};
			while (valueNew > 0) {
				newBuffer[3 + encodedBytes] = static_cast<uint8_t>(valueNew & 0xFF);
				valueNew >>= 8;
				++encodedBytes;
			}
			newBuffer[1] = encodedBytes;
			newBuffer[2] = 0;
			writeString(newBuffer, 1ull + 2ull + static_cast<uint64_t>(encodedBytes));
		}

		/// @brief Append an int64_t value to the `stringReal` member.
		/// @param valueNew The int64_t value to be appended.
		inline void appendInt64(int64_t valueNew) {
			uint8_t newBuffer[11]{ static_cast<uint8_t>(etf_type::Small_Big_Ext) };
			uint8_t encodedBytes{};
			while (valueNew > 0) {
				newBuffer[3 + encodedBytes] = valueNew & 0xFF;
				valueNew >>= 8;
				++encodedBytes;
			}
			newBuffer[1] = encodedBytes;
			if (valueNew >= 0) {
				newBuffer[2] = 0;
			} else {
				newBuffer[2] = 1;
			}
			writeString(newBuffer, 1ull + 2ull + static_cast<uint64_t>(encodedBytes));
		}

		/// @brief Append a uint32_t value to the `stringReal` member.
		/// @param valueNew The uint32_t value to be appended.
		inline void appendUint32(const uint32_t valueNew) {
			uint8_t newBuffer[5]{ static_cast<uint8_t>(etf_type::Integer_Ext) };
			storeBits(newBuffer + 1, valueNew);
			writeString(newBuffer, std::size(newBuffer));
		}

		/// @brief Append an int32_t value to the `stringReal` member.
		/// @param valueNew The int32_t value to be appended.
		inline void appendInt32(const int32_t valueNew) {
			uint8_t newBuffer[5]{ static_cast<uint8_t>(etf_type::Integer_Ext) };
			storeBits(newBuffer + 1, valueNew);
			writeString(newBuffer, std::size(newBuffer));
		}

		/// @brief Append a uint8_t value to the `stringReal` member.
		/// @param valueNew The uint8_t value to be appended.
		inline void appendUint8(const uint8_t valueNew) {
			uint8_t newBuffer[2]{ static_cast<uint8_t>(etf_type::Small_Integer_Ext), static_cast<uint8_t>(valueNew) };
			writeString(newBuffer, std::size(newBuffer));
		}

		/// @brief Append an int8_t value to the `stringReal` member.
		/// @param valueNew The int8_t value to be appended.
		inline void appendInt8(const int8_t valueNew) {
			uint8_t newBuffer[2]{ static_cast<uint8_t>(etf_type::Small_Integer_Ext), static_cast<uint8_t>(valueNew) };
			writeString(newBuffer, std::size(newBuffer));
		}

		/// @brief Append a boolean value to the `stringReal` member.
		/// @param data The boolean value to be appended.
		inline void appendBool(bool data) {
			if (data) {
				uint8_t newBuffer[6]{ static_cast<uint8_t>(etf_type::Small_Atom_Ext), static_cast<uint8_t>(4), 't', 'r', 'u', 'e' };
				writeString(newBuffer, std::size(newBuffer));
			} else {
				uint8_t newBuffer[7]{ static_cast<uint8_t>(etf_type::Small_Atom_Ext), static_cast<uint8_t>(5), 'f', 'a', 'l', 's', 'e' };
				writeString(newBuffer, std::size(newBuffer));
			}
		}

		/// @brief Append the format version to the `stringReal` member.
		inline void appendVersion() {
			uint8_t newBuffer[1]{ static_cast<uint8_t>(formatVersion) };
			writeString(newBuffer, std::size(newBuffer));
		}

		/// @brief Append a nil extension to the `stringReal` member.
		inline void appendNilExt() {
			uint8_t newBuffer[1]{ static_cast<uint8_t>(etf_type::Nil_Ext) };
			writeString(newBuffer, std::size(newBuffer));
		}

		/// @brief Append a nil value to the `stringReal` member.
		inline void appendNil() {
			uint8_t newBuffer[5]{ static_cast<uint8_t>(etf_type::Small_Atom_Ext), static_cast<uint8_t>(3), 'n', 'i', 'l' };
			writeString(newBuffer, std::size(newBuffer));
		}

		/// @brief Set the value of the `etf_serializer` based on the specified JSON type and arguments.
		/// @tparam typeNew The JSON type to set.
		/// @tparam value_types The types of arguments to forward.
		/// @param args The arguments to forward to the constructor.
		template<json_type typeNew, typename... value_types> inline void setValue(value_types&&... args) {
			destroyImpl();
			type = typeNew;
			if constexpr (typeNew == json_type::object_t) {
				allocator<object_type> alloc{};
				allocator_traits<object_type> allocTraits{};
				objectValue = allocTraits.allocate(alloc, 1);
				allocTraits.construct(alloc, objectValue, std::forward<value_types>(args)...);
			} else if constexpr (typeNew == json_type::array_t) {
				allocator<array_type> alloc{};
				allocator_traits<array_type> allocTraits{};
				arrayValue = allocTraits.allocate(alloc, 1);
				allocTraits.construct(alloc, arrayValue, std::forward<value_types>(args)...);
			} else if constexpr (typeNew == json_type::string_t) {
				allocator<string_type> alloc{};
				allocator_traits<string_type> allocTraits{};
				stringValue = allocTraits.allocate(alloc, 1);
				allocTraits.construct(alloc, stringValue, std::forward<value_types>(args)...);
			} else if constexpr (typeNew == json_type::float_t) {
				allocator<float_type> alloc{};
				allocator_traits<float_type> allocTraits{};
				floatValue = allocTraits.allocate(alloc, 1);
				allocTraits.construct(alloc, floatValue, std::forward<value_types>(args)...);
			} else if constexpr (typeNew == json_type::uint_t) {
				allocator<uint_type> alloc{};
				allocator_traits<uint_type> allocTraits{};
				uintValue = allocTraits.allocate(alloc, 1);
				allocTraits.construct(alloc, uintValue, std::forward<value_types>(args)...);
			} else if constexpr (typeNew == json_type::int_t) {
				allocator<int_type> alloc{};
				allocator_traits<int_type> allocTraits{};
				intValue = allocTraits.allocate(alloc, 1);
				allocTraits.construct(alloc, intValue, std::forward<value_types>(args)...);
			} else if constexpr (typeNew == json_type::bool_t) {
				allocator<bool_type> alloc{};
				allocator_traits<bool_type> allocTraits{};
				boolValue = allocTraits.allocate(alloc, 1);
				allocTraits.construct(alloc, boolValue, std::forward<value_types>(args)...);
			}
		}

		/// @brief Destroy the value of the specified JSON type and deallocate memory.
		/// @tparam typeNew The JSON type to destroy.
		template<json_type typeNew> inline void destroy() {
			if constexpr (typeNew == json_type::object_t) {
				allocator<object_type> alloc{};
				allocator_traits<object_type> allocTraits{};
				allocTraits.destroy(alloc, objectValue);
				alloc.deallocate(static_cast<object_type*>(objectValue), 1);
				objectValue = nullptr;
			} else if constexpr (typeNew == json_type::array_t) {
				allocator<array_type> alloc{};
				allocator_traits<array_type> allocTraits{};
				allocTraits.destroy(alloc, arrayValue);
				alloc.deallocate(static_cast<array_type*>(arrayValue), 1);
				arrayValue = nullptr;
			} else if constexpr (typeNew == json_type::string_t) {
				allocator<string_type> alloc{};
				allocator_traits<string_type> allocTraits{};
				allocTraits.destroy(alloc, stringValue);
				alloc.deallocate(static_cast<string_type*>(stringValue), 1);
				stringValue = nullptr;
			} else if constexpr (typeNew == json_type::float_t) {
				allocator<float_type> alloc{};
				allocator_traits<float_type> allocTraits{};
				allocTraits.destroy(alloc, floatValue);
				alloc.deallocate(static_cast<float_type*>(floatValue), 1);
				floatValue = nullptr;
			} else if constexpr (typeNew == json_type::uint_t) {
				allocator<uint_type> alloc{};
				allocator_traits<uint_type> allocTraits{};
				allocTraits.destroy(alloc, uintValue);
				alloc.deallocate(static_cast<uint_type*>(uintValue), 1);
				uintValue = nullptr;
			} else if constexpr (typeNew == json_type::int_t) {
				allocator<int_type> alloc{};
				allocator_traits<int_type> allocTraits{};
				allocTraits.destroy(alloc, intValue);
				alloc.deallocate(static_cast<int_type*>(intValue), 1);
				intValue = nullptr;
			} else if constexpr (typeNew == json_type::bool_t) {
				allocator<bool_type> alloc{};
				allocator_traits<bool_type> allocTraits{};
				allocTraits.destroy(alloc, boolValue);
				alloc.deallocate(static_cast<bool_type*>(boolValue), 1);
				boolValue = nullptr;
			}
		}

		/// @brief Compare the values of the `etf_serializer` for the specified JSON type.
		/// @tparam typeNew The JSON type to compare.
		/// @param other The other `etf_serializer` object to compare with.
		/// @return True if the values match, false otherwise.
		template<json_type typeNew> inline bool compareValues(const etf_serializer& other) const {
			if constexpr (typeNew == json_type::object_t) {
				return *objectValue == *other.objectValue;
			} else if constexpr (typeNew == json_type::array_t) {
				return *arrayValue == *other.arrayValue;
			} else if constexpr (typeNew == json_type::string_t) {
				return *stringValue == *other.stringValue;
			} else if constexpr (typeNew == json_type::float_t) {
				return *floatValue == *other.floatValue;
			} else if constexpr (typeNew == json_type::uint_t) {
				return *uintValue == *other.uintValue;
			} else if constexpr (typeNew == json_type::int_t) {
				return *intValue == *other.intValue;
			} else if constexpr (typeNew == json_type::bool_t) {
				return *boolValue == *other.boolValue;
			} else {
				return true;
			}
		}

		/// @brief Destroy the current value of `etf_serializer` based on its type.
		inline void destroyImpl() {
			switch (type) {
				case json_type::object_t: {
					destroy<json_type::object_t>();
					break;
				}
				case json_type::array_t: {
					destroy<json_type::array_t>();
					break;
				}
				case json_type::string_t: {
					destroy<json_type::string_t>();
					break;
				}
				case json_type::float_t: {
					destroy<json_type::float_t>();
					break;
				}
				case json_type::uint_t: {
					destroy<json_type::uint_t>();
					break;
				}
				case json_type::int_t: {
					destroy<json_type::int_t>();
					break;
				}
				case json_type::bool_t: {
					destroy<json_type::bool_t>();
					break;
				}
				case json_type::null_t: {
					break;
				}
				default: {
					break;
				}
			}
			type = json_type::null_t;
		}
	};

}