// EsBluetooth.UUID_defs.h
// This is MACROS-X definition file to create Bluetooth services|profiles UUIDs mappings
// !DO NOT INCLUDE IT IN PROJECT!
// It's implicitly included by EsBluetooth h|cpp files as needed
//
// UUID16_MAP_PROT_ENTRY(sym, val, str)
// UUID16_MAP_PSVC_ENTRY(sym, val, str, isSrvClass, isProf)
//

UUID16_MAP_BEGIN
/// Protocol UUIDs
UUID16_MAP_PROT_BEGIN
UUID16_MAP_PROT_ENTRY(SDP_PROTOCOL,               0x0001, esT("SDP"))
UUID16_MAP_PROT_ENTRY(UDP_PROTOCOL,               0x0002, esT("UDP"))
UUID16_MAP_PROT_ENTRY(RFCOMM_PROTOCOL,            0x0003, esT("RFCOMM"))
UUID16_MAP_PROT_ENTRY(TCP_PROTOCOL,               0x0004, esT("TCP"))
UUID16_MAP_PROT_ENTRY(TCSBIN_PROTOCOL,            0x0005, esT("TCSBIN"))
UUID16_MAP_PROT_ENTRY(TCSAT_PROTOCOL,             0x0006, esT("TCSAT"))
UUID16_MAP_PROT_ENTRY(OBEX_PROTOCOL,              0x0008, esT("OBEX"))
UUID16_MAP_PROT_ENTRY(IP_PROTOCOL,                0x0009, esT("IP"))
UUID16_MAP_PROT_ENTRY(FTP_PROTOCOL,               0x000A, esT("FTP"))
UUID16_MAP_PROT_ENTRY(HTTP_PROTOCOL,              0x000C, esT("HTTP"))
UUID16_MAP_PROT_ENTRY(WSP_PROTOCOL,               0x000E, esT("WSP"))
UUID16_MAP_PROT_ENTRY(BNEP_PROTOCOL,              0x000F, esT("BNEP"))
UUID16_MAP_PROT_ENTRY(UPNP_PROTOCOL,              0x0010, esT("UPnP"))
UUID16_MAP_PROT_ENTRY(HID_PROTOCOL,               0x0011, esT("HID"))
UUID16_MAP_PROT_ENTRY(HCCC_PROTOCOL,              0x0012, esT("HCCC"))
UUID16_MAP_PROT_ENTRY(HCDC_PROTOCOL,              0x0014, esT("HCDC"))
UUID16_MAP_PROT_ENTRY(HCN_PROTOCOL,               0x0016, esT("HCN"))
UUID16_MAP_PROT_ENTRY(AVCTP_PROTOCOL,             0x0017, esT("AVCTP"))
UUID16_MAP_PROT_ENTRY(AVDTP_PROTOCOL,             0x0019, esT("AVDTP"))
UUID16_MAP_PROT_ENTRY(CMPT_PROTOCOL,              0x001B, esT("CMPT"))
UUID16_MAP_PROT_ENTRY(UDI_C_PLANE_PROTOCOL,       0x001D, esT("UDI-C-Plane"))
UUID16_MAP_PROT_ENTRY(MCAPCC_PROTOCOL,            0x001E, esT("MCAPCC"))
UUID16_MAP_PROT_ENTRY(MCAPDC_PROTOCOL,            0x001F, esT("MCAPDC"))
UUID16_MAP_PROT_ENTRY(L2CAP_PROTOCOL,             0x0100, esT("L2CAP"))
UUID16_MAP_PROT_END

