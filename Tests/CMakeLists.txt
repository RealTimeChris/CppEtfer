#	MIT License
#
#	Copyright (c) 2023 RealTimeChris
#
#	Permission is hereby granted, free of charge, to any person obtaining a copy of this 
#	software and associated documentation files (the "Software"), to deal in the Software 
#	without restriction, including without limitation the rights to use, copy, modify, merge, 
#	publish, distribute, sublicense, and/or sell copies of the Software, and to permit 
#	persons to whom the Software is furnished to do so, subject to the following conditions:
#
#	The above copyright notice and this permission notice shall be included in all copies or 
#	substantial portions of the Software.
#
#	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
#	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
#	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
#	FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
#	OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
#	DEALINGS IN THE SOFTWARE.
#
# CMakeLists.txt - The CMake script for building this library.
# Dec 17, 2022
# https://github.com/RealTimeChris/CppEtfer

cmake_minimum_required(VERSION 3.18)

set(TEST_NAME  "CppEtferTest")
set(PRODUCT_VERSION "1.0.0")

set(CMAKE_CXX_STANDARD 20)

add_executable("${TEST_NAME}" "CppEtfer.cpp")

include(FetchContent)
FetchContent_Declare(
   Jsonifier
   GIT_REPOSITORY https://github.com/RealTimeChris/Jsonifier.git
   GIT_TAG dev
)
FetchContent_MakeAvailable(Jsonifier)

set_target_properties(
	"${TEST_NAME}" PROPERTIES
	OUTPUT_NAME "CppEtfer"
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
)

target_link_libraries(
	"${TEST_NAME}" PUBLIC
	Jsonifier::Jsonifier
	CppEtfer::CppEtfer
)
