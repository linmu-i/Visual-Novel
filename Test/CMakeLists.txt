cmake_minimum_required (VERSION 3.12)

project(EbbGlow_Test VERSION 0.0.0)
add_executable(EbbGlow_Test)

target_compile_features(EbbGlow_Test PRIVATE cxx_std_20)

target_link_libraries(EbbGlow_Test PUBLIC EbbGlow)

target_compile_options(EbbGlow_Test PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-finput-charset=UTF-8 -fexec-charset=UTF-8>
)

set_property(TARGET EbbGlow_Test
             PROPERTY VS_DEBUGGER_WORKING_DIRECTORY 
             "${CMAKE_SOURCE_DIR}/working_dir")

target_include_directories(EbbGlow_Test PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_sources(EbbGlow_Test PRIVATE
	"src/Test.cpp"
)