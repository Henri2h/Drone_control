
//As a worker normally take another JavaScript file to execute we convert the function in an URL: http://stackoverflow.com/a/16799132/2576706
function getScriptPath(foo) { return window.URL.createObjectURL(new Blob([foo.toString().match(/^\s*function\s*\(\s*\)\s*\{(([\s\S](?!\}$))*[\s\S])/)[1]], { type: 'text/javascript' })); }

var MAX_VALUE = 10000;

/*
 *	Here are the workers
 */
//Worker 1
var worker1 = new Worker(getScriptPath(function () {
  self.addEventListener('message', function (e) {
    var value = 0;
    while (value <= e.data) {
      self.postMessage(value);
      value++;
    }
  }, false);
}));
//We add a listener to the worker to get the response and show it in the page
worker1.addEventListener('message', function (e) {
  document.getElementById("result1").innerHTML = e.data;
}, false);


//Worker 2
var worker2 = new Worker(getScriptPath(function () {
  self.addEventListener('message', function (e) {
    var value = 0;
    while (value <= e.data) {
      self.postMessage(value);
      value++;
    }
  }, false);
}));
worker2.addEventListener('message', function (e) {
  document.getElementById("result2").innerHTML = e.data;
}, false);


//Worker 3
var worker3 = new Worker(getScriptPath(function () {
  self.addEventListener('message', function (e) {
    var value = 0;
    while (value <= e.data) {
      self.postMessage(value);
      value++;
    }

    exampleSocket.onerror = function (event) {
      console.log("error : " + event.data);
    }

    var loaded = false;
    exampleSocket.onopen = function (event) {
      loaded = true;
      console.log("loaded");
    }

  }, false);
}));
worker3.addEventListener('message', function (e) {
  document.getElementById("result3").innerHTML = e.data;
}, false);








console.log("start");

// On instantie le Worker
var monWorker = new Worker('Worker_Client.js');
// On se prépare à traiter le message de retour qui sera
// renvoyé par le worker
monWorker.addEventListener("message", function (event) {

  console.log("Worker has returned data");

}, false);

// On démarre le worker en lui envoyant un 1er message
monWorker.postMessage("Start_server");

// On stoppe le worker via la commande terminate()
// monWorker.terminate();


