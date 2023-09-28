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
/// https://github.com/RealTimeChris/cpp_etfer
/// Feb 3, 2023
#pragma once

#include <concepts>
#include <string>
#include <chrono>
#include <tuple>

namespace cpp_etfer {

	class raw_json_data;

	template<typename value_type> class vector;

	template<typename value_type> struct core {};

	template<typename value_type> struct array {
		value_type parseValue;
	};

	template<typename value_type> array(value_type) -> array<std::decay_t<value_type>>;

	template<typename value_type> struct object {
		value_type parseValue;
	};

	template<typename value_type> object(value_type) -> object<std::decay_t<value_type>>;

	// From:
	// https://stackoverflow.com/questions/16337610/how-to-know-if-a-type-is-a-specialization-of-stdvector
	template<typename Test, template<typename...> typename Ref> struct is_specialization_v : std::false_type {};

	template<template<typename...> typename Ref, typename... Args> struct is_specialization_v<Ref<Args...>, Ref> : std::true_type {};

	template<typename value_type> using ref_unwrap = std::decay_t<std::decay_t<value_type>>;

	template<size_t I> using Tag = std::integral_constant<size_t, I>;

	template<size_t I> constexpr Tag<I> TagV{};

	template<typename value_type>
	concept range = requires(value_type& value) {
		typename value_type::value_type;
		requires !std::same_as<void, decltype(value.begin())>;
		requires !std::same_as<void, decltype(value.end())>;
	};

	template<typename value_type>
	concept stateless = std::is_empty_v<std::decay_t<value_type>>;

	template<typename value_type>
	concept map_subscriptable = requires(value_type value) {
		{ value[std::declval<typename value_type::key_type>()] } -> std::same_as<typename value_type::reference>;
	} || requires(value_type value) {
		{ value[std::declval<typename value_type::key_type>()] } -> std::same_as<typename value_type::const_reference>;
	};

	template<typename value_type>
	concept vector_subscriptable = requires(value_type value) {
		{ value[std::declval<uint64_t>()] } -> std::same_as<typename std::remove_reference_t<std::decay_t<value_type>>::reference>;
	} || requires(value_type value) {
		{ value[std::declval<uint64_t>()] } -> std::same_as<typename std::remove_reference_t<std::decay_t<value_type>>::const_reference>;
	};

	template<typename value_type>
	concept pair_t = requires(value_type value) {
		{ value.first } -> std::same_as<typename value_type::first_type>;
		{ value.second } -> std::same_as<typename value_type::second_type>;
	};

	template<typename value_type>
	concept has_size = requires(value_type value) {
		{ value.size() };
	};

	template<typename value_type>
	concept has_data = requires(value_type data) {
		{ data.data() } -> std::same_as<typename value_type::pointer>;
	} || requires(value_type data) {
		{ data.data() } -> std::same_as<typename value_type::const_pointer>;
	};

	template<typename value_type>
	concept indexable = stateless<std::decay_t<value_type>> || requires(value_type value) { value[Tag<0>()]; };

	template<typename value_type, typename u>
	concept same_as = std::same_as<value_type, u> && std::same_as<u, value_type>;

	template<typename value_type, typename u>
	concept other_than = !std::same_as<std::decay_t<value_type>, u>;

	template<typename tuple>
	concept base_list_tuple = requires() { typename ref_unwrap<tuple>::base_list; };

	template<typename value_type>
	concept ordered = requires(value_type const& object) {
		{ object <=> object };
	};

	template<typename value_type>
	concept equality_comparable = requires(value_type const& object) {
		{ object == object } -> same_as<bool>;
	};

	template<typename value_type>
	concept float_t = std::floating_point<std::decay_t<value_type>>;

	template<typename value_type>
	concept char_t = std::same_as<char, value_type> || std::same_as<wchar_t, value_type> || std::same_as<char8_t, value_type> || std::same_as<char16_t, value_type> ||
		std::same_as<char32_t, value_type>;

	template<typename value_type>
	concept bool_t = std::same_as<std::decay_t<value_type>, bool>;

	template<typename value_type>
	concept integer_t = std::integral<value_type> && !bool_t<value_type> || std::is_enum_v<value_type>;

	template<typename value_type>
	concept searchable_string_value = char_t<std::decay_t<value_type>> || indexable<std::decay_t<value_type>>;

	template<typename value_type>
	concept has_substr = requires(value_type value) {
		{ value.substr(std::declval<uint64_t>(), std::declval<uint64_t>()) };
	};

	template<typename value_type>
	concept string_t = has_substr<std::decay_t<value_type>> && has_data<std::decay_t<value_type>> && !std::same_as<char, std::decay_t<value_type>> && has_size<value_type> &&
		vector_subscriptable<value_type>;

	template<typename value_type>
	concept map_t = requires(value_type data) {
		typename value_type::mapped_type;
		typename value_type::key_type;
	} && range<std::decay_t<value_type>> && map_subscriptable<std::decay_t<value_type>>;

