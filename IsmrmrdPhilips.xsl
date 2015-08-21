<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:output method="xml" indent="yes"/>

  <xsl:variable name="readoutOversampling">
    <xsl:value-of select="philips/oversample_factors/value[1]" />
  </xsl:variable>

  <xsl:variable name="phaseOversampling">
    <xsl:value-of select="philips/oversample_factors/value[2]" />
  </xsl:variable>

  <xsl:variable name="sliceOversampling">
    <xsl:value-of select="philips/oversample_factors/value[3]" />
  </xsl:variable>


  <xsl:variable name="studyID">STUDY</xsl:variable>
  <xsl:variable name="patientID">PATIENT</xsl:variable>

  <xsl:variable name="strSeperator">_</xsl:variable>

  <xsl:template match="/">
    <ismrmrdHeader xsi:schemaLocation="http://www.ismrm.org/ISMRMRD ismrmrd.xsd"
                   xmlns="http://www.ismrm.org/ISMRMRD"
                   xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                   xmlns:xs="http://www.w3.org/2001/XMLSchema">

      <measurementInformation>
        <measurementID>
          <xsl:value-of select="concat($patientID, $strSeperator, $studyID, $strSeperator, string(siemens/HEADER/MeasUID))"/>
        </measurementID>
	<patientPosition>HFS</patientPosition> <!-- TODO: must be calculated somehow -->
        <protocolName>
		<xsl:for-each select="philips/scan_name/value"><xsl:value-of select="." /><xsl:choose><xsl:when test="position() != last()"><xsl:text> </xsl:text></xsl:when></xsl:choose></xsl:for-each>
        </protocolName>
      </measurementInformation>

      <acquisitionSystemInformation>
        <systemVendor>PHILIPS</systemVendor>
        <relativeReceiverNoiseBandwidth>0.793</relativeReceiverNoiseBandwidth>
        <receiverChannels>
          <xsl:value-of select="philips/nr_measured_channels/value" />
        </receiverChannels>
	
	<!-- Coil Labels -->
	<xsl:for-each select="philips/channel_names">
	  <coilLabel>
	    <coilNumber><xsl:value-of select="./@idx1"/></coilNumber>
	    <coilName><xsl:value-of select="./value"/></coilName>
	  </coilLabel>
	</xsl:for-each>
      </acquisitionSystemInformation>

      <experimentalConditions>
        <H1resonanceFrequency_Hz>0</H1resonanceFrequency_Hz>
      </experimentalConditions>

      <encoding>
        <trajectory>cartesian</trajectory>
        <encodedSpace>
          <matrixSize>
            <x>
              <xsl:value-of select="philips/recon_resolutions/value[1] * $readoutOversampling"/>
            </x>
            <y>
              <xsl:value-of select="philips/recon_resolutions/value[2] * $phaseOversampling"/>
            </y>
            <z>
              <xsl:value-of select="philips/recon_resolutions/value[3] * $sliceOversampling"/>
            </z>
          </matrixSize>

          <fieldOfView_mm>
            <x>
              <xsl:value-of select="philips/voxel_sizes/value[1] * philips/recon_resolutions/value[1] * $readoutOversampling"/>
            </x>
            <y>
              <xsl:value-of select="philips/voxel_sizes/value[2] * philips/recon_resolutions/value[2] * $phaseOversampling"/>
            </y>
            <z>
              <xsl:value-of select="philips/voxel_sizes/value[3] * philips/recon_resolutions/value[3] * $sliceOversampling"/>
            </z>		  
          </fieldOfView_mm>
        </encodedSpace>
        <reconSpace>
          <matrixSize>
            <x>
              <xsl:value-of select="philips/recon_resolutions/value[1]"/>
            </x>
            <y>
              <xsl:value-of select="philips/recon_resolutions/value[2]"/>
            </y>
            <z>
              <xsl:value-of select="philips/recon_resolutions/value[3]"/>
            </z>
          </matrixSize>

          <fieldOfView_mm>
            <x>
              <xsl:value-of select="philips/voxel_sizes/value[1] * philips/recon_resolutions/value[1]"/>
            </x>
            <y>
              <xsl:value-of select="philips/voxel_sizes/value[2] * philips/recon_resolutions/value[2]"/>
            </y>
            <z>
              <xsl:value-of select="philips/voxel_sizes/value[3] * philips/recon_resolutions/value[3]"/>
            </z>		  
          </fieldOfView_mm>
        </reconSpace>
        <encodingLimits>
          <kspace_encoding_step_1>
            <minimum>0</minimum>
            <maximum>
	      <xsl:value-of select="philips/max_encoding_numbers/value[2]-philips/min_encoding_numbers/value[2]"/>
            </maximum>
            <center>
	      <xsl:value-of select="floor((philips/max_encoding_numbers/value[2]-philips/min_encoding_numbers/value[2]) div 2)"/>
            </center>
          </kspace_encoding_step_1>
          <kspace_encoding_step_2>
            <minimum>0</minimum>
            <maximum>
	      <xsl:value-of select="philips/max_encoding_numbers/value[3]-philips/min_encoding_numbers/value[3]"/>
            </maximum>
            <center>
	      <xsl:value-of select="floor((philips/max_encoding_numbers/value[3]-philips/min_encoding_numbers/value[3]) div 2)"/>
            </center>
          </kspace_encoding_step_2>
          <slice>
            <minimum>0</minimum>
            <maximum>
              <xsl:value-of select="philips/nr_locations/value[1] - 1"/>
            </maximum>
            <center>0</center>
          </slice>
          <set>
            <minimum>0</minimum>
            <maximum>
              <xsl:value-of select="philips/nr_rows/value - 1"/>
            </maximum>
            <center>0</center>
          </set>
          <phase>
            <minimum>0</minimum>
            <maximum>
              <xsl:value-of select="philips/nr_cardiac_phases/value - 1"/>
            </maximum>
            <center>0</center>
          </phase>
          <repetition>
            <minimum>0</minimum>
	    <maximum>
              <xsl:value-of select="philips/nr_dynamic_scans/value - 1"/>
	    </maximum>
            <center>0</center>
	  </repetition>
          <segment>
            <minimum>0</minimum>
            <maximum>0</maximum>
            <center>0</center>
          </segment>
          <contrast>
            <minimum>0</minimum>
	    <maximum>
              <xsl:value-of select="philips/nr_echoes/value - 1"/>
	    </maximum>
            <center>0</center>
          </contrast>
          <average>
            <minimum>0</minimum>
	    <maximum>
              <xsl:value-of select="philips/nr_measurements/value - 1"/>
	    </maximum>
            <center>0</center>
          </average>
        </encodingLimits>
      </encoding>

      <sequenceParameters>
	<xsl:for-each select="philips/repetition_times/value">
	  <TR>ETH</TR>
        </xsl:for-each>
	<xsl:for-each select="philips/echo_times/value">
	  <TE>
            <xsl:value-of select="." />
	  </TE>
        </xsl:for-each>
	<xsl:for-each select="philips/inversion_delays/value">
	  <TI>
            <xsl:value-of select="." />
	  </TI>
        </xsl:for-each>
	<xsl:for-each select="philips/flip_angles/value">
	  <flipAngle_deg>
            <xsl:value-of select="." />
	  </flipAngle_deg>
        </xsl:for-each>
      </sequenceParameters>
    </ismrmrdHeader>
  </xsl:template>
</xsl:stylesheet>
