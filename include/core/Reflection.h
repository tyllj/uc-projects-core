#include <stdint.h>

namespace core { namespace reflection {
    namespace impl
    {
        static constexpr unsigned int FRONT_SIZE = sizeof("core::impl::reflection::GetTypeNameHelper<") - 1u;
        static constexpr unsigned int BACK_SIZE = sizeof(">::getTypeName") - 1u;

        constexpr char* strcat(char* dest, const char* src, size_t count) {
            if (dest != nullptr && src != nullptr && count != 0) {
                char* d = dest;
                for (; *d != '\0'; ++d) {
                }
                do {
                    if ((*d = *src++) == '\0') {
                        break;
                    }
                    d++;
                } while (--count != 0);
                *d = '\0';
            }
            return dest;
        }

        template <typename T>
        struct GetTypeNameHelper {
            static constexpr char* getTypeName() {
                const size_t size = sizeof(__FUNCTION__) - FRONT_SIZE - BACK_SIZE;
                char typeName[size] = {};
                return strcat(typeName, __FUNCTION__ + FRONT_SIZE, size - 1u);
            }
        };
    }

    template <typename T>
    constexpr char* getTypeName() {
        return impl::GetTypeNameHelper<T>::getTypeName();
    }
}}