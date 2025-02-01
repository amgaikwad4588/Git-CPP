// #include <Codex/litera.h>
// #include <cstring>
// #include <stdexcept>

// namespace Codex {

//     /**
//      * LITERA MAIN OBJECT
//      */

//     Litera::Litera() : str {nullptr} {
//         str = new unsigned char[1];
//         str[0] = '\x00';
//     }

//     Litera::Litera(size_t strSize, unsigned char* str) : str {nullptr} {
//         this->str = new unsigned char[strSize + 1];
//         std::memcpy(this->str, str, strSize);
//         this->str[strSize] = '\x00'; // Null-terminate
//     }

//     Litera::~Litera() {
//         delete [] str;
//     }

//     /**
//      * LITERA_8
//      */
//     Litera_8::Litera_8() : Litera(), size {} {}
//     Litera_8::Litera_8(size_t size, unsigned char* str) :
//         Litera(size, str), size {size} {
//     }

//     /**
//      * LITERA_16
//      */

//     Litera_16::Litera_16(size_t size, unsigned char* str) :
//         Litera(size, str), size {size} {
//     }

//     /**
//      * LITERA_32
//      */

//     Litera_32::Litera_32(size_t size, unsigned char* str) :
//         Litera(size, str), size {size} {
//     }


//     Litera* LiteraFactory(unsigned char* str) {

//         if (!str) {
//             throw std::invalid_argument("Input string is null.");
//         }

//         const char* cstr = reinterpret_cast<const char*>(str);
//         size_t strSize = std::strlen(cstr);

//         if (MAX_LITERA_8 >= strSize) return new Litera_8(strSize, str);
//         if (MAX_LITERA_16 >= strSize) return new Litera_16(strSize, str);

//         return new Litera_32(strSize, str);

//     }


// }