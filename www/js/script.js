// Class to represent a row in the seat reservations grid
function Alarm(name, dow, hour, minute, file) {
    // function Alarm(id, alarm) {
    var self = this;
    self.name = ko.observable(name);
    self.dow = ko.observableArray(dow);
    self.hour = ko.observable(hour);
    self.minute = ko.observable(minute);
    self.file = ko.observable(file);

    self.timeForm = ko.observable(hour + ":" + minute);

    self.exportFormat = function () {
        var obj = new Object();
        obj.name = self.name()
        obj.hour = parseInt(self.timeForm().split(":")[0]);
        obj.minute = parseInt(self.timeForm().split(":")[1]);
        obj.file = self.file().url();
        obj.dow = Array();
        self.dow().forEach(d => {
            obj.dow.push(d.value);
        });
        return obj;
    }
}

function General(time_offset, audio_volume) {
    var self = this;
    self.time_offset = ko.observable(time_offset);
    self.audio_volume = ko.observable(audio_volume);
}

function Song(name, url, size) {
    var self = this;
    self.name = ko.observable(name);
    self.url = ko.observable(url);
    self.size = ko.observable(size);
}

function playbackCommand(req) {
    // return $.ajax({
    //     url: "/api/playback",
    //     type: "POST",
    //     data: req,
    //     contentType: "application/json; charset=utf-8",
    //     dataType: "json"
    // });
    return $.post("/api/playback",
        JSON.stringify(req),
        function () {
            console.log("sent ", req);
        });
}

// Overall viewmodel for this screen, along with initial state
function SettingsViewModel() {
    var self = this;

    // Non-editable catalog data - would come from the server
    self.weekdays = [
        { name: "Sunday", value: 0 },
        { name: "Monday", value: 1 },
        { name: "Tuesday", value: 2 },
        { name: "Wednesday", value: 3 },
        { name: "Thursday", value: 4 },
        { name: "Friday", value: 5 },
        { name: "Saturday", value: 6 }
    ];

    // load files list from SD
    self.songs = ko.observableArray([]);
    $.getJSON("/api/songs", function (allData) {
        console.log("Songs", allData);
        var mappedSongs = $.map(allData, function (a) {
            return new Song(a.name, a.url, a.size);
        });
        self.songs(mappedSongs);
    });

    // get current config
    self.general = ko.observable(new General(0, 0));
    self.alarms = ko.observableArray([]);
    $.getJSON("/api/config", function (allData) {
        // general
        self.general(new General(allData.general.time_offset,
            allData.general.audio_volume));

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
                song)
        });
        self.alarms(mappedAlarms);

        $('.selectpicker').selectpicker();
    });

    // Editable data
    self.alarms = ko.observableArray(self.alarmObjects);

    // Operations
    self.addAlarm = function () {
        self.alarms.push(new Alarm("unnamed", self.weekdays[0], 0, 0, self.songs[0]));
    }
    self.removeAlarm = function (alarm) { self.alarms.remove(alarm) }

    self.save = function () {
        var data = {};
        data.general = self.general();
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
                alert("Saved config!");
            })
            .fail(function (e) {
                console.log(e);
                alert("Error: " + e.responseText);
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
                alert("Deleted song!");
                self.songs.remove(file)
            })
            .fail(function (e) {
                console.log(e);
                alert("Error: " + e.responseText);
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
                alert("Error: " + e.responseText);
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
                alert("Error: " + e.responseText);
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
                alert("Error: " + e.responseText);
            });
    }

    // Audio volume
    self.selectedPlayback = ko.observable();
    self.volume = ko.observable(10);
    self.volume.subscribe(function (value) {
        console.log("New volume: ", value);
        var req = {
            "action": "volume",
            "volume": value
        };

        playbackCommand(req).fail(function (e) {
            console.log(e);
            alert("Error: " + e.responseText);
        });
    });

    // new song
    self.streamName = ko.observable();
    self.streamUrl = ko.observable();
    self.songFile = ko.observable();

    self.addSong = function () {
        console.log(self.songFile());
    }

    self.addStream = function () {
        if (self.streamName() != undefined && self.streamUrl() != undefined) {
            var req = {
                "action": "addStream",
                "stream": {
                    "name": self.streamName(),
                    "url": self.streamUrl()
                }
            };

            $.post("/api/songs",
                JSON.stringify(req),
                function () {
                    console.log("sent ", req);
                }).done(function (resp) {
                    self.songs.push(new Song(self.streamName(), self.streamUrl()));
                    $('#modalStream').modal('hide');
                })
                .fail(function (e) {
                    console.log(e);
                    alert("Error: " + e.responseText);
                });
        }
    }
}

ko.applyBindings(new SettingsViewModel());