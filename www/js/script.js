// function loadConfig(){
//     $.getJSON("config.json", function( data ) {
//         var items = [];
//         $.each( data, function( key, val ) {
//           items.push( "<li id='" + key + "'>" + val + "</li>" );
//         });

//         $( "<ul/>", {
//           "class": "my-new-list",
//           html: items.join( "" )
//         }).appendTo( "body" );
//       });
// }

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
        obj.dow = self.dow();
        obj.hour = parseInt(self.timeForm().split(":")[0]);
        obj.minute = parseInt(self.timeForm().split(":")[1]);
        obj.file = self.file().name;
        return obj;
    }
}

function General(time_offset, audio_volume) {
    var self = this;
    self.time_offset = ko.observable(time_offset);
    self.audio_volume = ko.observable(audio_volume);
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
    self.files = ko.observableArray([]);
    $.getJSON("/api/files", function (allData) {
        console.log("Files", allData);
        self.files(allData);
    });

    // get current config
    self.general = ko.observable(new General(0, 0));
    self.alarms = ko.observableArray([]);
    $.getJSON("/api/config", function (allData) {
        // general
        self.general(new General(allData.general.time_offset,
            allData.general.audio_volume));

        var mappedAlarms = $.map(allData.alarms, function (a) {
            var dowArray = Array();
            a.dow.forEach(d => {
                dowArray.push(self.weekdays[d]);
            });

            return new Alarm(a.name,
                dowArray,
                a.hour,
                a.minute,
                self.files().find(f => f.name === a.file))
        });
        self.alarms(mappedAlarms);

        $('.selectpicker').selectpicker();
    });

    // Editable data
    self.alarms = ko.observableArray(self.alarmObjects);

    // Operations
    self.addAlarm = function () {
        self.alarms.push(new Alarm("unnamed", self.weekdays[0], 0, 0, self.files[0]));
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

    self.removeFile = function (file) {
        console.log(ko.toJSON(file));

        var jqxhr = $.post("/api/files/delete",
            ko.toJSON(file),
            function () {
                console.log("delete file ", file);
            })
            .done(function (resp) {
                alert("Deleted file!");
                self.files.remove(file)
            })
            .fail(function (e) {
                console.log(e);
                alert("Error: " + e.responseText);
        });
    }
}

ko.applyBindings(new SettingsViewModel());

$(document).ready(function() {
    $('#multiple-checkboxes').multiselect({
      includeSelectAllOption: true,
    });

    console.log("READY!");
});