///ServicesUUIDs
UUID16_MAP_PSVC_BEGIN
UUID16_MAP_PSVC_ENTRY(ServiceDiscoveryServer,     0x1000, _i("Service discovery server"),                         1, 0)
UUID16_MAP_PSVC_ENTRY(BrowseGroupDescriptor,      0x1001, _i("Browse group descriptor"),                          1, 0)
UUID16_MAP_PSVC_ENTRY(PublicBrowseGroup,          0x1002, _i("Public browse group"),                              1, 0)
UUID16_MAP_PSVC_ENTRY(SerialPort,                 0x1101, _i("Serial Port"),                                      1, 1)
UUID16_MAP_PSVC_ENTRY(LANAccessUsingPPP,          0x1102, _i("LAN Access"),                                       1, 1)
UUID16_MAP_PSVC_ENTRY(DialupNetworking,           0x1103, _i("Dial-up Networking"),                               1, 1)
UUID16_MAP_PSVC_ENTRY(IrMCSync,                   0x1104, _i("Synchronization"),                                  1, 1)
UUID16_MAP_PSVC_ENTRY(OBEXObjectPush,             0x1105, _i("Object Push"),                                      1, 1)
UUID16_MAP_PSVC_ENTRY(OBEXFileTransfer,           0x1106, _i("File Transfer"),                                    1, 1)
UUID16_MAP_PSVC_ENTRY(IrMcSyncCommand,            0x1107, _i("Synchronization Command channel"),                  1, 0)
UUID16_MAP_PSVC_ENTRY(Headset,                    0x1108, _i("Headset"),                                          1, 1)
UUID16_MAP_PSVC_ENTRY(Cordless,                   0x1109, _i("Cordless Telephony"),                               1, 1)
UUID16_MAP_PSVC_ENTRY(AudioSource,                0x110A, _i("Advanced Audio Distribution source"),               1, 0)
UUID16_MAP_PSVC_ENTRY(AudioSink,                  0x110B, _i("Advanced Audio Distribution sink"),                 1, 0)
UUID16_MAP_PSVC_ENTRY(AVRemoteControlTarget,      0x110C, _i("Audio/Video Remote Control target"),                1, 0)
UUID16_MAP_PSVC_ENTRY(AdvancedAudioDistribution,  0x110D, _i("Advanced Audio Distribution"),                      0, 1)
UUID16_MAP_PSVC_ENTRY(AVRemoteControl,            0x110E, _i("Audio/Video Remote Control"),                       1, 1)
UUID16_MAP_PSVC_ENTRY(VideoConferencing,          0x110F, _i("Audio/Video Remote Controller/Video conferencing"), 1, 0)
UUID16_MAP_PSVC_ENTRY(Intercom,                   0x1110, _i("Intercom"),                                         1, 0)
UUID16_MAP_PSVC_ENTRY(Fax,                        0x1111, _i("Fax"),                                              1, 0)
UUID16_MAP_PSVC_ENTRY(HeadsetAudioGateway,        0x1112, _i("Headset audio gateway"),                            1, 0)
UUID16_MAP_PSVC_ENTRY(WAP,                        0x1113, _i("WAP interoperability"),                             1, 0)
UUID16_MAP_PSVC_ENTRY(WAPClient,                  0x1114, _i("WAP client interoperability"),                      1, 0)
UUID16_MAP_PSVC_ENTRY(PANU,                       0x1115, _i("Personal Area Networking User"),                    1, 1)
UUID16_MAP_PSVC_ENTRY(NAP,                        0x1116, _i("Personal Area Networking Network Access Point"),    1, 1)
UUID16_MAP_PSVC_ENTRY(GN,                         0x1117, _i("Personal Area Networking Group ad-hoc Network"),    1, 1)
UUID16_MAP_PSVC_ENTRY(DirectPrinting,             0x1118, _i("Direct Printing"),                                  1, 0)
UUID16_MAP_PSVC_ENTRY(ReferencePrinting,          0x1119, _i("Reference Printing"),                               1, 0)
UUID16_MAP_PSVC_ENTRY(Imaging,                    0x111A, _i("Basic Imaging"),                                    0, 1)
UUID16_MAP_PSVC_ENTRY(ImagingResponder,           0x111B, _i("Imaging responder"),                                1, 0)
UUID16_MAP_PSVC_ENTRY(ImagingAutomaticArchive,    0x111C, _i("Imaging automatic archive"),                        1, 0)
UUID16_MAP_PSVC_ENTRY(ImagingRefObjects,          0x111D, _i("Imaging reference objects"),                        1, 0)
UUID16_MAP_PSVC_ENTRY(Handsfree,                  0x111E, _i("Hands-Free"),                                       1, 1)
UUID16_MAP_PSVC_ENTRY(HandsfreeAudioGateway,      0x111F, _i("Hands-Free audio gateway"),                         1, 0)
UUID16_MAP_PSVC_ENTRY(DirectPrintingRefObjects,   0x1120, _i("Direct printing reference objects"),                1, 0)
UUID16_MAP_PSVC_ENTRY(ReflectedUI,                0x1121, _i("Direct printing UI"),                               1, 0)
UUID16_MAP_PSVC_ENTRY(BasicPrinting,              0x1122, _i("Basic printing"),                                   0, 1)
UUID16_MAP_PSVC_ENTRY(PrintingStatus,             0x1123, _i("Printing status"),                                  1, 0)
UUID16_MAP_PSVC_ENTRY(HumanInterfaceDevice,       0x1124, _i("Human Interface Device"),                           1, 1)
UUID16_MAP_PSVC_ENTRY(HardcopyCableReplacement,   0x1125, _i("Hardcopy cable replacement"),                       0, 1)
UUID16_MAP_PSVC_ENTRY(HCRPrint,                   0x1126, _i("Hardcopy cable replacement-printer"),               1, 0)
UUID16_MAP_PSVC_ENTRY(HCRScan,                    0x1127, _i("Hardcopy cable replacement-scanner"),               1, 0)
UUID16_MAP_PSVC_ENTRY(CommonISDNAccess,           0x1128, _i("Common ISDN Access"),                               1, 1)
UUID16_MAP_PSVC_ENTRY(VideoConferencingGW,        0x1129, _i("Video conferencing gateway"),                       1, 0)
UUID16_MAP_PSVC_ENTRY(UDIMT,                      0x112A, esT("UDIMT"),                                           1, 0)
UUID16_MAP_PSVC_ENTRY(UDITA,                      0x112B, esT("UDITA"),                                           1, 0)
UUID16_MAP_PSVC_ENTRY(AudioVideo,                 0x112C, _i("Audio-Video"),                                      1, 0)
UUID16_MAP_PSVC_ENTRY(SIM_Access,                 0x112D, _i("SIM access"),                                       1, 1)
UUID16_MAP_PSVC_ENTRY(PhonebookAccessClient,      0x112E, _i("Phonebook access client"),                          1, 0)
UUID16_MAP_PSVC_ENTRY(PhonebookAccessServer,      0x112F, _i("Phonebook access server"),                          1, 0)
UUID16_MAP_PSVC_ENTRY(PhonebookAccess,            0x1130, _i("Phonebook access"),                                 0, 1)
UUID16_MAP_PSVC_ENTRY(HeadsetHS,                  0x1131, _i("Headset"),                                          1, 0)
UUID16_MAP_PSVC_ENTRY(MessageAccessServer,        0x1132, _i("Message access server"),                            1, 0)
UUID16_MAP_PSVC_ENTRY(MessageNotificationServer,  0x1133, _i("Message notification server"),                      1, 0)
UUID16_MAP_PSVC_ENTRY(MessageAccess,              0x1134, _i("Message access"),                                   0, 1)
UUID16_MAP_PSVC_ENTRY(GlobalNavSatSystem,         0x1135, _i("Global navigation satellite system"),               0, 1)
UUID16_MAP_PSVC_ENTRY(GlobalNavSatSystemServer,   0x1136, _i("Global navigation satellite system server"),        1, 0)
UUID16_MAP_PSVC_ENTRY(Display3d,                  0x1137, _i("3D Synchronization - display"),                     1, 0)
UUID16_MAP_PSVC_ENTRY(Glasses3d,                  0x1138, _i("3D Synchronization - glasses"),                     1, 0)
UUID16_MAP_PSVC_ENTRY(Synchronization3d,          0x1139, _i("3D Synchronization"),                               0, 1)
UUID16_MAP_PSVC_ENTRY(MultiprofileSpecProfile,    0x113A, _i("Multi-Profile Specification profile"),              0, 1)
UUID16_MAP_PSVC_ENTRY(MultiprofileSpecService,    0x113B, _i("Multi-Profile Specification service"),              1, 0)
UUID16_MAP_PSVC_ENTRY(PnPInformation,             0x1200, _i("PnP information"),                                  1, 1)
UUID16_MAP_PSVC_ENTRY(GenericNetworking,          0x1201, _i("Generic networking"),                               1, 0)
UUID16_MAP_PSVC_ENTRY(GenericFileTransfer,        0x1202, _i("Generic file transfer"),                            1, 0)
UUID16_MAP_PSVC_ENTRY(GenericAudio,               0x1203, _i("Generic audio"),                                    1, 0)
UUID16_MAP_PSVC_ENTRY(GenericTelephony,           0x1204, _i("Generic telephony"),                                1, 0)
UUID16_MAP_PSVC_ENTRY(UPNP_Service,               0x1205, esT("ESDP UPnP"),                                       1, 0)
UUID16_MAP_PSVC_ENTRY(UPNP_IP_Service,            0x1206, esT("ESDP UPnP IP"),                                    1, 0)
UUID16_MAP_PSVC_ENTRY(ESDP_UPNP_IP_PAN,           0x1300, esT("ESDP UPnP IP PAN"),                                1, 0)
UUID16_MAP_PSVC_ENTRY(ESDP_UPNP_IP_LAP,           0x1301, esT("ESDP UPnP IP LAP"),                                1, 0)
UUID16_MAP_PSVC_ENTRY(ESDP_UPNP_L2CAP,            0x1302, esT("ESDP UPnP L2CAP"),                                 1, 0)
UUID16_MAP_PSVC_ENTRY(VideoSource,                0x1303, _i("Video source"),                                     1, 0)
UUID16_MAP_PSVC_ENTRY(VideoSink,                  0x1304, _i("Video sink"),                                       1, 0)
UUID16_MAP_PSVC_ENTRY(VideoDistribution,          0x1305, _i("Video distribution"),                               0, 1)
UUID16_MAP_PSVC_ENTRY(HDP,                        0x1400, _i("Health device"),                                    0, 1)
UUID16_MAP_PSVC_ENTRY(HDP_Source,                 0x1401, _i("Health device source"),                             1, 0)
UUID16_MAP_PSVC_ENTRY(HDP_Sink,                   0x1402, _i("Health device sink"),                               1, 0)
UUID16_MAP_PSVC_END
UUID16_MAP_END

#undef UUID16_MAP_BEGIN
#undef UUID16_MAP_PROT_BEGIN
#undef UUID16_MAP_PROT_ENTRY
#undef UUID16_MAP_PROT_END
#undef UUID16_MAP_PSVC_BEGIN
#undef UUID16_MAP_PSVC_ENTRY
#undef UUID16_MAP_PSVC_END
#undef UUID16_MAP_END
