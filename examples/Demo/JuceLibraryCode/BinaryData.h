/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   ballon_bmp;
    const int            ballon_bmpSize = 409654;

    extern const char*   ballon_8bit_bmp;
    const int            ballon_8bit_bmpSize = 103478;

    extern const char*   IMG_1883_JPG;
    const int            IMG_1883_JPGSize = 1218029;

    extern const char*   pencils_jpeg;
    const int            pencils_jpegSize = 254617;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Number of elements in the namedResourceList array.
    const int namedResourceListSize = 4;

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes) throw();
}
