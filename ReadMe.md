# CppEtfer
A couple of classes for parsing from/serializing into Erlang's External Term Format. Currently only the types used by the Discord API are supported, but if you would like me to implement more just let me know and I will do so.

## Compiler Support
----
![MSVC_20922](https://img.shields.io/github/actions/workflow/status/RealTimeChris/CppEtfer/MSVC_2022-Windows.yml?style=plastic&logo=microsoft&logoColor=green&label=MSVC_2022&labelColor=pewter&color=blue)
![CLANG_18](https://img.shields.io/github/actions/workflow/status/RealTimeChris/CppEtfer/CLANG_18-Ubuntu.yml?style=plastic&logo=linux&logoColor=green&label=CLANG_18&labelColor=pewter&color=blue)
![GCC_13](https://img.shields.io/github/actions/workflow/status/RealTimeChris/CppEtfer/GCC_13-MacOS.yml?style=plastic&logo=apple&logoColor=green&label=GCC_13&labelColor=pewter&color=blue)

## Operating System Support
----
![Windows](https://img.shields.io/github/actions/workflow/status/RealTimeChris/CppEtfer/MSVC_2022-Windows.yml?style=plastic&logo=microsoft&logoColor=green&label=Windows&labelColor=pewter&color=blue)
![Linux](https://img.shields.io/github/actions/workflow/status/RealTimeChris/CppEtfer/CLANG_18-Ubuntu.yml?style=plastic&logo=linux&logoColor=green&label=Linux&labelColor=pewter&color=blue)
![Mac](https://img.shields.io/github/actions/workflow/status/RealTimeChris/CppEtfer/GCC_13-MacOS.yml?style=plastic&logo=apple&logoColor=green&label=MacOS&labelColor=pewter&color=blue)


## Installation (CMake-FetchContent)
- Requirements:
	- CMake 3.18 or later.
	- A C++20 or later compiler.
- Steps:   Add the following to your CMakeLists.txt build script.
```cpp
include(FetchContent)

FetchContent_Declare(
   CppEtfer
   GIT_REPOSITORY https://github.com/RealTimeChris/CppEtfer.git
   GIT_TAG main
)
FetchContent_MakeAvailable(CppEtfer)

target_link_libraries("${PROJECT_NAME}" PRIVATE CppEtfer)
```

## Usage - Parsing Directly to Data
1. Create a specialization of the `cpp_etfer_core` struct for the class which you would like to parse into:
```cpp
template<> struct cpp_etfer::core<ActivityData> {
	using value_type				 = ActivityData;
	static constexpr auto parseValue = createObject("name", &value_type::name, "type", &value_type::type, "state", &value_type::state);
};

template<> struct cpp_etfer::core<UpdatePresenceData> {
	using value_type = UpdatePresenceData;
	static constexpr auto parseValue = createObject("afk", &value_type::afk, "activities", &value_type::activities, "since", &value_type::since, "status", &value_type::statusReal);
};
```
2. Pass in an instance of the structure which you would like to parse into, into the `cpp_etfer::etf_parser::parseEtfToData()` function, along with a string containing the data to be parsed:
```cpp
auto newString = updatePresenceData.operator cpp_etfer::etf_serializer().operator std::basic_string<uint8_t>();

cpp_etfer::etf_parser parser{};
updatePresenceData.activities.clear();
parser.parseEtfToData(updatePresenceData, newString);
```
3. Use the data.

## Usage - Parsing to Json Data
1. Instantiate an instance of etf_parser.
2. Pass to its method `parseEtfToJson` a string of some sort containing the data to be parsed.
3. Collect the output of the function and use it.
```cpp
	std::basic_string<char> guildString{};
	CppEtfer::etf_parser parser{};
	auto newData = parser.parseEtfToJson(guildString);
	std::cout << "Json data: " << newData << std::endl;
```

## Usage - Serializing
