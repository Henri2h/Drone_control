console.log("started");
var exampleSocket = new WebSocket("ws://192.168.0.100:8766");

var Data = [];

var readStatus = false;
var canChangeReadStatus = true;

var iter = 1;
var time_start = new Date();
var toRead = 0;

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

    var now = new Date();
    var dt = now - time_start;

    if (iter != 0) {

        //console.log(dt + " : " + iter + " : " + iter / dt * 1000);
    }
}



function getStatus() {
    Data = ["Status"];
    setReadStatusFalse();
    exampleSocket.send("#Status");
    toRead = 22;

    var now = new Date();
    var dt = now - time_start;

    if (iter != 0) {

        //console.log(dt + " : " + iter + " : " + iter / dt * 1000);
    }
}

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

var loaded = false;
exampleSocket.onopen = function (event) {
    loaded = true;
    console.log("Socket open, set read status true");
    setReadStatusTrue();
    time_start = new Date();
}


var mode = true;
function update() {
if(iter >= 0){    if (readStatus) {
        if (mode == true) {
            mode = false;
            getStatus();
        }
        else {
            mode = true;
            getFStatus();
        }
        iter++;
    }
    else {
        // console.log("Wait...");
    }}
	else{
	Data[0] = iter;
setReadStatusTrue();	
	}
}

var x = setInterval(update, 1);

