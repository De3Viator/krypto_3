#include <iostream>
#include <cstdint>
#include <array>
#include <bitset>

const unsigned short tetra_size = 4;
const unsigned short xor_bit = 1;

struct SplittedData {
    uint8_t first_tetra[tetra_size];
    uint8_t second_tetra[tetra_size];
};
enum CryptoOperation {ENCRYPT_MODE, DECRYPT_MODE};

SplittedData split_by_tetras(uint8_t data) {
    SplittedData result{};

    for (int i = 7; i >= 0; --i) {
        bool bit = (data >> i) & 1;
        if (i < 4) {
            result.second_tetra[(tetra_size-1)-i] = bit;
        } else {
            result.first_tetra[(tetra_size*2-1)-i] = bit;
        }
    }

    return result;
}

std::bitset<4> makeCryptoOperation(uint8_t tetra[tetra_size], CryptoOperation mode) {
    for (int i = 0; i < tetra_size/2; ++i) {
        bool first_bit = tetra[i];
        bool second_bit = tetra[tetra_size - i - 1];
        if(mode == DECRYPT_MODE) {
            tetra[i] = xor_bit ^ second_bit;
            tetra[tetra_size - i - 1] = xor_bit ^ first_bit;
        } else {
            tetra[i] = second_bit ^ xor_bit;
            tetra[tetra_size - i - 1] = first_bit ^ xor_bit;
        }
    }
    uint8_t result = 0;
    for (int i = 0; i < 4; ++i) {
        bool el = tetra[i];
        result = (result << 1) | el;
    }
    std::bitset<4> binary(result);
    return binary;
}

uint8_t encrypt(uint8_t data) {
    SplittedData splitted_result = split_by_tetras(data);
    uint8_t  first_encryption = makeCryptoOperation(splitted_result.first_tetra, ENCRYPT_MODE).to_ulong();
    uint8_t  second_encryption = makeCryptoOperation(splitted_result.second_tetra, ENCRYPT_MODE).to_ulong();
    uint8_t  result = (first_encryption << 4) | second_encryption;

    return result;
}


uint8_t decrypt(uint8_t data) {
    SplittedData splitted_result = split_by_tetras(data);
    uint8_t  first_decryption = makeCryptoOperation(splitted_result.first_tetra, DECRYPT_MODE).to_ulong();
    uint8_t  second_decryption = makeCryptoOperation(splitted_result.second_tetra, DECRYPT_MODE).to_ulong();
    uint8_t  result = (first_decryption << 4) | second_decryption;
    return result;
}

int main() {
    uint8_t bits = 0b10100110;
    std::cout<<bits;
    uint8_t encrypted = encrypt(bits);
    std::cout<<encrypted;
    uint8_t decrypted = decrypt(encrypted);
    std::cout<<decrypted;
    return 0;
}