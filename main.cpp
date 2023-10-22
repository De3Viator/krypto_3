#include <iostream>
#include <cstdint>
#include <array>
#include <bitset>
#include <map>

const unsigned short tetra_size = 4;
const unsigned short xor_bit = 1;

struct SplittedData {
    uint8_t first_tetra[tetra_size];
    uint8_t second_tetra[tetra_size];
};
struct BitsetComparer {
    bool operator()(const std::bitset<4>& a, const std::bitset<4>& b) const {
        return a.to_ulong() < b.to_ulong();
    }
};
enum CryptoOperation {ENCRYPT_MODE, DECRYPT_MODE};
std::map<std::bitset<4>, std::bitset<4>, BitsetComparer> table;

void loadTable() {
    for (unsigned int i = 0; i < 16; i++) {
        std::bitset<4> key(i);
        std::bitset<4> value((i + 4) % 16);
        table[key] = value;
    }
}

std::bitset<4> findValueInTableByKey(std::bitset<4> byte) {
    std::bitset<4> key(byte);
    std::bitset<4> value = table[key];
    return value;
}

std::bitset<4> findKeyInTableByValue(std::bitset<4> key) {
    for (const auto& entry : table) {
        if (entry.second == std::bitset<4>(key)) {
            return entry.first;
        }
    }
}

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

uint8_t makeCryptoArrayToBites(const uint8_t tetra[tetra_size]) {
    uint8_t result = 0;
    for (int i = 0; i < tetra_size; ++i) {
        bool el = tetra[i];
        result = (result << 1) | el;
    }
    return result;
}

std::bitset<4> makeCryptoOperation(uint8_t tetra[tetra_size], CryptoOperation mode) {
    if(mode == DECRYPT_MODE) {
        uint8_t result = makeCryptoArrayToBites(tetra);
        std::bitset<4> decrypted = findKeyInTableByValue(result);
        for (int i = 0; i < 4; i++) {
            bool el = decrypted.test(i) ? 1 : 0;
            tetra[tetra_size-1-i] = el;
        }
    }
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
    uint8_t result = makeCryptoArrayToBites(tetra);
    std::bitset<4> binary(result);
    std::bitset<4> code;
    if(mode == ENCRYPT_MODE) {
        code = findValueInTableByKey(binary);
    }
    else code = binary;
    return code;
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
    loadTable();
    uint8_t bits = 0b10100110;
    std::cout<<bits;
    uint8_t encrypted = encrypt(bits);
    std::cout<<encrypted;
    uint8_t decrypted = decrypt(encrypted);
    std::cout<<decrypted;
    return 0;
}