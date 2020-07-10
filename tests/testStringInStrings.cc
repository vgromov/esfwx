// String-in-strings tests
//

TEST(EsStringInStringsTest, Collection) {
  EsString::Array sa = {
    esT("Refresh"), 
    esT("Re-enumerate connected spectrometers"), 
    esT("Stop all"), 
    esT("Stop all running spectrometer devices"), 
    esT("Device"), 
    esT("File"), 
    esT("Application File commands"), 
    esT("Load default calibration"), 
    esT("Load default analyzer"), 
    esT("Save spectrum analyzer to file and make it default"), 
    esT("Save data as..."), 
    esT("Export data"), 
    esT("Close"), 
    esT("Exit|Exit the application"), 
    esT("Previous frame"), 
    esT("Next frame"), 
    esT("View"), 
    esT("Help"), 
    esT("Toggle application automatic update"), 
    esT("Load spectrum analyzer object from file"), 
    esT("Import"), 
    esT("Wave lengths"), 
    esT("Select preview source:"), 
    esT("Make device selected"), 
    esT("Use Sensitivity Calibration"), 
    esT("Back"), 
    esT("Toggle bookmark 0"), 
    esT("Toggle bookmark 1"), 
    esT("Toggle bookmark 2"), 
    esT("Toggle bookmark 3"), 
    esT("Toggle bookmark 4"), 
    esT("Toggle bookmark 5"), 
    esT("Toggle bookmark 6"), 
    esT("Toggle bookmark 7"), 
    esT("Toggle bookmark 8"), 
    esT("Toggle bookmark 9"), 
    esT("Goto bookmark"), 
    esT("X"), 
    esT("Y"), 
    esT("Zoom"), 
    esT("Zoom Out X"), 
    esT("Wavelengths (nm)")
  };

  EsVariant vsa = sa;
  EsVariant vstr;

  vstr = EsString(esT("Device"));
  EXPECT_TRUE(vsa.has(vstr));

  vstr = EsString(esT("Languages"));
  EXPECT_FALSE(vsa.has(vstr));
}
