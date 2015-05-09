#include <iostream>
#include <fstream>
#include <sstream>
#include "pugixml.hpp"
#include <exception>

void sintoxml(std::ostream& s, const char* filename)
{
  std::ifstream f(filename);
  if (!f.is_open()) {
    throw std::runtime_error("Unable to open SIN file for XML conversion");
  }

  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child();
  root.set_name("philips");

  std::string line;
  const size_t buffer_size = 4096;
  char buffer[buffer_size];
  while (!f.eof()) {
    f.getline(buffer,buffer_size);
    std::string line(buffer);
    std::stringstream s;
    uint16_t idx1, idx2, idx3;
    char tmp;
    std::string parameter_name;
    std::string parameter_value;
    if (line.find(':') != std::string::npos && std::isdigit(line[1])) {
      s << line;
      s >> idx1; s >> idx2; s >> idx3; s >> tmp;
      s >> parameter_name;
      pugi::xml_node parm = root.append_child(parameter_name.c_str());
      pugi::xml_attribute attr_idx1 = parm.append_attribute("idx1");
      attr_idx1.set_value(idx1);
      pugi::xml_attribute attr_idx2 = parm.append_attribute("idx2");
      attr_idx2.set_value(idx2);
      pugi::xml_attribute attr_idx3 = parm.append_attribute("idx3");
      attr_idx3.set_value(idx3);
      s >> tmp;
      while (!s.eof()) {
	s >> parameter_value;
	if (s.eof()) break;
	pugi::xml_node v = parm.append_child("value");
	v.append_child(pugi::node_pcdata).set_value(parameter_value.c_str());
      }
    }
  }

  f.close();
  doc.save(s);
}


int main(int argc, char** argv)
{
  std::cout << "SIN to XML converter" << std::endl;

  if (argc != 2) {
    std::cout << "Please provide *.sin filename" << std::endl;
    return -1;
  }

  std::stringstream s;
  try {
    sintoxml(s,argv[1]);
  } catch (std::runtime_error& e) {
    std::cerr << "Failed to convert SIN file to XML: " << e.what() << std::endl;
    return -1;
  }

  std::cout << s.str() << std::endl;
  return 0;
}
