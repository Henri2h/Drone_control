console.log("started");

// web socket
var socket; //= new WebSocket("ws://192.168.0.14:8766");

var connected = false;
var loaded = false;

var Data = [];

var readStatus = false;
var canChangeReadStatus = true;

var iter = 1;
var toRead = 0;

function connect(url) {
    socket = new WebSocket(url);

    socket.onmessage = function (event) {
        Data.push(event.data);
        console.log("Pos :", Data.length - 2, event.data); // remove firs item from count

        if (Data.length - 1 >= toRead) {
            self.postMessage(Data)
            setReadStatusTrue();

            console.log("Done");
        }
    }

    socket.onerror = function (event) {
        console.log("error : " + event.data);
    }

    socket.onopen = function (event) {
        loaded = true;
        console.log("Socket open, set read status true");
        setReadStatusTrue();
    }

    connected = true;
}

self.onmessage = function (event) {
    this.console.log("Event : " + event.data);
    this.connect(event.data);
}

function setReadStatusTrue() {
    if (readStatus == false && canChangeReadStatus) {
        readStatus = true;
    }
}
function setReadStatusFalse() {
    if (canChangeReadStatus) {
        readStatus = false;
    }
}

function getFStatus() {
    Data = ["FStatus"];
    setReadStatusFalse();
    socket.send("#FStatus");
    toRead = 16 + 1;
}

function getStatus() {
    Data = ["Status"];
    setReadStatusFalse();
    socket.send("#Status");
    toRead = 20 + 1;
}


function getGains() {
    Data = ["Gains"];
    setReadStatusFalse();
    socket.send("#GetGainsRate");
    toRead = 9 + 1;
}



var mode = 0;
function update() {
    if (connected) {
        if (iter >= 0) {
            if (readStatus) {

                mode++;
                if (mode < 8) {
                    getFStatus();
                }
                else if (mode == 8) {
                    getStatus();
                }
                else {
                    getGains();
                    mode = 0;
                }
                iter++;
            }
            else {
                // console.log("Wait...");
            }
        }
        else {
            Data[0] = iter;
            setReadStatusTrue();
        }
    }
}

var x = setInterval(update, 0);

