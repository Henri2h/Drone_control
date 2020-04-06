console.log("started");

// web socket
var exampleSocket; //= new WebSocket("ws://192.168.0.14:8766");

var connected = false;
var loaded = false;

var Data = [];

var readStatus = false;
var canChangeReadStatus = true;

var iter = 1;
var toRead = 0;

function connect(url) {
    exampleSocket = new WebSocket(url);

    exampleSocket.onmessage = function (event) {
        Data.push(event.data);

        if (Data.length >= toRead) {
            self.postMessage(Data)
            setReadStatusTrue();
        }
    }

    exampleSocket.onerror = function (event) {
        console.log("error : " + event.data);
    }

    exampleSocket.onopen = function (event) {
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
    exampleSocket.send("#FStatus");
    toRead = 18;
}

function getStatus() {
    Data = ["Status"];
    setReadStatusFalse();
    exampleSocket.send("#Status");
    toRead = 22;
}


function getGains(){
    Data = ["Gains"];
    setReadStatusFalse();
    exampleSocket.send("#GetGainsRate");
    toRead = 9;
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
                else if(mode == 8){
                    getGains();
                }
                else {
                    getStatus();
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