	template<typename value_type>
	concept has_emplace_back = requires(value_type data) {
		{ data.emplace_back(std::declval<typename value_type::value_type&&>()) } -> std::same_as<typename value_type::reference>;
	} || requires(value_type data) {
		{ data.emplace_back(std::declval<typename value_type::value_type&&>()) } -> std::same_as<typename value_type::const_reference>;
	};

	template<typename value_type>
	concept unique_ptr_t = requires(value_type other) {
		typename value_type::element_type;
		typename value_type::deleter_type;
		{ other.release() } -> std::same_as<typename value_type::pointer>;
	};

	template<typename value_type>
	concept has_find = requires(value_type c) {
		{ c.find(std::declval<const typename value_type::key_type&>()) } -> std::same_as<typename value_type::iterator>;
	} || requires(value_type c) {
		{ c.find(std::declval<const typename value_type::key_type&>()) } -> std::same_as<typename value_type::const_iterator>;
	};

	template<typename value_type>
	concept has_excluded_keys = requires(value_type value) {
		{ value.excludedKeys };
	};

	template<typename value_type>
	concept null_t = std::is_null_pointer_v<value_type>;

	template<typename value_type>
	concept has_resize = requires(value_type value) { value.resize(std::declval<uint64_t>()); };

	template<typename value_type> inline auto dataPtr(value_type& buffer) {
		if constexpr (has_resize<std::decay_t<value_type>>) {
			return buffer.data();
		} else {
			return buffer;
		}
	}

	template<typename value_type>
	concept cpp_etfer_t = requires { core<std::decay_t<value_type>>::parseValue; };

	struct empty_val {
		static constexpr std::tuple<> parseValue{};
	};

	template<typename value_type> constexpr auto coreWrapperV = [] {
		if constexpr (cpp_etfer_t<std::decay_t<value_type>>) {
			return core<std::decay_t<value_type>>::parseValue;
		} else {
			return empty_val{};
		}
	}();

	template<typename value_type> constexpr auto coreV = coreWrapperV<std::decay_t<value_type>>.parseValue;

	template<typename value_type> using core_t = std::decay_t<decltype(coreV<std::decay_t<value_type>>)>;

	template<typename value_type> using core_wrapper_t = std::decay_t<decltype(coreWrapperV<std::decay_t<value_type>>)>;

	template<typename value_type>
	concept cpp_etfer_array_t = cpp_etfer_t<std::decay_t<value_type>>;

	template<typename value_type>
	concept cpp_etfer_object_t = cpp_etfer_t<std::decay_t<value_type>>;

	template<typename value_type>
	concept object_t = map_t<std::decay_t<value_type>> || cpp_etfer_object_t<std::decay_t<value_type>>;

	template<typename value_type>
	concept enum_t = std::is_enum_v<std::decay_t<value_type>>;

	template<typename value_type>
	concept tuple_t = requires(value_type value) { std::tuple_size<std::decay_t<value_type>>::value; } && !range<std::decay_t<value_type>>;

	template<typename value_type>
	concept array_tuple_t = cpp_etfer_array_t<std::decay_t<value_type>> || tuple_t<std::decay_t<value_type>>;

	template<typename value_type>
	concept array_t = ( !map_t<std::decay_t<value_type>> && vector_subscriptable<std::decay_t<value_type>> && has_data<std::decay_t<value_type>> &&
						  has_emplace_back<std::decay_t<value_type>> )&&!tuple_t<std::decay_t<value_type>> &&
			!has_substr<std::decay_t<value_type>> && requires(value_type data) { typename value_type::value_type; } ||
		is_specialization_v<value_type, vector>::value;

	template<typename value_type>
	concept raw_array_t = ( ( !map_t<std::decay_t<value_type>> && vector_subscriptable<std::decay_t<value_type>> && has_data<std::decay_t<value_type>> &&
								!has_emplace_back<std::decay_t<value_type>> )&&!tuple_t<std::decay_t<value_type>> &&
							  !has_substr<std::decay_t<value_type>> && !has_resize<std::decay_t<value_type>> ) ||
		std::is_array_v<std::decay_t<value_type>>;

	template<typename value_type>
	concept vector_like = has_resize<std::decay_t<value_type>> && vector_subscriptable<std::decay_t<value_type>> && has_data<std::decay_t<value_type>>;

	template<typename value_type>
	concept core_type = is_specialization_v<core<std::decay_t<value_type>>, core>::value;

	template<typename value_type>
	concept time_type = is_specialization_v<std::chrono::duration<std::decay_t<value_type>>, std::chrono::duration>::value;

	template<typename value_type>
	concept char_type = requires(value_type) { sizeof(value_type) == 1; };

	template<typename value_type>
	concept num_t = ( std::integral<std::decay_t<value_type>> || enum_t<value_type> )&&!bool_t<value_type>;

}// namespace cpp_etfer_internal
