#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

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

  size_t count = 0;
  size_t max_count = 300;
  while ((file_length - f.tellg()) >= sizeof(philips::label)) {
    if (max_count-- == 0) break;
    f.read(reinterpret_cast<char*>(&l),sizeof(philips::label));
    if (l.new_.label_type > philips::LABEL_TYPE_MIN && l.new_.label_type < philips::LABEL_TYPE_MAX) {
      std::cout << raw_file_offset;
      std::cout << "\t" << l.new_.data_size
		<< "\t" << l.new_.coded_data_size
		<< "\t" << l.new_.normalization_factor
		<< "\t" << l.new_.seq_nr
		<< "\t" << l.new_.label_type
		<< "\t" << static_cast<int>(l.new_.control)
		<< "\t" << l.new_.e1_profile_nr
		<< std::endl;


      uint8_t* buffer = new uint8_t[l.new_.coded_data_size];

      size_t potential_ints_lsb = 0;
      size_t potential_ints_msb = 0;

      size_t boffset = 0;
      while (boffset < l.new_.coded_data_size) {
	potential_ints_msb++;
	while (buffer[boffset] & 0x80) boffset++;
	boffset++;
	//	std::cout << boffset << std::endl;;
      }
      //std::cout << "Potential ints msb: " << potential_ints_msb << std::endl;
      
      raw.read((char*)buffer,l.new_.coded_data_size);
      count++;
      raw_file_offset += l.new_.coded_data_size;
      
      uint64_t* converted = new uint64_t[l.new_.data_size/4];
      
      size_t bytes_used = 0;
      size_t int_count = 0;
      while (bytes_used < l.new_.coded_data_size) {
	if (int_count >= l.new_.data_size/4) {
	  //std::cerr << "Too many integers produced by stream" << std::endl;
	  for (size_t i = 0; i < int_count; i++) {
	    //std::cout << converted[i] << ", ";
	  }
	  //std::cout << std::endl;
	  break;
	}
	
	uint8_t bc = 0;
	converted[int_count] = decodeVarint<uint64_t>(buffer+bytes_used, 10, &bc);
	int_count++;
	bytes_used += bc;
      }
      //std::cout << "Ints expected: " << l.new_.data_size/4 << ", ints decoded: " << int_count << std::endl; 
      delete [] converted;
      delete [] buffer; 
    }
  }
  f.close();
  raw.close();
  
  std::cout << "Found " << count << " labels" << std::endl;
  std::cout << "Data size: " << raw_file_offset << std::endl;
  return 0;
}
