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
    dst: "DST",
    alarm_volume: "Alarm audio volume",
    alarms: "Alarms",
    name: "Name",
    weekday: "Weekday",
    time: "Time",
    song: "Song/Stream/FM",
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
    sunday: "sunday",
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
    dst: "Sommerzeit",
    alarm_volume: "Alarm Lautstärke",
    alarms: "Wecker",
    name: "Name",
    weekday: "Wochentag",
    time: "Zeit",
    song: "Lied/Stream/FM",
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

function getLanguage(){
    var lang = navigator.language || navigator.userLanguage;
    console.log(lang);
    if(lang === "de-DE"){
        return german;
    }
    return english;
}

function modalAlert(msg){
    $("#modalAlertBody").html(msg);
    $('#modalAlert').modal();
}

function modalSuccess(msg){
    $("#modalSuccessBody").html(msg);
    $('#modalSuccess').modal();
}


function Alarm(name, dow, hour, minute, file) {
    // function Alarm(id, alarm) {
    var self = this;
    self.name = ko.observable(name);
    self.dow = ko.observableArray(dow);
    self.hour = ko.observable(hour);
    self.minute = ko.observable(minute);
    self.file = ko.observable(file);

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
        obj.file = self.file().url();
        obj.type = self.file().type();
        obj.dow = Array();
        self.dow().forEach(d => {
            obj.dow.push(d.value);
        });
        return obj;
    }
}

function General(gmt_offset, dst_offset, audio_volume) {
    var self = this;
    self.gmt_offset = ko.observable(gmt_offset);
    self.dst_offset = ko.observable(dst_offset);

    self.gmt_offset_h = ko.pureComputed({
        read: function () {
            return self.gmt_offset() / 3600;
        },
        write: function (value) {
            self.gmt_offset(value * 3600);
        },
        owner: self
    });

    self.dst_offset_h = ko.pureComputed({
        read: function () {
            return self.dst_offset() == 0 ? false : true;
        },
        write: function (value) {
            console.log(value);
            if (value) {
                self.dst_offset(3600);
            } else {
                self.dst_offset(0);
            }
        },
        owner: self
    });

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

    self.name_extended = ko.computed(function(){
        if (self.type() == "fm") {
            return self.name() + " [FM: " + self.url() + " MHz]";
        }
        if (self.type() == "stream") {
            return self.name() + " [stream]";
        }
        return self.name();
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

    // load files list from SD
    self.songs = ko.observableArray([]);
    $.getJSON("/api/songs", function (allData) {
        console.log("Songs", allData);
        var mappedSongs = $.map(allData, function (a) {
            return new Song(a.name, a.url, a.size, a.type);
        });
        self.songs(mappedSongs);
    });

    // get current config
    self.general = ko.observable(new General(0, 0, 0));
    self.network = ko.observable(new Network(false));
    self.alarms = ko.observableArray([]);

    $.getJSON("/api/config", function (allData) {
        // general
        self.general(new General(allData.general.gmt_offset,
            allData.general.dst_offset,
            allData.general.audio_volume));
        // network
        if(allData.network){
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
                song);
        });
        self.alarms(mappedAlarms);

        $('.selectpicker').selectpicker();
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
        self.alarms.push(new Alarm("new Alarm", [self.weekdays[0]], 0, 0, self.songs[0]));
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


        console.log(req);

        $.post("/api/songs",
            JSON.stringify(req),
            function () {
                console.log("sent ", req);
            })
            .done(function (resp) {
                modalAlert("Deleted song!");
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

        console.log("Play ", song)

        var req = {
            "action": "play",
            "url": song
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
    self.volume = ko.observable(10);
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
                    "url": self.streamUrl(),
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
window.setInterval(model.playbackUpdate, 1000);
// model.playbackUpdate();
ko.applyBindings(model);