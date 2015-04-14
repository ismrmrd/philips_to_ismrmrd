#ifndef PHILIPS_H
#define PHILIPS_H
/**********************************************************************************

 Header file describing the layout of the Philips MRI raw data label files (*.lab)

 The information in this header has been obtained by reverse engineering
 using a hex editor and the output readPhilipsExports module and associated
 ReadPhilips node in the GPI framework. Those tools can be obtained from:
 https://github.com/gpilab/philips-data-reader

 Michael S. Hansen (michael.hansen@nih.gov)
 April 2015

***********************************************************************************/
namespace philips
{

  /*
    
    #Reverse engineered from GPI module readPhilipsExports
    #In Python:

    import readPhilipsExports as rp   
    for l in range(100000):
        if rp.LabelTypeEnum(l) != 'LABEL_TYPE_MAX':
            print str(rp.LabelTypeEnum(l)) + " = " + str(hex(l))
   */  
  typedef enum
  {
    LABEL_TYPE_MIN = 0x7f00,
    LABEL_TYPE_STANDARD = 0x7f01,
    LABEL_TYPE_IGEO = 0x7f02,
    LABEL_TYPE_IGEO_PMC = 0x7f03,
    LABEL_TYPE_COIL_POS = 0x7f04,
    LABEL_TYPE_NON_LIN = 0x7f05,
    LABEL_TYPE_MAX
  } label_types;


  /*
    #Reverse engineered from GPI module readPhilipsExports
    #In Python:

    import readPhilipsExports as rp   
    for l in range(100000):
        if rp.CtrlEnum(l) != 'CTRL_MAX':
            print str(rp.CtrlEnum(l)) + " = " + str(hex(l)) + "," 
   */
  typedef enum
  {
    CTRL_NORMAL_DATA = 0x0,
    CTRL_DC_OFFSET_DATA = 0x1,
    CTRL_JUNK_DATA = 0x2,
    CTRL_ECHO_PHASE_DATA = 0x3,
    CTRL_NO_DATA = 0x4,
    CTRL_NEXT_PHASE = 0x5,
    CTRL_SUSPEND = 0x6,
    CTRL_RESUME = 0x7,
    CTRL_TOTAL_END = 0x8,
    CTRL_INVALIDATION = 0x9,
    CTRL_TYPE_NR_END = 0xa,
    CTRL_VALIDATION = 0xb,
    CTRL_NO_OPERATION = 0xc,
    CTRL_DYN_SCAN_INFO = 0xd,
    CTRL_SELECTIVE_END = 0xe,
    CTRL_FRC_CH_DATA = 0xf,
    CTRL_FRC_NOISE_DATA = 0x10,
    CTRL_REFERENCE_DATA = 0x11,
    CTRL_DC_FIXED_DATA = 0x12,
    CTRL_NAVIGATOR_DATA = 0x13,
    CTRL_FLUSH = 0x14,
    CTRL_RECON_END = 0x15,
    CTRL_IMAGE_STATUS = 0x16,
    CTRL_TRACKING = 0x17,
    CTRL_FLUOROSCOPY_TOGGLE = 0x18,
    CTRL_REJECTED_DATA = 0x19,
    CTRL_PROGRESS_INFO = 0x1a,
    CTRL_END_PREP_PHASE = 0x1b,
    CTRL_CHANNEL_DEFINITION = 0x1c,
    CTRL_MAX
  } ctrl_types;


