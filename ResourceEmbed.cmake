# MACRO to embed a single file into a C++ header.
# ARGUMENTS: INPUT_FILE_PATH, VARIABLE_NAME
function(embed_resource input_file output_header_file variable_name)
    # Read the binary data of the file into a CMake variable.
    file(READ "${input_file}" RESOURCE_DATA HEX)

    # 1. Get size
    string(LENGTH "${RESOURCE_DATA}" HEX_LENGTH)
    math(EXPR FILE_SIZE "${HEX_LENGTH} / 2")

    # 2. Format Hex: Convert "aabbcc" into "0xaa, 0xbb, 0xcc, "
    # We match every 2 characters and append '0x' and ', '
    string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1, " FORMATTED_DATA "${RESOURCE_DATA}")

    # 3. Create the Header Content
    set(HEADER_CONTENT "/* Automatically generated from ${INPUT_FILE} */\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}#pragma once\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}#include <cstddef>\n\n")

    # Using 'inline' ensures no linker errors if included in multiple .cpp files
    set(HEADER_CONTENT "${HEADER_CONTENT}namespace resources {\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}    inline const unsigned char ${variable_name}_data[] = {\n        ${FORMATTED_DATA}\n    };\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}    inline const std::size_t ${variable_name}_size = ${FILE_SIZE};\n")
    set(HEADER_CONTENT "${HEADER_CONTENT}}\n")

    # 4. Write to file
    file(WRITE "${output_header_file}" "${HEADER_CONTENT}")
endfunction()

embed_resource("${INPUT_FILE}" "${OUTPUT_HEADER}" "${VAR_NAME}")