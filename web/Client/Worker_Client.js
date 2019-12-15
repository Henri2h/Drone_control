var exampleSocket = new WebSocket("ws://192.168.0.31:8766");

var Data = [];

var readStatus = false;
var canChangeReadStatus = true;

var iter = 1;


var time_start = new Date();

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


function getStatus() {
    Data = [];
    setReadStatusFalse();
    exampleSocket.send("#FStatus");
}


exampleSocket.onmessage = function (event) {
    document.getElementById("result_" + Data.length).innerHTML = event.data;

    Data.push(event.data);
    if (Data.length >= 17) {
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


/*
onmessage = function (e) {
    console.log('Worker: Message received from main script');
    let result = e.data[0] * e.data[1];
    if (isNaN(result)) {
        postMessage('Please write two numbers');
    } else {
        let workerResult = 'Result: ' + result;
        console.log('Worker: Posting message back to main script');
        postMessage(workerResult);
    }
}
*/

function update() {
    if (readStatus) {
        getStatus();
        iter++;

        document.getElementById("result_iter").innerHTML = iter;
    }
    else {
        console.log("Wait...");
    }
}

var x = setInterval(update, 10);