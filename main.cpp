#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <complex>
#include <cassert>
#include <stdexcept>
#include "pugixml.hpp"
#include <math.h>

#include <libxml/parser.h>
#include <libxml/xmlschemas.h>
#include <libxml/xmlmemory.h>
#include <libxml/debugXML.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlIO.h>
#include <libxml/xinclude.h>
#include <libxml/catalog.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/dataset.h"
#include "ismrmrd/version.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "philips.h"

struct sinparms
{
  sinparms()
    : ismira(true)
    , nchan(1)
  {
    
  }
  
  bool ismira;
  uint16_t nchan;
  std::vector< std::complex<float> > pda_amp_factors;
};

void sintoxml(std::ostream& s, const char* filename, sinparms& sp)
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

      bool get_nchan = false;
      float pda[2];
      bool get_pda = false;
      int pda_comp = 0;
      if (parameter_name == "enable_pda") {
	sp.ismira = false;
      } else if (parameter_name == "max_measured_channels") {
	get_nchan = true;
      } else if (parameter_name == "nr_measured_channels") {
	get_nchan = true;
      } else if (parameter_name == "pda_ampl_factors") {
	get_pda = true;
      }
      
      while (!s.eof()) {
	s >> parameter_value;

	if (get_nchan) {
	  sp.nchan = std::atoi(parameter_value.c_str());
	}

	if (get_pda) {
	  pda[pda_comp++] = std::atof(parameter_value.c_str());
      
	  if (pda_comp > 1) {
	    sp.pda_amp_factors.push_back(std::complex<float>(pda[0],pda[1]));
	    pda_comp = 0;
	  }
	}

	if (s.eof()) break;
	pugi::xml_node v = parm.append_child("value");
	v.append_child(pugi::node_pcdata).set_value(parameter_value.c_str());
      }
    }
  }

  f.close();
  doc.save(s);
}

int xml_file_is_valid(std::string& xml, std::string& schema_file)
{
    xmlDocPtr doc;
    //parse an XML in-memory block and build a tree.
    doc = xmlParseMemory(xml.c_str(), xml.size());

    xmlDocPtr schema_doc;
    //parse an XML in-memory block and build a tree.
    schema_doc = xmlParseMemory(schema_file.c_str(), schema_file.size());

    //Create an XML Schemas parse context for that document. NB. The document may be modified during the parsing process.
    xmlSchemaParserCtxtPtr parser_ctxt = xmlSchemaNewDocParserCtxt(schema_doc);
    if (parser_ctxt == NULL)
    {
        /* unable to create a parser context for the schema */
        xmlFreeDoc(schema_doc);
        return -2;
    }

    //parse a schema definition resource and build an internal XML Shema struture which can be used to validate instances.
    xmlSchemaPtr schema = xmlSchemaParse(parser_ctxt);
    if (schema == NULL)
    {
        /* the schema itself is not valid */
        xmlSchemaFreeParserCtxt(parser_ctxt);
        xmlFreeDoc(schema_doc);
        return -3;
    }

    //Create an XML Schemas validation context based on the given schema.
    xmlSchemaValidCtxtPtr valid_ctxt = xmlSchemaNewValidCtxt(schema);
    if (valid_ctxt == NULL)
    {
        /* unable to create a validation context for the schema */
        xmlSchemaFree(schema);
        xmlSchemaFreeParserCtxt(parser_ctxt);
        xmlFreeDoc(schema_doc);
        xmlFreeDoc(doc);
        return -4;
    }

    //Validate a document tree in memory. Takes a schema validation context and a parsed document tree
    int is_valid = (xmlSchemaValidateDoc(valid_ctxt, doc) == 0);
    xmlSchemaFreeValidCtxt(valid_ctxt);
    xmlSchemaFree(schema);
    xmlSchemaFreeParserCtxt(parser_ctxt);
    xmlFreeDoc(schema_doc);
    xmlFreeDoc(doc);

    /* force the return value to be non-negative on success */
    return is_valid ? 1 : 0;
}

