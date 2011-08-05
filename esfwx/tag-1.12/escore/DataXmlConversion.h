#ifndef _data_xml_conversion_h_
#define _data_xml_conversion_h_

// data file xml convertion to misc formats
//
namespace DataXmlConversion
{
EKOSF_CORE_FUNC	wxString toXhtml(const wxXmlDocument& doc);
EKOSF_CORE_FUNC	wxString toCsv(const wxXmlDocument& doc);
}

#endif // _data_xml_conversion_h_
