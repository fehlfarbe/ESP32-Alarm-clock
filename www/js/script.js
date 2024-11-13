var english = {
    settings: "Settings",
    network_settings: "Network settings",
    network_hostname: "Hostname",
    network_static_enabled: "Static IP",
    network_static_ip: "Static IP",
    network_subnet: "Subnet mask",
    network_gateway: "Gateway",
    network_primary_dns: "Primary DNS",
    network_secondary_dns: "Secondary DNS",
    timezone: "Timezone",
    alarm_volume: "Alarm audio volume",
    alarms: "Alarms",
    name: "Name",
    weekday: "Weekday",
    time: "Time",
    songs: "Audio sources",
    delete: "Delete",
    save_config: "Save Config",
    url: "URL",
    size: "Size",
    action: "Action",
    add_stream: "Add stream",
    add_song: "Add song",
    add_fm: "Add fm",
    file: "File",
    add: "Add",
    close: "Close",
    monday: "Monday",
    tuesday: "Tuesday",
    wednesday: "Wednesday",
    thursday: "Thursday",
    friday: "Friday",
    saturday: "Saturday",
    sunday: "Sunday",
    fm_frequency: "frequency"
}

var german = {
    settings: "Einstellungen",
    network_settings: "Netzwerkeinstellungen",
    network_hostname: "Hostname",
    network_static_enabled: "Statische IP",
    network_static_ip: "Statische IP",
    network_subnet: "Subnetzmaske",
    network_gateway: "Gateway",
    network_primary_dns: "Priärer DNS",
    network_secondary_dns: "Secondärer DNS",
    timezone: "Zeitzone",
    alarm_volume: "Alarm Lautstärke",
    alarms: "Wecker",
    name: "Name",
    weekday: "Wochentag",
    time: "Zeit",
    songs: "Audioquellen",
    delete: "Löschen",
    save_config: "Einstellungen speichern",
    url: "URL",
    size: "Größe",
    action: "Aktion",
    add_stream: "Stream hinzufügen",
    add_song: "Lied hinzufügen",
    add_fm: "Radiosender hinzufügen",
    file: "Datei",
    add: "Hinzufügen",
    close: "Schließen",
    monday: "Montag",
    tuesday: "Dienstag",
    wednesday: "Mittwoch",
    thursday: "Donnerstag",
    friday: "Freitag",
    saturday: "Samstag",
    sunday: "Sonntag",
    fm_frequency: "Frequenz"
}