  /*
    
    The structure of the label (*.lab) files has been reverse engineered in the following way:

    With access to two different versions of the files (an older and a newer) it was determined that there are two different formats. 
    
    The readPhilipsExports Python Module from GPI was used find a) the names of the header fields and b) the location of the header fields. 
    The readPhilipsExports module has a function:

    readLab(filename, isMira)
       Parse a *.lab header accompanying the *.raw data file.
    
    The isMira argument is apparently used to indicate if this is a new header (True) or an old one (False).

    Both new and old headers are 64 bytes long. 

    By visual inspection of the result of reading new and old files using this function, the location of two header fields
    "data_size" and "label_type" was found. These fields are located in the same bytes in the new and old header. 

    A file with test labels was then generated using the following function:

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


    The test label file was then read in python with code like:

    #Python script for inspecting test labels

    import readPhilipsExports as rp

    lab = rp.rp.readLab('test.lab', True) #For new label layout
    lab = rp.rp.readLab('test.lab', False) #For old label layout

    #End of Python script for inspecting test labels

    For example to figure out which bytes contribute to the echo_nr field, simply inspect:
    
    lab['echo_nr'] 

    and find non-zero entries. 

    The names of the header fields are listed below.

    Old header field:
    ['control', 'grad_echo_nr', 'rr_interval', 'echo_nr', 'cardiac_phase_nr', 'dst_code', 'leading_dummies_size', 'monitoring_flag', 'e2_profile_nr', 'row_nr', 'trailing_dummies_size', 'label_type', 'raw_format', 'src_code', 'rf_echo_nr', 'rtop_offset', 'dynamic_scan_nr', 'random_phase', 'location_nr', 'data_size', 'gain_setting_index', 'e3_profile_nr', 'progress_cnt', 'measurement_sign', 'channels_active', 'enc_time', 'measurement_phase', 'extra_attr_nr', 'e1_profile_nr', 'measurement_nr', 'mix_nr', 'seq_nr', 'spare_1']

    New header fields:
    ['control', 'grad_echo_nr', 'rr_interval', 'echo_nr', 'cardiac_phase_nr', 'measurement_nr', 'monitoring_flag', 'e2_profile_nr', 'row_nr', 'label_type', 'e1_profile_nr', 'rf_echo_nr', 'rtop_offset', 'dynamic_scan_nr', 'random_phase', 'location_nr', 'data_size', 'gain_setting_index', 'e3_profile_nr', 'progress_cnt', 'measurement_sign', 'coded_data_size', 'channels_active', 'enc_time', 'measurement_phase', 'extra_attr_nr', 'normalization_factor', 'raw_format', 'mix_nr', 'seq_nr', 'spare_1']
   */

  typedef struct
  {
    uint32_t data_size;
    uint32_t coded_data_size;
    uint16_t src_code;
    uint16_t dst_code;
    uint16_t seq_nr;
    uint16_t label_type;
    char control;
    char monitoring_flag;
    char measurement_phase;
    char measurement_sign;
    char gain_setting_index;
    char spare_1;
    uint16_t spare_2;
    uint16_t progress_cnt;
    uint16_t mix_nr;
    uint16_t dynamic_scan_nr;
    uint16_t cardiac_phase_nr;
    uint16_t echo_nr;
    uint16_t location_nr;
    uint16_t row_nr;
    uint16_t extra_attr_nr;
    uint16_t  measurement_nr;
    uint16_t e1_profile_nr;
    uint16_t e2_profile_nr;
    uint16_t e3_profile_nr;
    uint16_t rf_echo_nr;
    uint16_t grad_echo_nr;
    uint16_t enc_time;
    uint16_t random_phase;
    uint16_t rr_interval;
    uint16_t rtop_offset;
    uint32_t channels_active;
  } old_label;

  typedef struct
  {
    uint32_t data_size;
    uint32_t coded_data_size;
    float normalization_factor;
    uint16_t seq_nr;
    uint16_t label_type;
    char control;
    char monitoring_flag;
    char measurement_phase;
    char measurement_sign;
    char gain_setting_index;
    char raw_format;
    uint16_t spare_1;
    uint16_t progress_cnt;
    uint16_t mix_nr;
    uint16_t dynamic_scan_nr;
    uint16_t cardiac_phase_nr;
    uint16_t echo_nr;
    uint16_t location_nr;
    uint16_t row_nr;
    uint16_t extra_attr_nr;
    uint16_t measurement_nr;
    uint16_t e1_profile_nr;
    uint16_t e2_profile_nr;
    uint16_t e3_profile_nr;
    uint16_t rf_echo_nr;
    uint16_t grad_echo_nr;
    uint16_t enc_time;
    uint16_t random_phase;
    uint16_t rr_interval;
    uint16_t rtop_offset;
    uint32_t channels_active;
  } new_label; //Total size 64 bytes

  typedef struct
  {
    union
    {
      old_label old_;
      new_label new_;
    }; 
  } label; //Total size 64 bytes

}

#endif //PHILIPS_H
