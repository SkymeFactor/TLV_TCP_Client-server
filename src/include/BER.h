#pragma once
#include <iostream>
#include <string.h>

namespace easyBER {

    template <class T>
    class BERmsg final {
    public:
        explicit BERmsg<T> (T *string, size_t &&size) {
            this->length = size;
            this->message = reinterpret_cast<uint8_t *>(string);
            this->encodeLength();
        }

        explicit BERmsg<T> (char *string) {
            this->tag = reinterpret_cast<uint8_t*>(string)[0];
            this->length = 0;
            decodeLength(string);
        }

        ~BERmsg<T> () {};

        static size_t getTotalLengthFromHeader (char *header) {
            uint8_t tag = header[0];
            size_t length = 0;
            int add_len = 0;

            if ( (tag & 0x80) == 0 ) {
                length = tag & 0x7F;
            } else {
                add_len = tag & 0x7F;
                for (int i = 0; i < add_len; i++) {
                    length |= static_cast<uint8_t>(header[i + 1]);
                    length = length << (8 * (add_len - i - 1));
                }
            }

            return length + add_len + 1;
        }

        char *translateToCString () {
            
            char *result;
            int add_len = 0;

            if ( (this->tag & 0x80) != 0 )
                add_len = this->tag & 0x7F;
            result = new char[this->length + add_len + 1];

            result[0] = static_cast<char>(this->tag);

            if (this->length_array != nullptr)
                strncpy(result + 1, reinterpret_cast<char*>(this->length_array), add_len);
            strncpy(result + add_len + 1, reinterpret_cast<char*>(this->message), length);
            result[this->length + add_len + 1] = '\0';

            return result;
        }

        T *getMessage () {
            return reinterpret_cast<T*>(this->message);
        }

        size_t getMessageLength () {
            size_t size = 0;
            if ( (this->tag & 0x80) == 0 ) {
                size = this->tag & 0x7F;
            } else {
                int add_len = this->tag & 0x7F;
                for (int i = add_len; i > 0; i--)
                    size |= (length_array[add_len - i] << (8 * (i - 1)));
            }
            return size;
        }

    private:
        uint8_t tag;
        size_t length;
        uint8_t *length_array = nullptr;
        uint8_t *message = nullptr;

        void encodeLength () {
            if (this->length < 128)
                this->tag = this->length & 0x7F;
            else {
                this->tag = 0x80;
                int add_len = 1;
                for (int i = 1; i < sizeof(this->length); i++)
                    if (this->length >> (8 * i) > 0)
                        add_len++;
                this->tag |= add_len;
                this->length_array = new uint8_t[add_len];
                for (int i = 0; i < add_len; i++)
                    this->length_array[add_len - 1 - i] = (char)(this->length >> (8 * i));
            }
        }
        
        void decodeLength (char *string) {
            int add_len = 0;
            if ( (this->tag & 0x80) == 0 ) {
                this->length = this->tag & 0x7F;
            } else {
                add_len = this->tag & 0x7F;
                this->length_array = new uint8_t[add_len];
                for (int i = 0; i < add_len; i++) {
                    length_array[i] = static_cast<uint8_t>(string[i + 1]);
                    this->length |= static_cast<uint8_t>(string[i + 1]);
                    this->length = this->length << (8 * (add_len - i - 1));
                }
            }
            this->message = new uint8_t[this->length];
            for (int i = 0; i < this->length; i++)
                message[i] = reinterpret_cast<uint8_t*>(string)[i + add_len + 1];
            message[this->length] = '\0';
        }
    };

} // namespace easyBER