// POSIX time zones from https://github.com/nayarsystems/posix_tz_db/blob/master/zones.json
var timezones = {
    "Africa/Abidjan": "GMT0",
    "Africa/Accra": "GMT0",
    "Africa/Addis_Ababa": "EAT-3",
    "Africa/Algiers": "CET-1",
    "Africa/Asmara": "EAT-3",
    "Africa/Bamako": "GMT0",
    "Africa/Bangui": "WAT-1",
    "Africa/Banjul": "GMT0",
    "Africa/Bissau": "GMT0",
    "Africa/Blantyre": "CAT-2",
    "Africa/Brazzaville": "WAT-1",
    "Africa/Bujumbura": "CAT-2",
    "Africa/Cairo": "EET-2EEST,M4.5.5/0,M10.5.4/24",
    "Africa/Casablanca": "<+01>-1",
    "Africa/Ceuta": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Africa/Conakry": "GMT0",
    "Africa/Dakar": "GMT0",
    "Africa/Dar_es_Salaam": "EAT-3",
    "Africa/Djibouti": "EAT-3",
    "Africa/Douala": "WAT-1",
    "Africa/El_Aaiun": "<+01>-1",
    "Africa/Freetown": "GMT0",
    "Africa/Gaborone": "CAT-2",
    "Africa/Harare": "CAT-2",
    "Africa/Johannesburg": "SAST-2",
    "Africa/Juba": "CAT-2",
    "Africa/Kampala": "EAT-3",
    "Africa/Khartoum": "CAT-2",
    "Africa/Kigali": "CAT-2",
    "Africa/Kinshasa": "WAT-1",
    "Africa/Lagos": "WAT-1",
    "Africa/Libreville": "WAT-1",
    "Africa/Lome": "GMT0",
    "Africa/Luanda": "WAT-1",
    "Africa/Lubumbashi": "CAT-2",
    "Africa/Lusaka": "CAT-2",
    "Africa/Malabo": "WAT-1",
    "Africa/Maputo": "CAT-2",
    "Africa/Maseru": "SAST-2",
    "Africa/Mbabane": "SAST-2",
    "Africa/Mogadishu": "EAT-3",
    "Africa/Monrovia": "GMT0",
    "Africa/Nairobi": "EAT-3",
    "Africa/Ndjamena": "WAT-1",
    "Africa/Niamey": "WAT-1",
    "Africa/Nouakchott": "GMT0",
    "Africa/Ouagadougou": "GMT0",
    "Africa/Porto-Novo": "WAT-1",
    "Africa/Sao_Tome": "GMT0",
    "Africa/Tripoli": "EET-2",
    "Africa/Tunis": "CET-1",
    "Africa/Windhoek": "CAT-2",
    "America/Adak": "HST10HDT,M3.2.0,M11.1.0",
    "America/Anchorage": "AKST9AKDT,M3.2.0,M11.1.0",
    "America/Anguilla": "AST4",
    "America/Antigua": "AST4",
    "America/Araguaina": "<-03>3",
    "America/Argentina/Buenos_Aires": "<-03>3",
    "America/Argentina/Catamarca": "<-03>3",
    "America/Argentina/Cordoba": "<-03>3",
    "America/Argentina/Jujuy": "<-03>3",
    "America/Argentina/La_Rioja": "<-03>3",
    "America/Argentina/Mendoza": "<-03>3",
    "America/Argentina/Rio_Gallegos": "<-03>3",
    "America/Argentina/Salta": "<-03>3",
    "America/Argentina/San_Juan": "<-03>3",
    "America/Argentina/San_Luis": "<-03>3",
    "America/Argentina/Tucuman": "<-03>3",
    "America/Argentina/Ushuaia": "<-03>3",
    "America/Aruba": "AST4",
    "America/Asuncion": "<-04>4<-03>,M10.1.0/0,M3.4.0/0",
    "America/Atikokan": "EST5",
    "America/Bahia": "<-03>3",
    "America/Bahia_Banderas": "CST6",
    "America/Barbados": "AST4",
    "America/Belem": "<-03>3",
    "America/Belize": "CST6",
    "America/Blanc-Sablon": "AST4",
    "America/Boa_Vista": "<-04>4",
    "America/Bogota": "<-05>5",
    "America/Boise": "MST7MDT,M3.2.0,M11.1.0",
    "America/Cambridge_Bay": "MST7MDT,M3.2.0,M11.1.0",
    "America/Campo_Grande": "<-04>4",
    "America/Cancun": "EST5",
    "America/Caracas": "<-04>4",
    "America/Cayenne": "<-03>3",
    "America/Cayman": "EST5",
    "America/Chicago": "CST6CDT,M3.2.0,M11.1.0",
    "America/Chihuahua": "CST6",
    "America/Costa_Rica": "CST6",
    "America/Creston": "MST7",
    "America/Cuiaba": "<-04>4",
    "America/Curacao": "AST4",
    "America/Danmarkshavn": "GMT0",
    "America/Dawson": "MST7",
    "America/Dawson_Creek": "MST7",
    "America/Denver": "MST7MDT,M3.2.0,M11.1.0",
    "America/Detroit": "EST5EDT,M3.2.0,M11.1.0",
    "America/Dominica": "AST4",
    "America/Edmonton": "MST7MDT,M3.2.0,M11.1.0",
    "America/Eirunepe": "<-05>5",
    "America/El_Salvador": "CST6",
    "America/Fort_Nelson": "MST7",
    "America/Fortaleza": "<-03>3",
    "America/Glace_Bay": "AST4ADT,M3.2.0,M11.1.0",
    "America/Godthab": "<-02>2<-01>,M3.5.0/-1,M10.5.0/0",
    "America/Goose_Bay": "AST4ADT,M3.2.0,M11.1.0",
    "America/Grand_Turk": "EST5EDT,M3.2.0,M11.1.0",
    "America/Grenada": "AST4",
    "America/Guadeloupe": "AST4",
    "America/Guatemala": "CST6",
    "America/Guayaquil": "<-05>5",
    "America/Guyana": "<-04>4",
    "America/Halifax": "AST4ADT,M3.2.0,M11.1.0",
    "America/Havana": "CST5CDT,M3.2.0/0,M11.1.0/1",
    "America/Hermosillo": "MST7",
    "America/Indiana/Indianapolis": "EST5EDT,M3.2.0,M11.1.0",
    "America/Indiana/Knox": "CST6CDT,M3.2.0,M11.1.0",
    "America/Indiana/Marengo": "EST5EDT,M3.2.0,M11.1.0",
    "America/Indiana/Petersburg": "EST5EDT,M3.2.0,M11.1.0",
    "America/Indiana/Tell_City": "CST6CDT,M3.2.0,M11.1.0",
    "America/Indiana/Vevay": "EST5EDT,M3.2.0,M11.1.0",
    "America/Indiana/Vincennes": "EST5EDT,M3.2.0,M11.1.0",
    "America/Indiana/Winamac": "EST5EDT,M3.2.0,M11.1.0",
    "America/Inuvik": "MST7MDT,M3.2.0,M11.1.0",
    "America/Iqaluit": "EST5EDT,M3.2.0,M11.1.0",
    "America/Jamaica": "EST5",
    "America/Juneau": "AKST9AKDT,M3.2.0,M11.1.0",
    "America/Kentucky/Louisville": "EST5EDT,M3.2.0,M11.1.0",
    "America/Kentucky/Monticello": "EST5EDT,M3.2.0,M11.1.0",
    "America/Kralendijk": "AST4",
    "America/La_Paz": "<-04>4",
    "America/Lima": "<-05>5",
    "America/Los_Angeles": "PST8PDT,M3.2.0,M11.1.0",
    "America/Lower_Princes": "AST4",
    "America/Maceio": "<-03>3",
    "America/Managua": "CST6",
    "America/Manaus": "<-04>4",
    "America/Marigot": "AST4",
    "America/Martinique": "AST4",
    "America/Matamoros": "CST6CDT,M3.2.0,M11.1.0",
    "America/Mazatlan": "MST7",
    "America/Menominee": "CST6CDT,M3.2.0,M11.1.0",
    "America/Merida": "CST6",
    "America/Metlakatla": "AKST9AKDT,M3.2.0,M11.1.0",
    "America/Mexico_City": "CST6",
    "America/Miquelon": "<-03>3<-02>,M3.2.0,M11.1.0",
    "America/Moncton": "AST4ADT,M3.2.0,M11.1.0",
    "America/Monterrey": "CST6",
    "America/Montevideo": "<-03>3",
    "America/Montreal": "EST5EDT,M3.2.0,M11.1.0",
    "America/Montserrat": "AST4",
    "America/Nassau": "EST5EDT,M3.2.0,M11.1.0",
    "America/New_York": "EST5EDT,M3.2.0,M11.1.0",
    "America/Nipigon": "EST5EDT,M3.2.0,M11.1.0",
    "America/Nome": "AKST9AKDT,M3.2.0,M11.1.0",
    "America/Noronha": "<-02>2",
    "America/North_Dakota/Beulah": "CST6CDT,M3.2.0,M11.1.0",
    "America/North_Dakota/Center": "CST6CDT,M3.2.0,M11.1.0",
    "America/North_Dakota/New_Salem": "CST6CDT,M3.2.0,M11.1.0",
    "America/Nuuk": "<-02>2<-01>,M3.5.0/-1,M10.5.0/0",
    "America/Ojinaga": "CST6CDT,M3.2.0,M11.1.0",
    "America/Panama": "EST5",
    "America/Pangnirtung": "EST5EDT,M3.2.0,M11.1.0",
    "America/Paramaribo": "<-03>3",
    "America/Phoenix": "MST7",
    "America/Port-au-Prince": "EST5EDT,M3.2.0,M11.1.0",
    "America/Port_of_Spain": "AST4",
    "America/Porto_Velho": "<-04>4",
    "America/Puerto_Rico": "AST4",
    "America/Punta_Arenas": "<-03>3",
    "America/Rainy_River": "CST6CDT,M3.2.0,M11.1.0",
    "America/Rankin_Inlet": "CST6CDT,M3.2.0,M11.1.0",
    "America/Recife": "<-03>3",
    "America/Regina": "CST6",
    "America/Resolute": "CST6CDT,M3.2.0,M11.1.0",
    "America/Rio_Branco": "<-05>5",
    "America/Santarem": "<-03>3",
    "America/Santiago": "<-04>4<-03>,M9.1.6/24,M4.1.6/24",
    "America/Santo_Domingo": "AST4",
    "America/Sao_Paulo": "<-03>3",
    "America/Scoresbysund": "<-01>1<+00>,M3.5.0/0,M10.5.0/1",
    "America/Sitka": "AKST9AKDT,M3.2.0,M11.1.0",
    "America/St_Barthelemy": "AST4",
    "America/St_Johns": "NST3:30NDT,M3.2.0,M11.1.0",
    "America/St_Kitts": "AST4",
    "America/St_Lucia": "AST4",
    "America/St_Thomas": "AST4",
    "America/St_Vincent": "AST4",
    "America/Swift_Current": "CST6",
    "America/Tegucigalpa": "CST6",
    "America/Thule": "AST4ADT,M3.2.0,M11.1.0",
    "America/Thunder_Bay": "EST5EDT,M3.2.0,M11.1.0",
    "America/Tijuana": "PST8PDT,M3.2.0,M11.1.0",
    "America/Toronto": "EST5EDT,M3.2.0,M11.1.0",
    "America/Tortola": "AST4",
    "America/Vancouver": "PST8PDT,M3.2.0,M11.1.0",
    "America/Whitehorse": "MST7",
    "America/Winnipeg": "CST6CDT,M3.2.0,M11.1.0",
    "America/Yakutat": "AKST9AKDT,M3.2.0,M11.1.0",
    "America/Yellowknife": "MST7MDT,M3.2.0,M11.1.0",
    "Antarctica/Casey": "<+11>-11",
    "Antarctica/Davis": "<+07>-7",
    "Antarctica/DumontDUrville": "<+10>-10",
    "Antarctica/Macquarie": "AEST-10AEDT,M10.1.0,M4.1.0/3",
    "Antarctica/Mawson": "<+05>-5",
    "Antarctica/McMurdo": "NZST-12NZDT,M9.5.0,M4.1.0/3",
    "Antarctica/Palmer": "<-03>3",
    "Antarctica/Rothera": "<-03>3",
    "Antarctica/Syowa": "<+03>-3",
    "Antarctica/Troll": "<+00>0<+02>-2,M3.5.0/1,M10.5.0/3",
    "Antarctica/Vostok": "<+06>-6",
    "Arctic/Longyearbyen": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Asia/Aden": "<+03>-3",
    "Asia/Almaty": "<+06>-6",
    "Asia/Amman": "<+03>-3",
    "Asia/Anadyr": "<+12>-12",
    "Asia/Aqtau": "<+05>-5",
    "Asia/Aqtobe": "<+05>-5",
    "Asia/Ashgabat": "<+05>-5",
    "Asia/Atyrau": "<+05>-5",
    "Asia/Baghdad": "<+03>-3",
    "Asia/Bahrain": "<+03>-3",
    "Asia/Baku": "<+04>-4",
    "Asia/Bangkok": "<+07>-7",
    "Asia/Barnaul": "<+07>-7",
    "Asia/Beirut": "EET-2EEST,M3.5.0/0,M10.5.0/0",
    "Asia/Bishkek": "<+06>-6",
    "Asia/Brunei": "<+08>-8",
    "Asia/Chita": "<+09>-9",
    "Asia/Choibalsan": "<+08>-8",
    "Asia/Colombo": "<+0530>-5:30",
    "Asia/Damascus": "<+03>-3",
    "Asia/Dhaka": "<+06>-6",
    "Asia/Dili": "<+09>-9",
    "Asia/Dubai": "<+04>-4",
    "Asia/Dushanbe": "<+05>-5",
    "Asia/Famagusta": "EET-2EEST,M3.5.0/3,M10.5.0/4",
    "Asia/Gaza": "EET-2EEST,M3.4.4/50,M10.4.4/50",
    "Asia/Hebron": "EET-2EEST,M3.4.4/50,M10.4.4/50",
    "Asia/Ho_Chi_Minh": "<+07>-7",
    "Asia/Hong_Kong": "HKT-8",
    "Asia/Hovd": "<+07>-7",
    "Asia/Irkutsk": "<+08>-8",
    "Asia/Jakarta": "WIB-7",
    "Asia/Jayapura": "WIT-9",
    "Asia/Jerusalem": "IST-2IDT,M3.4.4/26,M10.5.0",
    "Asia/Kabul": "<+0430>-4:30",
    "Asia/Kamchatka": "<+12>-12",
    "Asia/Karachi": "PKT-5",
    "Asia/Kathmandu": "<+0545>-5:45",
    "Asia/Khandyga": "<+09>-9",
    "Asia/Kolkata": "IST-5:30",
    "Asia/Krasnoyarsk": "<+07>-7",
    "Asia/Kuala_Lumpur": "<+08>-8",
    "Asia/Kuching": "<+08>-8",
    "Asia/Kuwait": "<+03>-3",
    "Asia/Macau": "CST-8",
    "Asia/Magadan": "<+11>-11",
    "Asia/Makassar": "WITA-8",
    "Asia/Manila": "PST-8",
    "Asia/Muscat": "<+04>-4",
    "Asia/Nicosia": "EET-2EEST,M3.5.0/3,M10.5.0/4",
    "Asia/Novokuznetsk": "<+07>-7",
    "Asia/Novosibirsk": "<+07>-7",
    "Asia/Omsk": "<+06>-6",
    "Asia/Oral": "<+05>-5",
    "Asia/Phnom_Penh": "<+07>-7",
    "Asia/Pontianak": "WIB-7",
    "Asia/Pyongyang": "KST-9",
    "Asia/Qatar": "<+03>-3",
    "Asia/Qyzylorda": "<+05>-5",
    "Asia/Riyadh": "<+03>-3",
    "Asia/Sakhalin": "<+11>-11",
    "Asia/Samarkand": "<+05>-5",
    "Asia/Seoul": "KST-9",
    "Asia/Shanghai": "CST-8",
    "Asia/Singapore": "<+08>-8",
    "Asia/Srednekolymsk": "<+11>-11",
    "Asia/Taipei": "CST-8",
    "Asia/Tashkent": "<+05>-5",
    "Asia/Tbilisi": "<+04>-4",
    "Asia/Tehran": "<+0330>-3:30",
    "Asia/Thimphu": "<+06>-6",
    "Asia/Tokyo": "JST-9",
    "Asia/Tomsk": "<+07>-7",
    "Asia/Ulaanbaatar": "<+08>-8",
    "Asia/Urumqi": "<+06>-6",
    "Asia/Ust-Nera": "<+10>-10",
    "Asia/Vientiane": "<+07>-7",
    "Asia/Vladivostok": "<+10>-10",
    "Asia/Yakutsk": "<+09>-9",
    "Asia/Yangon": "<+0630>-6:30",
    "Asia/Yekaterinburg": "<+05>-5",
    "Asia/Yerevan": "<+04>-4",
    "Atlantic/Azores": "<-01>1<+00>,M3.5.0/0,M10.5.0/1",
    "Atlantic/Bermuda": "AST4ADT,M3.2.0,M11.1.0",
    "Atlantic/Canary": "WET0WEST,M3.5.0/1,M10.5.0",
    "Atlantic/Cape_Verde": "<-01>1",
    "Atlantic/Faroe": "WET0WEST,M3.5.0/1,M10.5.0",
    "Atlantic/Madeira": "WET0WEST,M3.5.0/1,M10.5.0",
    "Atlantic/Reykjavik": "GMT0",
    "Atlantic/South_Georgia": "<-02>2",
    "Atlantic/St_Helena": "GMT0",
    "Atlantic/Stanley": "<-03>3",
    "Australia/Adelaide": "ACST-9:30ACDT,M10.1.0,M4.1.0/3",
    "Australia/Brisbane": "AEST-10",
    "Australia/Broken_Hill": "ACST-9:30ACDT,M10.1.0,M4.1.0/3",
    "Australia/Currie": "AEST-10AEDT,M10.1.0,M4.1.0/3",
    "Australia/Darwin": "ACST-9:30",
    "Australia/Eucla": "<+0845>-8:45",
    "Australia/Hobart": "AEST-10AEDT,M10.1.0,M4.1.0/3",
    "Australia/Lindeman": "AEST-10",
    "Australia/Lord_Howe": "<+1030>-10:30<+11>-11,M10.1.0,M4.1.0",
    "Australia/Melbourne": "AEST-10AEDT,M10.1.0,M4.1.0/3",
    "Australia/Perth": "AWST-8",
    "Australia/Sydney": "AEST-10AEDT,M10.1.0,M4.1.0/3",
    // "Etc/GMT": "GMT0",
    // "Etc/GMT+0": "GMT0",
    // "Etc/GMT+1": "<-01>1",
    // "Etc/GMT+10": "<-10>10",
    // "Etc/GMT+11": "<-11>11",
    // "Etc/GMT+12": "<-12>12",
    // "Etc/GMT+2": "<-02>2",
    // "Etc/GMT+3": "<-03>3",
    // "Etc/GMT+4": "<-04>4",
    // "Etc/GMT+5": "<-05>5",
    // "Etc/GMT+6": "<-06>6",
    // "Etc/GMT+7": "<-07>7",
    // "Etc/GMT+8": "<-08>8",
    // "Etc/GMT+9": "<-09>9",
    // "Etc/GMT-0": "GMT0",
    // "Etc/GMT-1": "<+01>-1",
    // "Etc/GMT-10": "<+10>-10",
    // "Etc/GMT-11": "<+11>-11",
    // "Etc/GMT-12": "<+12>-12",
    // "Etc/GMT-13": "<+13>-13",
    // "Etc/GMT-14": "<+14>-14",
    // "Etc/GMT-2": "<+02>-2",
    // "Etc/GMT-3": "<+03>-3",
    // "Etc/GMT-4": "<+04>-4",
    // "Etc/GMT-5": "<+05>-5",
    // "Etc/GMT-6": "<+06>-6",
    // "Etc/GMT-7": "<+07>-7",
    // "Etc/GMT-8": "<+08>-8",
    // "Etc/GMT-9": "<+09>-9",
    // "Etc/GMT0": "GMT0",
    // "Etc/Greenwich": "GMT0",
    // "Etc/UCT": "UTC0",
    // "Etc/UTC": "UTC0",
    // "Etc/Universal": "UTC0",
    // "Etc/Zulu": "UTC0",
    "Europe/Amsterdam": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Andorra": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Astrakhan": "<+04>-4",
    "Europe/Athens": "EET-2EEST,M3.5.0/3,M10.5.0/4",
    "Europe/Belgrade": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Berlin": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Bratislava": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Brussels": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Bucharest": "EET-2EEST,M3.5.0/3,M10.5.0/4",
    "Europe/Budapest": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Busingen": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Chisinau": "EET-2EEST,M3.5.0,M10.5.0/3",
    "Europe/Copenhagen": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Dublin": "IST-1GMT0,M10.5.0,M3.5.0/1",
    "Europe/Gibraltar": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Guernsey": "GMT0BST,M3.5.0/1,M10.5.0",
    "Europe/Helsinki": "EET-2EEST,M3.5.0/3,M10.5.0/4",
    "Europe/Isle_of_Man": "GMT0BST,M3.5.0/1,M10.5.0",
    "Europe/Istanbul": "<+03>-3",
    "Europe/Jersey": "GMT0BST,M3.5.0/1,M10.5.0",
    "Europe/Kaliningrad": "EET-2",
    "Europe/Kiev": "EET-2EEST,M3.5.0/3,M10.5.0/4",
    "Europe/Kirov": "MSK-3",
    "Europe/Lisbon": "WET0WEST,M3.5.0/1,M10.5.0",
    "Europe/Ljubljana": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/London": "GMT0BST,M3.5.0/1,M10.5.0",
    "Europe/Luxembourg": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Madrid": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Malta": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Mariehamn": "EET-2EEST,M3.5.0/3,M10.5.0/4",
    "Europe/Minsk": "<+03>-3",
    "Europe/Monaco": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Moscow": "MSK-3",
    "Europe/Oslo": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Paris": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Podgorica": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Prague": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Riga": "EET-2EEST,M3.5.0/3,M10.5.0/4",
    "Europe/Rome": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Samara": "<+04>-4",
    "Europe/San_Marino": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Sarajevo": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Saratov": "<+04>-4",
    "Europe/Simferopol": "MSK-3",
    "Europe/Skopje": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Sofia": "EET-2EEST,M3.5.0/3,M10.5.0/4",
    "Europe/Stockholm": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Tallinn": "EET-2EEST,M3.5.0/3,M10.5.0/4",
    "Europe/Tirane": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Ulyanovsk": "<+04>-4",
    "Europe/Uzhgorod": "EET-2EEST,M3.5.0/3,M10.5.0/4",
    "Europe/Vaduz": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Vatican": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Vienna": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Vilnius": "EET-2EEST,M3.5.0/3,M10.5.0/4",
    "Europe/Volgograd": "MSK-3",
    "Europe/Warsaw": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Zagreb": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Europe/Zaporozhye": "EET-2EEST,M3.5.0/3,M10.5.0/4",
    "Europe/Zurich": "CET-1CEST,M3.5.0,M10.5.0/3",
    "Indian/Antananarivo": "EAT-3",
    "Indian/Chagos": "<+06>-6",
    "Indian/Christmas": "<+07>-7",
    "Indian/Cocos": "<+0630>-6:30",
    "Indian/Comoro": "EAT-3",
    "Indian/Kerguelen": "<+05>-5",
    "Indian/Mahe": "<+04>-4",
    "Indian/Maldives": "<+05>-5",
    "Indian/Mauritius": "<+04>-4",
    "Indian/Mayotte": "EAT-3",
    "Indian/Reunion": "<+04>-4",
    "Pacific/Apia": "<+13>-13",
    "Pacific/Auckland": "NZST-12NZDT,M9.5.0,M4.1.0/3",
    "Pacific/Bougainville": "<+11>-11",
    "Pacific/Chatham": "<+1245>-12:45<+1345>,M9.5.0/2:45,M4.1.0/3:45",
    "Pacific/Chuuk": "<+10>-10",
    "Pacific/Easter": "<-06>6<-05>,M9.1.6/22,M4.1.6/22",
    "Pacific/Efate": "<+11>-11",
    "Pacific/Enderbury": "<+13>-13",
    "Pacific/Fakaofo": "<+13>-13",
    "Pacific/Fiji": "<+12>-12",
    "Pacific/Funafuti": "<+12>-12",
    "Pacific/Galapagos": "<-06>6",
    "Pacific/Gambier": "<-09>9",
    "Pacific/Guadalcanal": "<+11>-11",
    "Pacific/Guam": "ChST-10",
    "Pacific/Honolulu": "HST10",
    "Pacific/Kiritimati": "<+14>-14",
    "Pacific/Kosrae": "<+11>-11",
    "Pacific/Kwajalein": "<+12>-12",
    "Pacific/Majuro": "<+12>-12",
    "Pacific/Marquesas": "<-0930>9:30",
    "Pacific/Midway": "SST11",
    "Pacific/Nauru": "<+12>-12",
    "Pacific/Niue": "<-11>11",
    "Pacific/Norfolk": "<+11>-11<+12>,M10.1.0,M4.1.0/3",
    "Pacific/Noumea": "<+11>-11",
    "Pacific/Pago_Pago": "SST11",
    "Pacific/Palau": "<+09>-9",
    "Pacific/Pitcairn": "<-08>8",
    "Pacific/Pohnpei": "<+11>-11",
    "Pacific/Port_Moresby": "<+10>-10",
    "Pacific/Rarotonga": "<-10>10",
    "Pacific/Saipan": "ChST-10",
    "Pacific/Tahiti": "<-10>10",
    "Pacific/Tarawa": "<+12>-12",
    "Pacific/Tongatapu": "<+13>-13",
    "Pacific/Wake": "<+12>-12",
    "Pacific/Wallis": "<+12>-12"
}

