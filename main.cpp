#include <iostream>
#include <fstream>
#include <string>

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

int main(int argc, char** argv)
{
  std::cout << "PHILIPS to ISMRMRD Converter" << std::endl;
  std::cout << "Size of header: " << sizeof(philips::label) << std::endl;

  if (argc != 2) {
    std::cout << "No file name supplied generating test label file" << std::endl;
    return generate_test_lab("test.lab");
  }

  std::string filename(argv[1]);
  std::ifstream f(filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate); //Open file and position at end

  if (!f.is_open()) {
    std::cout << "Error opening file" << std::endl;
    return -1;
  }

  size_t file_length = f.tellg();
  f.seekg (0, std::ios::beg);

  philips::label l;

  size_t count = 0;
  size_t raw_file_offset = 0;
  while ((file_length - f.tellg()) >= sizeof(philips::label)) {
    f.read(reinterpret_cast<char*>(&l),sizeof(philips::label));
    if (l.new_.label_type > philips::LABEL_TYPE_MIN && l.new_.label_type < philips::LABEL_TYPE_MAX) {
      std::cout << "\t" << l.new_.data_size
		<< "\t" << l.new_.coded_data_size
		<< "\t" << l.new_.normalization_factor
		<< "\t" << l.new_.seq_nr
		<< "\t" << l.new_.label_type
		<< "\t" << static_cast<int>(l.new_.control)
		<< std::endl;
      
      if (l.new_.control == 0) {
	count++;
	raw_file_offset += l.new_.data_size;//-l.leading_dummies_size-l.trailing_dummies_size;
      }
    }
  }
  f.close();

  std::cout << "Found " << count << " labels" << std::endl;
  std::cout << "Data size: " << raw_file_offset << std::endl;
  return 0;
}
