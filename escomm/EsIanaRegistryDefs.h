/// EsIanaRegistryDefs.h
/// This MACROS-X file is used for IANA registry initialization
/// !DO NOT INCLUDE THIS FILE DIRECTLY IN PROJECTS!
/// it's got implicitly included by EsIanaRegistry.cpp as appropriate
/// IANA_REGISTRY_ENTRY(port, protMask, svc, svcAlias1, svcAlias2, descr)
/// 

IANA_REGISTRY_BEGIN
IANA_REGISTRY_ENTRY( 7,   3, "echo",             "\0",                  "\0",                   "Echo")
IANA_REGISTRY_ENTRY( 9,   3, "discard",          "sink",                "null",                 "Discard")
IANA_REGISTRY_ENTRY( 13,  3, "daytime",          "\0",                  "\0",                   "Daytime")
IANA_REGISTRY_ENTRY( 17,  3, "qotd",             "quote",               "\0",                   "Quote of the day")
IANA_REGISTRY_ENTRY( 19,  3, "chargen",          "ttytst",              "source",               "Character generator")
IANA_REGISTRY_ENTRY( 20,  1, "ftp-data",         "\0",                  "\0",                   "File Transfer")
IANA_REGISTRY_ENTRY( 21,  1, "ftp",              "\0",                  "\0",                   "FTP Control")
IANA_REGISTRY_ENTRY( 22,  3, "ssh",              "\0",                  "\0",                   "Secure Shell Protocol")
IANA_REGISTRY_ENTRY( 23,  1, "telnet",           "\0",                  "\0",                   "Telnet")
IANA_REGISTRY_ENTRY( 25,  1, "smtp",             "mail",                "\0",                   "Simple Mail Transfer")
IANA_REGISTRY_ENTRY( 37,  3, "time",             "\0",                  "\0",                   "Time")
IANA_REGISTRY_ENTRY( 39,  2, "rlp",              "resource",            "\0",                   "Resource Location Protocol")
IANA_REGISTRY_ENTRY( 42,  3, "nameserver",       "name",                "\0",                   "Host Name Server")
IANA_REGISTRY_ENTRY( 43,  1, "nicname",          "whois",               "\0",                   "Who Is")
IANA_REGISTRY_ENTRY( 53,  3, "domain",           "\0",                  "\0",                   "Domain Name Server")
IANA_REGISTRY_ENTRY( 67,  2, "bootps",           "dhcps",               "\0",                   "Bootstrap Protocol Server")
IANA_REGISTRY_ENTRY( 68,  2, "bootpc",           "dhcpc",               "\0",                   "Bootstrap Protocol Client")
IANA_REGISTRY_ENTRY( 69,  2, "tftp",             "\0",                  "\0",                   "Trivial File Transfer")
IANA_REGISTRY_ENTRY( 70,  1, "gopher",           "\0",                  "\0",                   "Gopher")
IANA_REGISTRY_ENTRY( 79,  1, "finger",           "\0",                  "\0",                   "Finger")
IANA_REGISTRY_ENTRY( 80,  3, "http",             "www",                 "\0",                   "World Wide Web")
IANA_REGISTRY_ENTRY( 88,  3, "kerberos",         "krb5",                "\0",                   "Kerberos")
IANA_REGISTRY_ENTRY( 101, 1, "hostname",         "hostnames",           "\0",                   "NIC Host Name Server")
IANA_REGISTRY_ENTRY( 102, 1, "iso-tsap",         "\0",                  "\0",                   "ISO-TSAP Class 0")
IANA_REGISTRY_ENTRY( 107, 1, "rtelnet",          "\0",                  "\0",                   "Remote Telnet Service")
IANA_REGISTRY_ENTRY( 109, 1, "pop2",             "postoffice",          "\0",                   "Post Office Protocol - Version 2")
IANA_REGISTRY_ENTRY( 110, 1, "pop3",             "postoffice",          "\0",                   "Post Office Protocol - Version 3")
IANA_REGISTRY_ENTRY( 111, 3, "sunrpc",           "rpcbind",             "portmap",              "SUN Remote Procedure Call")
IANA_REGISTRY_ENTRY( 113, 1, "auth",             "ident",               "tap",                  "Authentication Sevice")
IANA_REGISTRY_ENTRY( 117, 1, "uucp-path",        "\0",                  "\0",                   "UUCP Path Service")
IANA_REGISTRY_ENTRY( 119, 1, "nntp",             "usenet",              "\0",                   "Network News Transfer Protocol")
IANA_REGISTRY_ENTRY( 123, 2, "ntp",              "\0",                  "\0",                   "Network Time Protocol")
IANA_REGISTRY_ENTRY( 135, 3, "epmap",            "loc-srv",             "\0",                   "DCE endpoint resolution")
IANA_REGISTRY_ENTRY( 137, 3, "netbios-ns",       "nbname",              "\0",                   "NETBIOS Name Service")
IANA_REGISTRY_ENTRY( 138, 2, "netbios-dgm",      "nbdatagram",          "\0",                   "NETBIOS Datagram Service")
IANA_REGISTRY_ENTRY( 139, 1, "netbios-ssn",      "nbsession",           "\0",                   "NETBIOS Session Service")
IANA_REGISTRY_ENTRY( 143, 1, "imap",             "imap4",               "\0",                   "Internet Message Access Protocol")
IANA_REGISTRY_ENTRY( 158, 1, "pcmail-srv",       "repository",          "\0",                   "PC Mail Server")
IANA_REGISTRY_ENTRY( 161, 2, "snmp",             "snmp",                "\0",                   "SNMP")
IANA_REGISTRY_ENTRY( 162, 2, "snmptrap",         "snmp-trap",           "\0",                   "SNMP TRAP")
IANA_REGISTRY_ENTRY( 170, 1, "print-srv",        "\0",                  "\0",                   "Network PostScript")
IANA_REGISTRY_ENTRY( 179, 1, "bgp",              "\0",                  "\0",                   "Border Gateway Protocol")
IANA_REGISTRY_ENTRY( 194, 1, "irc",              "\0",                  "\0",                   "Internet Relay Chat Protocol")
IANA_REGISTRY_ENTRY( 213, 2, "ipx",              "\0",                  "\0",                   "IPX over IP")
IANA_REGISTRY_ENTRY( 389, 1, "ldap",             "\0",                  "\0",                   "Lightweight Directory Access Protocol")
IANA_REGISTRY_ENTRY( 443, 3, "https",            "MCom",                "\0",                   "\0" )
IANA_REGISTRY_ENTRY( 445, 3, "microoft-ds",      "\0",                  "\0",                   "Microsoft CIFS")
IANA_REGISTRY_ENTRY( 464, 3, "kpasswd",          "\0",                  "\0",                   "Kerberos (v5)")
IANA_REGISTRY_ENTRY( 500, 2, "isakmp",           "ike",                 "\0",                   "Internet Key Exchange (IPSec)")
IANA_REGISTRY_ENTRY( 512, 1, "exec",             "\0",                  "\0",                   "Remote Process Execution")
IANA_REGISTRY_ENTRY( 512, 2, "biff",             "comsat",              "\0",                   "Notifies users of new mail")
IANA_REGISTRY_ENTRY( 513, 1, "login",            "\0",                  "\0",                   "Remote Login")
IANA_REGISTRY_ENTRY( 513, 2, "who",              "whod",                "\0",                   "Database of who's logged on, average load")
IANA_REGISTRY_ENTRY( 514, 1, "cmd",              "shell",               "\0",                   "Automatic Authentication")
IANA_REGISTRY_ENTRY( 514, 2, "syslog",           "\0",                  "\0",                   "\0")
IANA_REGISTRY_ENTRY( 515, 1, "printer",          "spooler",             "\0",                   "Listens for incoming connections")
IANA_REGISTRY_ENTRY( 517, 2, "talk",             "\0",                  "\0",                   "Establishes 1 Connection")
IANA_REGISTRY_ENTRY( 518, 2, "ntalk",            "\0",                  "\0",                   "\0")
IANA_REGISTRY_ENTRY( 520, 3, "router",           "routed",              "\0",                   "RIPv.1, RIPv.2")
IANA_REGISTRY_ENTRY( 525, 2, "timed",            "timeserver",          "\0",                   "Timeserver")
IANA_REGISTRY_ENTRY( 526, 1, "tempo",            "newdate",             "\0",                   "Newdate")
IANA_REGISTRY_ENTRY( 530, 3, "courier",          "rpc",                 "\0",                   "RPC")
IANA_REGISTRY_ENTRY( 531, 1, "conference",       "chat",                "\0",                   "IRC Chat")
IANA_REGISTRY_ENTRY( 532, 1, "netnews",          "readnews",            "\0",                   "Readnews")
IANA_REGISTRY_ENTRY( 533, 2, "netwall",          "\0",                  "\0",                   "For emergency broadcasts")
IANA_REGISTRY_ENTRY( 540, 1, "uucp",             "uucpd",               "\0",                   "Uucpd")
IANA_REGISTRY_ENTRY( 543, 1, "klogin",           "\0",                  "\0",                   "Kerberos login")
IANA_REGISTRY_ENTRY( 544, 1, "kshell",           "krcmd",               "\0",                   "Kerberos remote shell")
IANA_REGISTRY_ENTRY( 550, 2, "new-rwho",         "new-who",             "\0",                   "New-who")
IANA_REGISTRY_ENTRY( 556, 1, "remotefs",         "rfs",                 "rfs_server",           "Rfs Server")
IANA_REGISTRY_ENTRY( 560, 2, "rmonitor",         "rmonitord",           "\0",                   "Rmonitor")
IANA_REGISTRY_ENTRY( 561, 2, "monitor",          "\0",                  "\0",                   "\0")
IANA_REGISTRY_ENTRY( 636, 1, "ldaps",            "sldap",               "\0",                   "LDAP over TLS/SSL")
IANA_REGISTRY_ENTRY( 749, 3, "kerberos-adm",     "\0",                  "\0",                   "Kerberos administration")
IANA_REGISTRY_END

#undef IANA_REGISTRY_BEGIN
#undef IANA_REGISTRY_ENTRY
#undef IANA_REGISTRY_END