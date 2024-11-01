#
# SPDX-License-Identifier: BSD-3-Clause
#
# Copyright © 2024 Keith Packard
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided
#    with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.
#

if(NOT CONFIG_GLIBCXX_LIBCPP_USE_MODULE)
  return()
endif()

cmake_minimum_required(VERSION 3.14.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(libstdc++ CXX)

#
# Generate a subset of the compile options, removing Zephyr-specific settings
# which aren't appropriate for building libstdc++. This replicates the
# functionality of zephyr_get_compile_options_for_lang_as_string with the
# addition of the filter step.
#
function(libstdcxx_get_compile_options_for_lang_as_string lang i)
  get_property(all_options TARGET zephyr_interface PROPERTY INTERFACE_COMPILE_OPTIONS)
  process_flags(${lang} all_options all_option_list)
  set(option_list "")
  #
  # Filter out imacros, warning and rtti settings
  #
  set(skip_next 0)
  foreach(option ${all_option_list})
    if (skip_next)
      set(skip_next 0)
    elseif(option MATCHES "compiler,imacros" OR option MATCHES "compiler,include")
      set(skip_next 1)
    elseif(option MATCHES "compiler-cpp,no_rtti" OR
	option MATCHES "compiler-cpp,dialect" OR
	option MATCHES "compiler,warning" OR
	option MATCHES "compiler-cpp,warning")
    else()
      list(APPEND option_list ${option})
    endif()
  endforeach()

  #
  # The picolibc module doesn't get configured until after this one,
  # so we don't get its include directory in cxx_system_includes above.
  # Stick it in cxx_options as a convenient place
  #
  if(CONFIG_PICOLIBC_USE_MODULE)
    list(APPEND option_list "-isystem" "$<TARGET_PROPERTY:c,INTERFACE_SYSTEM_INCLUDE_DIRECTORIES>")
  endif()

  list(APPEND option_list "-D_DEFAULT_SOURCE")

  string(REPLACE ";" "$<SEMICOLON>" genexp_options "${option_list}")

  set(args_DELIMITER " ")

  set(gen_option_list "$<JOIN:${genexp_options},${args_DELIMITER}>")

  convert_list_of_flags_to_string_of_flags(gen_option_list options)
  set(${i} ${options} PARENT_SCOPE)
endfunction()

zephyr_get_system_include_directories_for_lang_as_string(CXX cxx_system_includes)
zephyr_get_compile_definitions_for_lang_as_string(       CXX cxx_definitions)
libstdcxx_get_compile_options_for_lang_as_string(        CXX cxx_options)

zephyr_get_system_include_directories_for_lang_as_string(C c_system_includes)
zephyr_get_compile_definitions_for_lang_as_string(       C c_definitions)
libstdcxx_get_compile_options_for_lang_as_string(        C c_options)

set(LIBSTDCXX_URL "${CONFIG_LIBSTDCXX_MODULE_DIR}/${CONFIG_LIBSTDCXX_MODULE_BASENAME}.tar.gz")

include(ExternalProject)

set(BUILD_TOOLS
	"CFLAGS_FOR_TARGET=${c_definitions} ${c_system_includes} ${c_options} ${CMAKE_REQUIRED_FLAGS}"
	"CXXFLAGS_FOR_TARGET=${cxx_definitions} ${cxx_system_includes} ${cxx_options} ${CMAKE_REQUIRED_FLAGS}"
	XGCC_FLAGS_FOR_TARGET=
	CC_FOR_TARGET=${CMAKE_C_COMPILER}
	GCC_FOR_TARGET=${CMAKE_C_COMPILER}
	CXX_FOR_TARGET=${CMAKE_CXX_COMPILER}
	RAW_CXX_FOR_TARGET=${CMAKE_CXX_COMPILER}
	AR_FOR_TARGET=${CMAKE_AR}
	AS_FOR_TARGET=${CMAKE_AS}
	LD_FOR_TARGET=${CMAKE_LINKER}
	NM_FOR_TARGET=${CMAKE_NM}
	OBJDUMP_FOR_TARGET=${CMAKE_OBJDUMP}
	RANLIB_FOR_TARGET=${CMAKE_RANLIB}
	READELF_FOR_TARGET=${CMAKE_READELF}
	STRIP_FOR_TARGET=${CMAKE_STRIP})

set(CONFIGURE_FLAGS
        --enable-bootstrap
	--disable-multilib
	--disable-decimal-float
	--disable-libffi
	--disable-libgomp
	--disable-libmudflap
	--disable-libquadmath
	--disable-libssp
	--disable-libstdcxx-pch
	--disable-nls
	--disable-shared
	--disable-threads
	--enable-tls
	--enable-cstdio=stdio_pure
	--disable-plugin
	--disable-libstdcxx-verbose
	--mandir=/usr/share/man
	--with-system-zlib
	--with-gnu-as
	--with-gnu-ld
	--with-newlib
	--with-headers=yes
	--without-included-gettext
	"--with-mpc-include=${CMAKE_CURRENT_SOURCE_DIR}/include"
	"--with-mpc-lib=${CMAKE_CURRENT_BINARY_DIR}/lib"
	"--with-mpfr-include=${CMAKE_CURRENT_SOURCE_DIR}/include"
	"--with-mpfr-lib=${CMAKE_CURRENT_BINARY_DIR}/lib"
	"--with-gmp-include=${CMAKE_CURRENT_SOURCE_DIR}/include"
	"--with-gmp-lib=${CMAKE_CURRENT_BINARY_DIR}/lib"
	"--with-gxx-include-dir=${PROJECT_BINARY_DIR}/libstdc++/${CROSS_COMPILE_TARGET}/include"
	"--with-host-libstdcxx=-static-libgcc -Wl,-Bstatic,-lstdc++,-Bdynamic -lm"
	--enable-languages=c,c++
	--disable-option-checking
	--target=${CROSS_COMPILE_TARGET})

add_subdirectory(lib)

ExternalProject_Add(libstdcxx_project
  URL "${LIBSTDCXX_URL}"
  PREFIX libstdc++
  SOURCE_DIR gcc
  BINARY_DIR build
  DEPENDS gmp mpfr mpc
  CONFIGURE_COMMAND ../gcc/configure --prefix=<INSTALL_DIR> ${CONFIGURE_FLAGS} ${BUILD_TOOLS}
  BUILD_COMMAND make all-target-libstdc++-v3 ${BUILD_TOOLS}
  INSTALL_COMMAND make -C ${CROSS_COMPILE_TARGET}/libstdc++-v3 install)

zephyr_include_directories(${PROJECT_BINARY_DIR}/libstdc++/${CROSS_COMPILE_TARGET}/include)
zephyr_include_directories(${PROJECT_BINARY_DIR}/libstdc++/${CROSS_COMPILE_TARGET}/include/${CROSS_COMPILE_TARGET})
set_property(TARGET linker PROPERTY c++_library ${PROJECT_BINARY_DIR}/libstdc++/${CROSS_COMPILE_TARGET}/lib/libstdc++.a)
add_dependencies(zephyr_interface libstdcxx_project)
