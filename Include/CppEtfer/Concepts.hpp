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
/// \file Concepts.hpp

#pragma once

#include <concepts>
#include <cstdint>

namespace CppEtfer {

	/// @brief Concept for types that are subscriptable with [] operator.
	template<typename value_type>
	concept map_subscriptable = requires(value_type data, uint64_t valueNew) {
		{ data[std::declval<typename value_type::key_type>()] } -> std::same_as<typename value_type::const_reference>;
	} || requires(value_type data, uint64_t valueNew) {
		{ data[std::declval<typename value_type::key_type>()] } -> std::same_as<typename value_type::reference>;
	};

	/// @brief Concept for types that have an emplace_back method.
	template<typename value_type>
	concept has_emplace = requires(value_type data) {
		{ data.emplace(std::declval<typename value_type::reference&&>()) } -> std::same_as<typename value_type::reference>;
	};

	/// @brief Concept for types that have a begin and end methods.
	template<typename value_type>
	concept has_range = requires(value_type data) {
		{ data.begin() } -> std::same_as<typename value_type::iterator>;
		{ data.end() } -> std::same_as<typename value_type::iterator>;
	};

	/// @brief Concept for object (associative container) types.
	template<typename value_type>
	concept object_t = requires(value_type data) {
		typename value_type::mapped_type;
		typename value_type::key_type;
	} && has_range<value_type> && map_subscriptable<value_type> && has_emplace<value_type>;

	/// @brief Concept for types that have a data method.
	template<typename value_type>
	concept has_data = requires(value_type data) {
		{ data.data() } -> std::same_as<typename value_type::const_pointer>;
	} || requires(value_type data) {
		{ data.data() } -> std::same_as<typename value_type::pointer>;
	};
	
	/// @brief Concept for types that are subscriptable with [] operator.
	template<typename value_type>
	concept vector_subscriptable = requires(value_type data) {
		{ data[std::declval<uint64_t>()] } -> std::same_as<typename value_type::const_reference>;
	} || requires(value_type data, uint64_t valueNew) {
		{ data[std::declval<uint64_t>()] } -> std::same_as<typename value_type::reference>;
	};

	/// @brief Concept for types that have a substr method.
	template<typename value_type>
	concept has_substr = requires(value_type data) {
		{ data.substr(std::declval<uint64_t>(), std::declval<uint64_t>()) } -> std::same_as<std::decay_t<value_type>>;
	};

	/// @brief Concept for string types.
	template<typename value_type>
	concept string_t = has_substr<std::decay_t<value_type>> && has_data<std::decay_t<value_type>> && vector_subscriptable<std::decay_t<value_type>>;

	/// @brief Concept for boolean types.
	template<typename value_type>
	concept bool_t = std::same_as<std::decay_t<value_type>, bool> ;

	/// @brief Concept for floating-point types.
	template<typename value_type>
	concept float_t = std::floating_point<std::decay_t<value_type>>;
	
	/// @brief Concept for integer types.
	template<typename value_type>
	concept integer_t = std::integral<std::decay_t<value_type>> && !bool_t<std::decay_t<value_type>>;

	/// @brief Concept for enumerator types.
	template<typename value_type>
	concept enum_t = std::is_enum_v<std::decay_t<value_type>>;

	/// @brief Concept for signed integral types.
	template<typename value_type>
	concept signed_t = std::signed_integral<std::decay_t<value_type>>;

	/// @brief Concept for unsigned integral types.
	template<typename value_type>
	concept unsigned_t = std::unsigned_integral<std::decay_t<value_type>>;

	/// @brief Concept for null types.
	template<typename value_type>
	concept null_t = std::same_as<std::decay_t<value_type>, std::nullptr_t>;

	/// @brief Concept for types that have a resize method.
	template<typename value_type>
	concept has_resize = requires(value_type data) {
		{ data.resize(std::declval<uint64_t>()) };
	};

	/// @brief Concept for types that have an emplace_back method.
	template<typename value_type>
	concept has_emplace_back = requires(value_type data) {
		{ data.emplace_back(std::declval<typename value_type::reference&&>()) } -> std::same_as<typename value_type::reference>;
	};	

	/// @brief Concept for array types.
	template<typename value_type>
	concept array_t =
		has_range<value_type> && has_resize<std::decay_t<value_type>> && has_emplace_back<std::decay_t<value_type>> && vector_subscriptable<std::decay_t<value_type>> &&
			requires(value_type other) { typename value_type::value_type; };

	

	
}