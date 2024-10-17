#!/bin/bash

# Usage example:
#
# The following command line:
#   './GenerateBigXRCFile.sh' 2000 >'MyInputFileReferencingManyResources.xrc'
# will generate an XRC file referencing 2000 small PNG images.
#
# Compiling this XRC file using `wxrc` and further compiling the generated C++ source code
# can help us tune our code and avoid slow C++ compilation process
# (when thousands of input resource files have been given as input).

function GenerateExampleImages
{
    set -e -o pipefail

    local folder_images='ExampleImages'
    mkdir -p "${folder_images}/"

    local i
    local n="${1:-10000}"

    printf '<?xml version="1.0" encoding="UTF-8"?>\n'
    printf '<resource xmlns="http://www.wxwidgets.org/wxxrc" version="2.5.3.0">\n'
    for ((i = 0; i < n; ++i)); do
        local pathname_image; printf -v pathname_image "%s/Example_%04Xh.png" "${folder_images}" "$((i))"

        rm -f "${pathname_image}"
        printf >>"${pathname_image}" \
            '%s' \
            "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A\x00\x00\x00\x0D\x49\x48\x44\x52" \
            "\x00\x00\x00\x10\x00\x00\x00\x10\x08\x02\x00\x00\x00\x90\x91\x68" \
            "\x36\x00\x00\x00\x09\x70\x48\x59\x73\x00\x00\x0E\xC4\x00\x00\x0E" \
            "\xC4\x01\x95\x2B\x0E\x1B\x00\x00\x00\x1A\x49\x44\x41\x54\x28\xCF" \
            "\x63\x6C\x60\xF8\xCF\x40\x0A\x60\x62\x20\x11\x8C\x6A\x18\xD5\x30" \
            "\x74\x34\x00\x00\xC5\xBF\x01\x9F\x22\x91\xFF\xBD\x00\x00\x00\x00"
            "\x49\x45\x4E\x44\xAE\x42\x60\x82"

        printf '  <object class="wxBitmap" name="Cat_%04Xh">%s</object>\n' "$((i))" "${pathname_image}"
    done
    printf '</resource>\n'
}

GenerateExampleImages "$@"
