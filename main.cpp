#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <complex>
#include <cassert>

#include "philips.h"

int generate_test_lab(const char* filename)
{
    typedef struct
    {
        uint32_t data_size;
        char test_bank1[10];
        uint16_t label_type;
        char test_bank2[48];
    } test_label;

    test_label l;

    std::ofstream f(filename, std::ofstream::binary);

    if (!f.is_open()) {
        std::cout << "Error opening file" << std::endl;
        return -1;
    }

    for (size_t i = 0; i < 10; ++i) {
        memset(&l,0,sizeof(test_label));
        l.label_type = philips::LABEL_TYPE_STANDARD;
        l.test_bank1[i] = 1;
        f.write(reinterpret_cast<char*>(&l),sizeof(test_label));
    }

    for (size_t i = 0; i < 48; ++i) {
        memset(&l,0,sizeof(test_label));
        l.label_type = philips::LABEL_TYPE_STANDARD;
        l.test_bank2[i] = 1;
        f.write(reinterpret_cast<char*>(&l),sizeof(test_label));
    }

    f.close();
    return 0;
}

/**
 * Decodes an unsigned variable-length integer using the MSB algorithm.
 * @param value A variable-length encoded integer of arbitrary size.
 * @param inputSize How many bytes are
 */
template<typename int_t = uint64_t>
int_t decodeVarint(uint8_t* input, uint8_t inputSize, uint8_t* bytes_used) {
    int_t ret = 0;
    for (uint8_t i = 0; i < inputSize; i++) {
        ret |= (input[i] & 127) << (7 * i);
        *bytes_used = i + 1;
        //If the next-byte flag is set
        if(!(input[i] & 128)) {
            break;
        }
    }
    return ret;
}

template <typename T> T read(uint8_t** bufptr)
{
    T t = *(reinterpret_cast<T*>(*bufptr));
    *bufptr += sizeof(T);
    return t;
}

int32_t decode_int(uint8_t** buffer, int32_t bitres, bool sign, int32_t& val, int32_t& bitpos) {
    int32_t tmp = 0;
    const int32_t NUMBITS = 32;

    if (bitpos == 0) {
        val = read<int32_t>(buffer);
        tmp = val;
        bitpos = NUMBITS - bitres;
    } else {
        tmp = val << (NUMBITS - bitpos);
        if (bitpos >= bitres) {
            bitpos -= bitres;
        } else {
            val = read<int32_t>(buffer);
            tmp |= (uint32_t)val >> bitpos;
            bitpos += NUMBITS - bitres;
        }
    }

    if (sign) {
        return tmp >> (NUMBITS - bitres);
    } else {
        return (uint32_t)tmp >> (NUMBITS - bitres);
    }
}

void decode_chunk(uint8_t* chunk_buffer, int32_t* decoded_buffer, size_t nelements)
{
    int32_t val = 0, bitpos = 0;
    int idx = 0;
    while (idx < nelements) {
        int32_t bitres = decode_int(&chunk_buffer, 5, false, val, bitpos);
        printf("-- val: %d, bitpos: %d, bitres: %d\n", val, bitpos, bitres);

        // `shift` is *usually* == 0
        int32_t shift = decode_int(&chunk_buffer, 5, false, val, bitpos);
        printf("-- val: %d, bitpos: %d, shift: %d\n", val, bitpos, shift);

        // `addend` is *usually* == 0
        uint32_t addend = (uint32_t)(1 << shift) >> 1;

        for (int n = std::min((int)nelements - idx, 16); n > 0; n--) {
            int32_t tmp = decode_int(&chunk_buffer, bitres, true, val, bitpos);
            int32_t result = (tmp << shift) + addend;
            printf("%2d val: %d, bitpos: %d, tmp: %d, result: %d\n", n, val, bitpos, tmp, result);
            /* printf("decoded[%d] = %d\n", idx, result); */
            decoded_buffer[idx++] = result;
        }
    }
}

void decode(uint8_t* encoded_buffer, int32_t* decoded_buffer, uint32_t decoded_data_size)
{
    size_t num_decoded_bytes = 0;
    int chunk_count = 0;
    double encoded_chunk_size = 0;
    double decoded_chunk_size = 0;
    while (num_decoded_bytes < decoded_data_size) {
        chunk_count++;
        uint16_t expected_decoded_size = read<uint16_t>(&encoded_buffer);
        uint16_t expected_encoded_size = read<uint16_t>(&encoded_buffer);
        uint32_t decoded_buffer_offset = read<uint32_t>(&encoded_buffer) / 4;

        size_t nelements = expected_decoded_size / 4;
        int32_t* output_buffer = decoded_buffer + decoded_buffer_offset;
        decode_chunk(encoded_buffer, output_buffer, nelements);
        num_decoded_bytes += expected_decoded_size;
        encoded_buffer += expected_encoded_size;

        encoded_chunk_size += (double)expected_encoded_size;
        decoded_chunk_size += (double)expected_decoded_size;
    }
    /* std::cout << */
    /*   "chunk_count: " << chunk_count << ", " << */
    /*   "avg. dec size: " << decoded_chunk_size / (double)chunk_count << ", " << */
    /*   "avg. enc size: " << encoded_chunk_size / (double)chunk_count << ", " << */
    /*   std::endl; */
}

