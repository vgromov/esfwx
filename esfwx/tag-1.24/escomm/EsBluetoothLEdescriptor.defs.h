// Bluetooth LE known descriptors XMACRO database
// Do not include it explicitly in project, it's being implicitly included from
// EsBluetoothLE code
//
ES_BLE_DESCRIPTOR_BEGIN
ES_BLE_DESCRIPTOR_ENTRY(ExtendedProperties,       _i("Characteristic Extended Properties"),   0x2900)
ES_BLE_DESCRIPTOR_ENTRY(UserDescription,          _i("Characteristic User Description"),      0x2901)
ES_BLE_DESCRIPTOR_ENTRY(ClientConfiguration,      _i("Client Characteristic Configuration"),  0x2902)
ES_BLE_DESCRIPTOR_ENTRY(ServerConfiguration,      _i("Server Characteristic Configuration"),  0x2903)
ES_BLE_DESCRIPTOR_ENTRY(PresentationFormat,       _i("Characteristic Format"),                0x2904)
ES_BLE_DESCRIPTOR_ENTRY(AggregateFormat,          _i("Characteristic Aggregate Format"),      0x2905)
ES_BLE_DESCRIPTOR_ENTRY(ValidRange,               _i("Valid Range"),                          0x2906)
ES_BLE_DESCRIPTOR_ENTRY(ExternalReportReference,  _i("External Report Reference"),            0x2907)
ES_BLE_DESCRIPTOR_ENTRY(ReportReference,          _i("Report Reference"),                     0x2908)
ES_BLE_DESCRIPTOR_END

#undef ES_BLE_DESCRIPTOR_BEGIN
#undef ES_BLE_DESCRIPTOR_ENTRY
#undef ES_BLE_DESCRIPTOR_END

