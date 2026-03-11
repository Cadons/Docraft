#include "docraft/utils/docraft_base64.h"

#include <array>
#include <stdexcept>

namespace docraft::utils {
    namespace {
        constexpr unsigned char kInvalid = 0xFF;

        // Build a lookup table for decoding base64 characters. Non-base64 characters are marked as kInvalid.
        constexpr std::array<unsigned char, 256> build_decode_table() {
            std::array<unsigned char, 256> table{};//default initialization to 0, then fill with kInvalid, table is 256 bytes, so this is not a large overhead
            table.fill(kInvalid);//mark all as invalid by default
            for (unsigned char c = 'A'; c <= 'Z'; ++c) {//A-Z maps to 0-25
                table[c] = static_cast<unsigned char>(c - 'A');
            }
            for (unsigned char c = 'a'; c <= 'z'; ++c) {//a-z maps to 26-51
                table[c] = static_cast<unsigned char>(c - 'a' + 26);
            }
            for (unsigned char c = '0'; c <= '9'; ++c) {//0-9 maps to 52-61
                table[c] = static_cast<unsigned char>(c - '0' + 52);
            }
            table[static_cast<unsigned char>('+')] = 62;//+ maps to 62
            table[static_cast<unsigned char>('/')] = 63;// / maps to 63
            return table;
        }

        constexpr auto kDecodeTable = build_decode_table();//global constant lookup table for decoding base64 characters
    } // namespace

    // Decodes base64 into bytes. Ignores whitespace and enforces valid padding/length.
    std::vector<unsigned char> decode_base64(std::string_view input) {
        std::vector<unsigned char> out;//pre-allocate output size based on input length, accounting for padding and ignoring whitespace
        out.reserve((input.size() * 3) / 4);//base64 encodes 3 bytes into 4 characters,
        //so the output will be at most 3/4 the size of the input (ignoring whitespace and padding)

        unsigned char quartet[4] = {0, 0, 0, 0};//to hold the 4 decoded values for each base64 quartet
        int quartet_len = 0;//number of valid base64 characters currently in the quartet
        int padding = 0;//number of padding characters encountered in the current quartet

        for (char ch : input) {//process each character in the input
            if (ch == '=' || ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t') {//ignore padding and whitespace
                if (ch == '=') {//count padding characters, but only if they are at the end of a quartet
                    padding++;//padding can only be valid if it comes after at least 2 base64 characters in the quartet, and no more than 2 padding characters are allowed
                    quartet[quartet_len++] = 0;//treat padding as zero value for decoding, but it will be handled specially when outputting bytes
                }
                continue;
            }
            unsigned char value = kDecodeTable[static_cast<unsigned char>(ch)];//lookup the value of the base64 character using the decode table
            if (value == kInvalid) {//if the character is not a valid base64 character, throw an error
                throw std::invalid_argument("Invalid base64 character.");
            }
            quartet[quartet_len++] = value;//add the decoded value to the quartet
            if (quartet_len == 4) {//once we have 4 valid base64 characters (or padding), we can decode them into bytes
                out.push_back(static_cast<unsigned char>((quartet[0] << 2) | (quartet[1] >> 4)));//the first byte is formed by taking the first 6 bits of the first character and the first 2 bits of the second character
                if (padding < 2) {//if there is less than 2 padding characters, we can output the second byte
                    out.push_back(static_cast<unsigned char>((quartet[1] << 4) | (quartet[2] >> 2)));
                }
                if (padding == 0) {//if there is no padding, we can output the third byte
                    out.push_back(static_cast<unsigned char>((quartet[2] << 6) | quartet[3]));
                }
                quartet_len = 0;//reset quartet for the next 4 characters
                padding = 0;//reset padding count for the next quartet
            }
        }

        // Any trailing, incomplete quartet indicates invalid base64 length.
        if (quartet_len != 0) {
            throw std::invalid_argument("Invalid base64 length.");
        }

        return out;
    }
} // namespace docraft::utils
