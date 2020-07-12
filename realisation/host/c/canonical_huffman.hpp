
#ifndef CANONICAL_HUFFMAN_HPP
#define CANONICAL_HUFFMAN_HPP

#include <queue>
#include <vector>
#include <tuple>
#include <cstdint>
#include <bitset>
#include <iostream>

class CanonicalHuffman {
    public:
    std::vector<size_t> get_huffman_code_sizes(uint8_t* raw_data, size_t raw_data_size, size_t dict_size) {
        std::vector<std::tuple<size_t, uint8_t>> probability;
        for (size_t symbol = 0; symbol < dict_size; symbol++) {
            probability.push_back(std::make_tuple(0, symbol));
        }

        for (size_t i = 0; i < raw_data_size; i++) {
            std::get<0>(probability[raw_data[i]])++;
        }

        std::priority_queue<std::tuple<size_t, std::vector<uint8_t>>, std::vector<std::tuple<size_t, std::vector<uint8_t>>>, std::greater<std::tuple<size_t, std::vector<uint8_t>>>> priority_queue;
        
        for (std::tuple<size_t, uint8_t> symbol : probability) {
            priority_queue.push(std::make_tuple(std::get<0>(symbol), std::vector<uint8_t>({std::get<1>(symbol)})));
        }

        std::vector<size_t> huffman_code_sizes;
        for (size_t i = 0; i < dict_size; i++) {
            huffman_code_sizes.push_back(0);
        }

        while (priority_queue.size() > 1) {
            std::tuple<size_t, std::vector<uint8_t>> elt_1 = priority_queue.top();
            priority_queue.pop();
            for (uint8_t elt : std::get<1>(elt_1)) {
                huffman_code_sizes[elt]++;
            }

            std::tuple<size_t, std::vector<uint8_t>> elt_0 = priority_queue.top();
            priority_queue.pop();
            for (uint8_t elt : std::get<1>(elt_0)) {
                huffman_code_sizes[elt]++;
            }

            std::get<1>(elt_1).insert(std::get<1>(elt_1).end(), std::get<1>(elt_0).begin(), std::get<1>(elt_0).end());
            priority_queue.push(std::make_tuple(std::get<0>(elt_1) + std::get<0>(elt_0), std::get<1>(elt_1)));
        }

        return huffman_code_sizes;
    }

    std::vector<std::tuple<std::bitset<256>, size_t>> get_canonical_huffman_code(std::vector<size_t>& huffman_code_sizes, uint8_t* size_counts, uint8_t* sorted_symbols) {
        bool same_code_length_flag = false;
        // for each code size, count number of symbols with this code size
        std::vector<size_t> size_counts_big;
        for (size_t i = 0; i < huffman_code_sizes.size(); i++) {
            size_counts_big.push_back(0);
        }
        for (size_t code_size : huffman_code_sizes) {
            size_counts_big[code_size]++;
            if (size_counts_big[code_size] > 255) {
                same_code_length_flag = true;
            }
        }
        // american flag sort: count number of elements in each bucket
        std::vector<size_t> bucket_indexes;
        size_t acc = 0;
        for (size_t i = 0; i < size_counts_big.size(); i++) {
            bucket_indexes.push_back(acc);
            acc += size_counts_big[i];
        }
        // american flag sort: sort symbols by code size
        for (size_t symbol = 0; symbol < huffman_code_sizes.size(); symbol++) {
            sorted_symbols[bucket_indexes[huffman_code_sizes[symbol]]] = symbol;
            bucket_indexes[huffman_code_sizes[symbol]]++;
        }
        // compute each symbols' code
        std::vector<std::tuple<std::bitset<256>, size_t>> canonical_code;
        for (size_t i = 0; i < huffman_code_sizes.size(); i++) {
            canonical_code.push_back(std::make_tuple(0, 0));
        }
        size_t index = 0;
        std::bitset<256> first_code_on_row = 0;
        for (size_t row = 0; row < size_counts_big.size(); row++) {
            first_code_on_row <<= 1;
            for (size_t i = 0; i < size_counts_big[row]; i++) {
                canonical_code[sorted_symbols[index]] = std::make_tuple(first_code_on_row, row);
                index++;

                std::bitset<256> mask = 1;
                for (size_t bit = 0; bit < first_code_on_row.size(); bit++) {
                    bool old_bit = first_code_on_row[bit];
                    first_code_on_row[bit] = !old_bit;
                    if (!old_bit) {
                        break;
                    }
                    mask <<= 1;
                }
            }
        }

        for (size_t i = 0; i < size_counts_big.size(); i++) {
            size_counts[i] = same_code_length_flag ? 0 : size_counts_big[i];
        }
        if (same_code_length_flag) {
            size_counts[0] = 1;
        }

        return canonical_code;
    }

    size_t encode_data(std::vector<std::tuple<std::bitset<256>, size_t>>& canonical_code, uint8_t* raw_data, size_t raw_data_size, size_t offset, uint8_t* encoded_data, size_t encoded_data_size) {
        size_t index = 0;
        size_t count = 0;
        for (size_t i = 0; i < raw_data_size - offset; i++) {
            uint8_t symbol = raw_data[i + offset];
            std::tuple<std::bitset<256>, size_t> code = canonical_code[symbol];

            if (std::get<1>(code) + index >= encoded_data_size*8) {
                break;
            }
            count++;
            for (size_t j = 0; j < std::get<1>(code); j++) {
                if ((std::get<0>(code) >> (std::get<1>(code) - 1 - j))[0]) encoded_data[index/8] |= 1 << (index % 8);
                else encoded_data[index/8] &= ~(1 << int(index % 8));
                index++;
            }
        }

        return count;
    }

};

#endif