function getLanguage() {
    var lang = navigator.language || navigator.userLanguage;
    console.log(lang);
    if (lang === "de-DE") {
        return german;
    }
    return english;
}

function modalAlert(msg) {
    $("#modalAlertBody").html(msg);
    $('#modalAlert').modal();
}

function modalSuccess(msg) {
    $("#modalSuccessBody").html(msg);
    $('#modalSuccess').modal();
}


function Alarm(name, dow, hour, minute, file, enabled) {
    // function Alarm(id, alarm) {
    var self = this;
    self.name = ko.observable(name);
    self.dow = ko.observableArray(dow);
    self.hour = ko.observable(hour);
    self.minute = ko.observable(minute);
    self.file = ko.observable(file);
    self.enabled = ko.observable(enabled);

    self.timeForm = ko.pureComputed({
        read: function () {
            return ((self.hour() < 10 ? '0' + self.hour() : self.hour()) || '00')
                + ':' +
                ((self.minute() < 10 ? '0' + self.minute() : self.minute()) || '00')
            return self.hour() + ":" + self.minute();
        },
        write: function (value) {
            console.log("Set time value to " + value);
            self.hour(parseInt(value.split(":")[0]));
            self.minute(parseInt(value.split(":")[1]));
            console.log("new hour: " + self.hour() + " min: " + self.minute());
        },
        owner: self
    });

    self.exportFormat = function () {
        var obj = new Object();
        obj.name = self.name()
        obj.hour = self.hour();
        obj.minute = self.minute();
        obj.file = encodeURIComponent(self.file().url());
        obj.type = self.file().type();
        obj.dow = Array();
        self.dow().forEach(d => {
            obj.dow.push(d.value);
        });
        obj.enabled = self.enabled();
        return obj;
    }
}

