Plotly.plot('tester', [{
    y: [1, 2, 3].map(rand),
    mode: 'lines',
    line: { color: '#80CAF6' }
}, {
    y: [1, 2, 3].map(rand),
    mode: 'lines',
    line: { color: '#DF56F1' }
}]);

function rand() {
    return Math.random();
}

var cnt = 0;


var time_start_ = new Date();
var iteration = 1;

var interval = setInterval(function () {

    Plotly.extendTraces('tester', {
         y: [[rand()], [rand()]]
     }, [0, 1])
 
     if (cnt === 100) clearInterval(interval);
 
    var t = new Date();
    var dt = t - time_start_;
    iteration++;
    console.log(iteration / dt * 1000);
}, 1);