std::string apply_stylesheet(std::string xml_raw, std::string xsl)
{
  xsltStylesheetPtr cur = NULL;
  xmlDocPtr doc, res, xml_doc;
  const char *params[16 + 1];
  int nbparams = 0;
  params[nbparams] = NULL;
  xmlSubstituteEntitiesDefault(1);
  xmlLoadExtDtdDefaultValue = 1;

  xml_doc = xmlParseMemory(xsl.c_str(), xsl.size());

  if (xml_doc == NULL) { 
    throw std::runtime_error("Error when parsing xsl parameter stylesheet");
  }

  cur = xsltParseStylesheetDoc(xml_doc);
  doc = xmlParseMemory(xml_raw.c_str(), xml_raw.size());
  res = xsltApplyStylesheet(cur, doc, params);

  xmlChar* out_ptr = NULL;
  int xslt_length = 0;
  int xslt_result = xsltSaveResultToString(&out_ptr, &xslt_length, res, cur);
  
  if (xslt_result < 0) {
    throw std::runtime_error("Failed to save converted doc to string");
  }

  std::string xml_out((char*)out_ptr,xslt_length);

  xsltFreeStylesheet(cur);
  xmlFreeDoc(res);
  xmlFreeDoc(doc);
  
  xsltCleanupGlobals();
  xmlCleanupParser();

  return xml_out;
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
    //printf("-- val: %d, bitpos: %d, bitres: %d\n", val, bitpos, bitres);

    // `shift` is *usually* == 0
    int32_t shift = decode_int(&chunk_buffer, 5, false, val, bitpos);
    //printf("-- val: %d, bitpos: %d, shift: %d\n", val, bitpos, shift);

    // `addend` is *usually* == 0
    uint32_t addend = (uint32_t)(1 << shift) >> 1;

    for (int n = std::min((int)nelements - idx, 16); n > 0; n--) {
      int32_t tmp = decode_int(&chunk_buffer, bitres, true, val, bitpos);
      int32_t result = (tmp << shift) + addend;
      //printf("%2d val: %d, bitpos: %d, tmp: %d, result: %d\n", n, val, bitpos, tmp, result);
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

  std::string filename;
  std::string parammap_xsl;
  std::string ismrmrd_schema;
  
  std::string ismrmrd_file;
  std::string ismrmrd_group;

  bool debug_xml = false;
  bool header_only = false;

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h",                  "Produce HELP message")
    ("file,f",                  po::value<std::string>(&filename), "<PHILIPS RAW>")
    ("pMapStyle,x",             po::value<std::string>(&parammap_xsl), "<Parameter stylesheet XSL file>")
    ("schema,s",                po::value<std::string>(&ismrmrd_schema), "<ISMRMRD schema file> (for validation)")
    ("output,o",                po::value<std::string>(&ismrmrd_file)->default_value("output.h5"), "<ISMRMRD output file>")
    ("outputGroup,g",           po::value<std::string>(&ismrmrd_group)->default_value("dataset"), "<ISMRMRD output group>")
    ("debug,X",                 po::value<bool>(&debug_xml)->implicit_value(true), "<Debug XML flag>")
    ("headerOnly,H",            po::value<bool>(&header_only)->implicit_value(true), "<HEADER ONLY flag (create xml header only)>")
    ;

  po::variables_map vm;

  try
    {
      po::store(po::parse_command_line(argc, argv, desc), vm);
      po::notify(vm);

      if (vm.count("help"))
        {
	  std::cout << desc << "\n";
	  return 1;
        }
    } catch(po::error& e) {
    std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
    std::cerr << desc << std::endl;
    return -1;
  }

  std::string lab_filename = filename + std::string(".lab");
  std::string raw_filename = filename + std::string(".raw");
  std::string sin_filename = filename + std::string(".sin");


  //Let's process the sin file first
  sinparms sp;
  std::stringstream s;
  std::string ismrmrd_xml;
  std::string xsl_string;
  std::ifstream fxsl(parammap_xsl.c_str());
  if (!fxsl) {
    std::cerr << "Parameter XSL stylesheet: " << parammap_xsl << " does not exist." << std::endl;
    std::cerr << desc << "\n";
    return -1;
  } else {
    std::string str_f((std::istreambuf_iterator<char>(fxsl)), std::istreambuf_iterator<char>());
    xsl_string = str_f;
  }
  fxsl.close();

  try {
    sintoxml(s,sin_filename.c_str(), sp);
    ismrmrd_xml = apply_stylesheet(s.str(), xsl_string);
    if (debug_xml) {
      std::ofstream rf("raw.xml");
      rf.write(s.str().c_str(),s.str().size());
      rf.close();
      std::ofstream pf("processed.xml");
      pf.write(ismrmrd_xml.c_str(),ismrmrd_xml.size());
      pf.close();
    }
  } catch (std::runtime_error& e) {
    std::cerr << "Failed to convert SIN file to XML: " << e.what() << std::endl;
    return -1;
  }

  if (vm.count("schema")) {
    std::ifstream fxsd(ismrmrd_schema.c_str());
    std::string schema_string;
    if (!fxsd) {
      std::cerr << "ISMRMRD Schema: " << ismrmrd_schema << " does not exist." << std::endl;
      std::cerr << desc << "\n";
      return -1;
    } else {
      std::string str_f((std::istreambuf_iterator<char>(fxsd)), std::istreambuf_iterator<char>());
      schema_string = str_f;
    }
    fxsd.close();
    if (!xml_file_is_valid(ismrmrd_xml,schema_string)) {
      std::cerr << "Generated ISMRMRD XML header is invalid" << std::endl;
      return -1;
    } else {
      std::cout << "ISMRMRD XML header is valid" << std::endl;
    }
  }
  
  std::ifstream labf(lab_filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate); //Open file and position at end

  if (!labf.is_open()) {
    std::cerr << "Error opening LAB file" << std::endl;
    return -1;
  }
    
  std::ifstream raw(raw_filename.c_str(), std::ios::in | std::ios::binary); 

  if (!raw.is_open()) {
    std::cerr << "Error opening RAW file" << std::endl;
  }

  size_t file_length = labf.tellg();
  size_t raw_file_offset = 512;//The first 512 bytes of file appear to be empty
  labf.seekg (0, std::ios::beg);
  raw.seekg (raw_file_offset, std::ios::beg);

  philips::label l;

  // Create an ISMRMRD dataset
  ISMRMRD::Dataset ismrmrd_dataset(ismrmrd_file.c_str(), ismrmrd_group.c_str(), true);
  ismrmrd_dataset.writeHeader(ismrmrd_xml);
  
  size_t count = 0;
  while ((file_length - labf.tellg()) >= sizeof(philips::label)) {
    labf.read(reinterpret_cast<char*>(&l), sizeof(philips::label));

    if (sp.ismira) {      
      if (l.new_.label_type > philips::LABEL_TYPE_MIN && l.new_.label_type < philips::LABEL_TYPE_MAX) {
	
	bool need_decoding = false;
	if (l.new_.raw_format == 4 || l.new_.raw_format == 6) {
	  need_decoding = true;
	}
	
	size_t nchan = sp.nchan;
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
	  }
	  delete [] buffer;
	} else {
	  raw.read((char*)converted, l.new_.data_size);
	  bytes_read = l.new_.coded_data_size;
	}
	
	raw_file_offset += bytes_read;

        if (l.new_.label_type ==  philips::LABEL_TYPE_STANDARD &&
	    ((static_cast<int>(l.new_.control) == philips::CTRL_FRC_NOISE_DATA) || (static_cast<int>(l.new_.control) == philips::CTRL_NORMAL_DATA))) {

	  ISMRMRD::Acquisition acq;
	  acq.clearAllFlags();

	  //TODO: Orientation and position information, should be converted from the SIN file
	  
	  if (l.new_.control == philips::CTRL_FRC_NOISE_DATA) {
	    acq.setFlag(ISMRMRD::ISMRMRD_ACQ_IS_NOISE_MEASUREMENT);
	  }
	  
	  acq.scan_counter() = l.new_.seq_nr;
	  acq.center_sample() = nsamp>>1; //TODO: Needs to be adjusted for partial Fourier, etc.

	  acq.idx().average              = l.new_.measurement_nr;
	  acq.idx().contrast             = l.new_.echo_nr;
	  acq.idx().kspace_encode_step_1 = l.new_.e1_profile_nr;
	  acq.idx().kspace_encode_step_2 = l.new_.e2_profile_nr;
	  acq.idx().phase                = l.new_.cardiac_phase_nr;
	  acq.idx().repetition           = l.new_.dynamic_scan_nr;
	  acq.idx().segment              = 0; //TODO: Fill in meaningful segment nr
	  acq.idx().set                  = 0; //TODO: Fill in meaningful set nr
	  acq.idx().slice                = l.new_.location_nr;
	  
	  acq.resize(nsamp, nchan);
	  std::complex<float>* dptr = acq.getDataPtr();

	  std::complex<float> correction_factor = std::polar<float>(1.0, M_PI * (static_cast<float>(l.new_.random_phase)/ 32767.0) - 0.5*l.new_.measurement_phase);

	  //chop correction
	  if (l.new_.e1_profile_nr%2) correction_factor *= -1.0; 

	  for (size_t c = 0; c < nchan; c++) {
	    for (size_t s = 0; s < nsamp; s++) {
	      dptr[c*nsamp + s] = std::complex<float>(converted[(c*nsamp + s)*2],converted[(c*nsamp + s)*2 + 1]) * correction_factor;
	    }
	  }

	  ismrmrd_dataset.appendAcquisition(acq);
	}

	delete [] converted;
      }
    } else {
      if (l.old_.label_type > philips::LABEL_TYPE_MIN && l.old_.label_type < philips::LABEL_TYPE_MAX) {

	size_t nchan = sp.nchan;
	size_t sample_bytes = 2; 
	size_t nsamp = l.old_.data_size / nchan / 2 / sample_bytes;
	
	int16_t* converted = new int16_t[l.old_.data_size/2];
	raw.read((char*)converted, l.old_.data_size);
	size_t bytes_read = l.old_.coded_data_size;
	
	raw_file_offset += bytes_read;

        if (l.old_.label_type ==  philips::LABEL_TYPE_STANDARD &&
	    ((static_cast<int>(l.old_.control) == philips::CTRL_FRC_NOISE_DATA) || (static_cast<int>(l.old_.control) == philips::CTRL_NORMAL_DATA))) {

	  ISMRMRD::Acquisition acq;
	  acq.clearAllFlags();

	  //TODO: Orientation and position information, should be converted from the SIN file
	  
	  if (l.old_.control == philips::CTRL_FRC_NOISE_DATA) {
	    acq.setFlag(ISMRMRD::ISMRMRD_ACQ_IS_NOISE_MEASUREMENT);
	  }
	  
	  acq.scan_counter() = l.old_.seq_nr;
	  acq.center_sample() = nsamp>>1; //TODO: Needs to be adjusted for partial Fourier, etc.

	  acq.idx().average              = l.old_.measurement_nr;
	  acq.idx().contrast             = l.old_.echo_nr;
	  acq.idx().kspace_encode_step_1 = l.old_.e1_profile_nr;
	  acq.idx().kspace_encode_step_2 = l.old_.e2_profile_nr;
	  acq.idx().phase                = l.old_.cardiac_phase_nr;
	  acq.idx().repetition           = l.old_.dynamic_scan_nr;
	  acq.idx().segment              = 0; //TODO: Fill in meaningful segment nr
	  acq.idx().set                  = 0; //TODO: Fill in meaningful set nr
	  acq.idx().slice                = l.old_.location_nr;
	  
	  acq.resize(nsamp, nchan);
	  std::complex<float>* dptr = acq.getDataPtr();

	  std::complex<float> correction_factor = std::polar<float>(1.0, M_PI * (static_cast<float>(-1.0*l.old_.random_phase)/ 32767.0) - 0.5*l.old_.measurement_phase);

	  //chop correction
	  if (l.old_.e1_profile_nr%2) correction_factor *= -1.0; 
	  
	  for (size_t c = 0; c < nchan; c++) {
	    for (size_t s = 0; s < nsamp; s++) {
	      dptr[c*nsamp + s] = std::complex<float>(converted[(c*nsamp + s)*2],converted[(c*nsamp + s)*2 + 1]) * correction_factor;
	      dptr[c*nsamp + s] *= sp.pda_amp_factors[l.old_.gain_setting_index];
	    }
	  }

	  ismrmrd_dataset.appendAcquisition(acq);
	}

	delete [] converted;
      }
    }
  }
  labf.close();
  raw.close();

  return 0;
}