function General(timezone, audio_volume) {
    var self = this;
    self.tz = ko.observable(timezone);
    self.audio_volume = ko.observable(audio_volume);
}

function Song(name, url, size, type) {
    var self = this;
    self.name = ko.observable(name);
    self.url = ko.observable(url);
    self.size = ko.observable(size);
    self.type = ko.observable(type);

    self.url_typed = ko.computed(function () {
        if (self.type() == "fm") {
            return "FM: " + self.url() + " MHz";
        }
        return self.url();
    }, this);

    self.name_extended = ko.computed(function () {
        if (self.type() == "fm") {
            return self.name() + " [FM: " + self.url() + " MHz]";
        }
        if (self.type() == "stream") {
            return self.name() + " [stream]";
        }
        return self.name();
    }, this);

    self.size_mib = ko.computed(function () {
        if (self.size()) {
            return (self.size() / (1024 * 1024)).toFixed(2) + " MiB";
        }

        return "";
    }, this);
}

function Network(hostname, static_ip_enabled, static_ip, subnet, gateway, primary_dns, secondary_dns) {
    var self = this;
    self.hostname = ko.observable(hostname);
    self.static_ip_enabled = ko.observable(static_ip_enabled);
    self.static_ip = ko.observable(static_ip);
    self.subnet = ko.observable(subnet);
    self.gateway = ko.observable(gateway);
    self.primary_dns = ko.observable(primary_dns);
    self.secondary_dns = ko.observable(secondary_dns);
}

