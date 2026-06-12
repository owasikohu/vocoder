# Converts a file into a C header containing a byte array, so webview resources
# can be served from memory (works for native and WASM/WCLAP builds alike).
#
# Usage:
#   cmake -DINPUT=<file> -DOUTPUT=<file.hxx> -DVARNAME=<ident> -P embed-resource.cmake
#
# Produces:  const unsigned char <VARNAME>[] = { 0x.., 0x.., ... };
# (no trailing NUL — consumers use sizeof(<VARNAME>) for the length)

file(READ "${INPUT}" hex HEX)
string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," body "${hex}")
file(WRITE "${OUTPUT}" "const unsigned char ${VARNAME}[] = {\n${body}\n};\n")
