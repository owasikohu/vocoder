cmake_minimum_required(VERSION 3.24)

# ----------------------------------------------------------------------------
# Vocoder — a Faust DSP shipped as a cross-platform CLAP plugin with a webview UI
#
#   Native CLAP (Win/macOS/Linux):  see scripts/build-native.sh
#   WCLAP (WebAssembly):            see scripts/build-wclap.sh
# ----------------------------------------------------------------------------

project(vocoder-clap VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Everything is linked into a shared module (.clap), so build all targets
# (incl. fetched deps) as position-independent code.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(CMAKE_CXX_STANDARD 20) # MSVC is strict about designated initialisers
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
if (APPLE)
	set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15)
endif()

if (EMSCRIPTEN)
	# Shared memory (implies atomics) — needed by the clap-wrapper WCLAP runtime.
	add_compile_options(-sSHARED_MEMORY)
	add_link_options(-sSHARED_MEMORY -sIMPORTED_MEMORY)
endif()

include(FetchContent)

# ---- dependencies ----------------------------------------------------------

set(CLAP_WRAPPER_DOWNLOAD_DEPENDENCIES TRUE)
set(CLAP_WRAPPER_DONT_ADD_TARGETS TRUE)
set(CLAP_WRAPPER_OUTPUT_NAME clap-wrapper-target)

FetchContent_Declare(clap
	GIT_REPOSITORY https://github.com/free-audio/clap.git
	GIT_TAG 1.2.7
	GIT_SHALLOW ON)
FetchContent_Declare(clap-wrapper
	GIT_REPOSITORY https://github.com/free-audio/clap-wrapper.git
	GIT_TAG 2a4da4db3665ca18dd0830c5dec4303d3765efec)
	# (no GIT_SHALLOW: a specific non-tip commit can't be fetched by a shallow clone)
FetchContent_Declare(webview-gui
	GIT_REPOSITORY https://github.com/geraintluff/webview-gui.git
	GIT_TAG main
	GIT_SUBMODULES_RECURSE ON)

FetchContent_MakeAvailable(clap clap-wrapper webview-gui)

# ---- embed webview resources (ui/*) as byte arrays -------------------------

set(GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/ui)
file(MAKE_DIRECTORY ${GEN_DIR})

set(EMBED_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/cmake/embed-resource.cmake)
set(RESOURCE_HXX "")
function(embed_ui name var)
	add_custom_command(
		OUTPUT ${GEN_DIR}/${name}.hxx
		COMMAND ${CMAKE_COMMAND}
			-DINPUT=${CMAKE_CURRENT_LIST_DIR}/plugin/ui/${name}
			-DOUTPUT=${GEN_DIR}/${name}.hxx
			-DVARNAME=${var}
			-P ${EMBED_SCRIPT}
		DEPENDS ${CMAKE_CURRENT_LIST_DIR}/plugin/ui/${name} ${EMBED_SCRIPT}
		VERBATIM)
	set(RESOURCE_HXX ${RESOURCE_HXX} ${GEN_DIR}/${name}.hxx PARENT_SCOPE)
endfunction()

embed_ui(index.html  index_html)
embed_ui(app.js      app_js)
embed_ui(style.css   style_css)
embed_ui(cbor.min.js cbor_min_js)
add_custom_target(vocoder_resources DEPENDS ${RESOURCE_HXX})

# ---- plugin implementation (static lib) ------------------------------------

add_library(vocoder_impl STATIC
	${CMAKE_CURRENT_LIST_DIR}/plugin/src/plugins.cpp
	${CMAKE_CURRENT_LIST_DIR}/plugin/src/vocoder-plugin.cpp)

add_dependencies(vocoder_impl vocoder_resources)

# The static lib is linked into a shared module (.clap), so it needs PIC.
set_target_properties(vocoder_impl PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(vocoder_impl PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/plugin/src
	${CMAKE_CURRENT_LIST_DIR}/plugin/faust
	${CMAKE_CURRENT_LIST_DIR}/third_party
	${CMAKE_CURRENT_LIST_DIR}/third_party/faust
	${CMAKE_CURRENT_BINARY_DIR}/generated)

target_compile_definitions(vocoder_impl PRIVATE
	CLAP_BUNDLE_ID="com.owasikohu.vocoder"
	CLAP_BUNDLE_VERSION="${CMAKE_PROJECT_VERSION}")

target_link_libraries(vocoder_impl PUBLIC clap webview-gui)

# ---- produce the CLAP (+ WCLAP) bundles ------------------------------------

make_clapfirst_plugins(
	TARGET_NAME vocoder
	IMPL_TARGET vocoder_impl

	OUTPUT_NAME "Vocoder"
	ENTRY_SOURCE "${CMAKE_CURRENT_LIST_DIR}/plugin/src/clap_entry.cpp"
	RESOURCE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/plugin/ui"

	BUNDLE_IDENTIFIER "com.owasikohu.vocoder"
	BUNDLE_VERSION ${CMAKE_PROJECT_VERSION}

	PLUGIN_FORMATS CLAP WCLAP
	COPY_AFTER_BUILD FALSE
)