function Playback(song, playing, current, position, duration, volume) {
    var self = this;
    self.song = ko.observable(song);
    self.playing = ko.observable(playing);
    self.current = ko.observable(current);
    self.position = ko.observable(position);
    self.duration = ko.observable(duration);
    self.volume = ko.observable(volume);

    self.volume.subscribe(function (value) {
        console.log("New volume: ", value);
        var req = {
            "action": "volume",
            "volume": parseFloat(value)
        };

        playbackCommand(req).fail(function (e) {
            console.log(e);
            modalAlert("Error: " + e.responseText);
        });
    });

    self.fmt_time = function (s) {
        var hour = parseInt(s / 3600);
        var minute = parseInt(s / 60) % 60;
        var second = s % 60;
        var text = "";
        if (hour > 0) {
            text += hour < 10 ? '0' + hour : hour;
            text += ":";
        }
        text += minute < 10 ? '0' + minute : minute;
        text += ":";
        text += second < 10 ? '0' + second : second;
        return text;
    }

    self.status = ko.computed(function () {
        var text = self.fmt_time(self.current());
        if (self.duration() > 0) {
            text += "/" + self.fmt_time(self.duration());
        }
        return text;
    }, this);
}

function playbackCommand(req) {
    return $.post("/api/playback",
        JSON.stringify(req),
        function () {
            console.log("sent ", req);
        });
}