int main(int argc, char** argv)
{
    std::cout << "PHILIPS to ISMRMRD Converter" << std::endl;
    std::cout << "Size of header: " << sizeof(philips::label) << std::endl;

    if (argc < 2) {
        std::cout << "No file name supplied generating test label file" << std::endl;
        return generate_test_lab("test.lab");
    }

    std::string filename = std::string(argv[1]) + std::string(".lab");
    std::ifstream f(filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate); //Open file and position at end

    if (!f.is_open()) {
        std::cerr << "Error opening LAB file" << std::endl;
        return -1;
    }

    std::string raw_filename = std::string(argv[1]) + std::string(".raw");
    std::ifstream raw(raw_filename.c_str(), std::ios::in | std::ios::binary); //Open file and position at end

    if (!raw.is_open()) {
        std::cerr << "Error opening RAW file" << std::endl;
    }


    int additional_raw_offset = 0;
    if (argc >= 3) {
        std::stringstream str;
        str << argv[2];
        str >> additional_raw_offset;
        std::cout << "Additional raw_offset: " << additional_raw_offset << std::endl;
    }

    size_t file_length = f.tellg();
    size_t raw_file_offset = 512 + additional_raw_offset; //The first 512 bytes of file appear to be empty
    f.seekg (0, std::ios::beg);
    raw.seekg (raw_file_offset, std::ios::beg);

    philips::label l;


    std::ofstream dump("dump.dat", std::ios::out | std::ios::binary);


    size_t count = 0;
    size_t max_count = 2000;
    while ((file_length - f.tellg()) >= sizeof(philips::label)) {
        if (max_count-- == 0) break;
        f.read(reinterpret_cast<char*>(&l), sizeof(philips::label));
        if (l.new_.label_type > philips::LABEL_TYPE_MIN && l.new_.label_type < philips::LABEL_TYPE_MAX) {

            bool need_decoding = false;
            if (l.new_.raw_format == 4 || l.new_.raw_format == 6) {
                need_decoding = true;
            }

            std::cout << need_decoding
              << "\t" << raw_file_offset
              << "\t" << l.new_.data_size
              << "\t" << l.new_.coded_data_size
              << "\t" << l.new_.normalization_factor
              << "\t" << l.new_.seq_nr
              << "\t" << l.new_.label_type
              << "\t" << static_cast<int>(l.new_.control)
              << "\t" << l.new_.e1_profile_nr
              << std::endl;

            // TODO: nr_measured_channels in 'sin' file
            size_t nchan = 13;
            size_t sample_bytes = 4; // int32... 2 bytes (int16) for non-Mira
            size_t nsamp = l.new_.data_size / nchan / 2 / sample_bytes;

            int32_t* converted = new int32_t[l.new_.data_size/4];
            memset(converted, 0, l.new_.data_size);

            count++;
            size_t bytes_read = 0;
            if (need_decoding) {
                uint8_t* buffer = new uint8_t[l.new_.coded_data_size];
                raw.read((char*)buffer, l.new_.coded_data_size);
                bytes_read = l.new_.coded_data_size;
                if (l.new_.control == philips::CTRL_NORMAL_DATA) {
                    decode(buffer, converted, l.new_.data_size);
                    auto data = new std::complex<float>[l.new_.data_size / 8];
                    for (int i = 0; i < l.new_.data_size / 8; i++) {
                        data[i] = std::complex<float>(converted[i*2], converted[i*2+1]);
                    }
                    dump.write(reinterpret_cast<char*>(data), l.new_.data_size);
                }
                delete [] buffer;
            } else {
                raw.read((char*)converted, l.new_.data_size);
                bytes_read = l.new_.coded_data_size;
            }

            raw_file_offset += bytes_read;


            delete [] converted;
        }
    }
    f.close();
    raw.close();

    std::cout << "Found " << count << " labels" << std::endl;
    std::cout << "Data size: " << raw_file_offset << std::endl;
    return 0;
}
