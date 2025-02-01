// #ifndef LITERA_H
// #define LITERA_H

// #include <cstdint>
// #include <cstddef>

// namespace Codex {

//     constexpr const uint8_t MAX_LITERA_8 = 255;
//     constexpr const uint16_t MAX_LITERA_16 = 65535;
//     constexpr const uint32_t MAX_LITERA_32 = 4294967295;

//     class Litera {

//         public:

//             Litera();
//             ~Litera();

//         protected:
//             Litera(size_t strSize, unsigned char* str);

//             unsigned char* str;

//     };

//     class Litera_8 : public Litera {
//         public:
//             Litera_8();
//             Litera_8(size_t size, unsigned char* str);
//         private:
//             uint8_t size;
//     };

//     class Litera_16 : public Litera {
//         public:
//             Litera_16(size_t size, unsigned char* str);
//         private:
//             uint16_t size;
//     };

//     class Litera_32 : public Litera {
//         public:
//             Litera_32(size_t size, unsigned char* str);
//         private:
//             uint32_t size;
//     };
// }

// #endif // LITERA_H