// Overall viewmodel for this screen, along with initial state
function SettingsViewModel() {
    var self = this;

    // i18n
    self.l = ko.observable(getLanguage());

    // Non-editable catalog data - would come from the server
    self.weekdays = [
        { name: self.l().sunday, value: 0 },
        { name: self.l().monday, value: 1 },
        { name: self.l().tuesday, value: 2 },
        { name: self.l().wednesday, value: 3 },
        { name: self.l().thursday, value: 4 },
        { name: self.l().friday, value: 5 },
        { name: self.l().saturday, value: 6 }
    ];

    // time zones
    self.timezones = ko.computed(function () {
        var opts = [];
        for (var key in timezones) {
            if (timezones.hasOwnProperty(key)) {
                opts.push({ name: key, value: key + "|" + timezones[key] });
            }
        }
        return opts;
    });

    self.general = ko.observable(null);
    self.network = ko.observable(new Network(false));
    self.alarms = ko.observableArray([]);

    // load files list from SD
    self.songsLoaded = ko.observable(false);
    self.songs = ko.observableArray([]);
    $.getJSON("/api/songs", function (allData) {
        console.log("Songs", allData);
        var mappedSongs = $.map(allData, function (a) {
            return new Song(a.name, a.url, a.size, a.type);
        });
        self.songs(mappedSongs);
        self.songsLoaded(true);

        // when songs are loaded, load config
        $.getJSON("/api/config", function (allData) {
            // general
            self.general(new General(
                allData.general.tz,
                allData.general.audio_volume));
            // network
            if (allData.network) {
                self.network(new Network(
                    allData.network.hostname,
                    allData.network.static_ip_enabled,
                    allData.network.static_ip,
                    allData.network.subnet,
                    allData.network.gateway,
                    allData.network.primary_dns,
                    allData.network.secondary_dns,
                ));
            } else {
                self.network(new Network());
            }

            // alarms
            var mappedAlarms = $.map(allData.alarms, function (a) {
                var dowArray = Array();
                a.dow.forEach(d => {
                    dowArray.push(self.weekdays[d]);
                });

                // compare songs by url or use first of array
                var song = self.songs().find(f => f.url() === a.file);
                if (song == undefined) {
                    song = self.songs()[0];
                }

                return new Alarm(a.name,
                    dowArray,
                    a.hour,
                    a.minute,
                    song,
                    a.enabled);
            });
            self.alarms(mappedAlarms);

            $('.selectpicker').selectpicker();
        });
    });


    // playback
    self.playback = ko.observable(new Playback(null, false, 0, 0));
    self.playbackUpdate = function () {
        $.getJSON("/api/playback", function (data) {
            // playback
            self.playback().song(null);
            self.playback().playing(data.playing);
            self.playback().current(data.current);
            self.playback().position(data.position);
            self.playback().duration(data.duration);
            self.playback().volume(data.volume);
        })
    };

    // Operations
    self.addAlarm = function () {
        self.alarms.push(new Alarm("new Alarm", [self.weekdays[0]], 0, 0, self.songs[0], true));
        $('.selectpicker').selectpicker();
    }
    self.removeAlarm = function (alarm) { self.alarms.remove(alarm) }

    self.save = function () {
        var data = {};
        data.general = self.general();
        data.network = self.network();
        data.alarms = [];
        self.alarms().forEach(a => {
            data.alarms.push(a.exportFormat());
        });

        console.log(ko.toJSON(data));

        var jqxhr = $.post("/api/config",
            ko.toJSON(data),
            function () {
                console.log("sent!");
            })
            .done(function (resp) {
                modalSuccess("Saved config!");
            })
            .fail(function (e) {
                console.log(e);
                modalAlert("Error: " + e.responseText);
            });
    };

    self.removeSong = function (file) {
        var req = {
            "action": "delete",
            "song": ko.toJS(file)
        }

        // urlencode elemets
        if (req?.song?.url) {
            req.song.url = encodeURIComponent(req.song.url);
            req.song.url_typed = encodeURIComponent(req.song.url);
        }

        console.log(req);

        $.post("/api/songs",
            JSON.stringify(req),
            function () {
                console.log("sent ", req);
            })
            .done(function (resp) {
                modalSuccess("Deleted song!");
                self.songs.remove(file)
            })
            .fail(function (e) {
                console.log(e);
                modalAlert("Error: " + e.responseText);
            });
    }

    self.playSong = function (file) {
        var song = null;
        if (file === self) {
            console.log("Playback");
            song = self.selectedPlayback();
        } else {
            song = file.url();
        }
        var volume = self.playback().volume();

        console.log("Play ", song, " with volume ", volume)

        var req = {
            "action": "play",
            "url": encodeURIComponent(song),
            "volume": volume
        };

        playbackCommand(req).done(function (resp) {
            console.log("started song!");
        })
            .fail(function (e) {
                console.log(e);
                modalAlert("Error: " + e.responseText);
            });
    }

    self.stopSong = function (file) {
        var req = {
            "action": "stop",
        };

        playbackCommand(req).done(function (resp) {
            console.log("stopped song!");
        })
            .fail(function (e) {
                console.log(e);
                modalAlert("Error: " + e.responseText);
            });
    }

    self.pauseSong = function (file) {
        var req = {
            "action": "pause",
        };

        playbackCommand(req).done(function (resp) {
            console.log("toggled song!");
        })
            .fail(function (e) {
                console.log(e);
                modalAlert("Error: " + e.responseText);
            });
    }

    // Playback
    self.selectedPlayback = ko.observable();

    // new song
    self.streamName = ko.observable();
    self.streamUrl = ko.observable();
    self.songFile = ko.observable();
    self.songProgress = ko.observable(0);
    self.fmName = ko.observable();
    self.fmFreq = ko.observable(100);

    self.addSong = function () {
        console.log(self.songFile());
        // console.log($("#song_file")[0]);
        var formData = new FormData();
        // formData.append($("#song_file")[0].files[0].name, $("#song_file")[0].files[0]);
        formData.append('file', $("#song_file")[0].files[0]);

        $.ajax({
            type: 'POST',
            url: "/upload",
            data: formData,
            processData: false,
            contentType: false,
            dataType: 'json',
            success: function (data, textStatus, jqXHR) {
                console.log("done", data);
                var song = self.songs().find(obj => {
                    return obj.url() === data.url;
                });
                // update exisiting songs filesize or add new song to list
                if (song !== undefined) {
                    console.log("File aready in list (was overwrittn). Update size");
                    // update songs size
                    song.size(data.size);
                } else {
                    self.songs.push(new Song(data.name, data.url, data.size, "file"));
                }
                $('#modalSong').modal('hide');
                self.songProgress(0);
            },
            error: function (e, textStatus, errorThrown) {
                console.log("Error: ", e);
                console.log(e.responseText);
                modalAlert("Error: " + e.responseText);
            },
            xhr: function () {
                var xhr = new window.XMLHttpRequest();
                xhr.upload.addEventListener("progress", function (evt) {
                    if (evt.lengthComputable) {
                        var percentComplete = (evt.loaded / evt.total) * 100;
                        self.songProgress(percentComplete);
                    }
                }, false);
                return xhr;
            },
        });
    }

    self.addStream = function () {
        if (self.streamName() != undefined && self.streamUrl() != undefined) {
            var req = {
                "action": "addStream",
                "stream": {
                    "name": self.streamName(),
                    "url": encodeURIComponent(self.streamUrl()),
                    "type": "stream"
                }
            };

            $.post("/api/songs",
                JSON.stringify(req),
                function () {
                    console.log("sent ", req);
                }).done(function (resp) {
                    self.songs.push(new Song(self.streamName(), self.streamUrl(), null, "stream"));
                    $('#modalStream').modal('hide');
                })
                .fail(function (e) {
                    console.log(e);
                    modalAlert("Error: " + e.responseText);
                });
        }
    }

    self.addFM = function () {
        if (self.fmName() != undefined && self.fmFreq() != undefined) {
            var req = {
                "action": "addStream",
                "stream": {
                    "name": self.fmName(),
                    "url": self.fmFreq(),
                    "type": "fm"
                }
            };

            $.post("/api/songs",
                JSON.stringify(req),
                function () {
                    console.log("sent ", req);
                }).done(function (resp) {
                    self.songs.push(new Song(self.fmName(), self.fmFreq(), null, "fm"));
                    $('#modalFM').modal('hide');
                })
                .fail(function (e) {
                    console.log(e);
                    modalAlert("Error: " + e.responseText);
                });
        }
    }
}

var model = new SettingsViewModel();
// window.setInterval(model.playbackUpdate, 1000);
model.playbackUpdate();
ko.applyBindings(model);