
if(localStorage.getItem("websocket_url") == null){
    localStorage.websocket_url = "ws://"+window.location.hostname+":8766";
}

console.log("Started chart setup");

var E_Gyration_plot = document.getElementById('gyrationPlot'); // chart
var E_Acceleration_plot = document.getElementById('accelerationPlot'); // chart

var worker = new Worker('js/Worker_Client.js');
worker.postMessage(localStorage.websocket_url);
var iter = 0;
Accel = []


var time_start = new Date();
// Setup an event listener that will handle messages received from the worker.
worker.addEventListener('message', function (e) {
    // Log the workers message.
    if (e.data[0] == "FStatus") {
        //parallel(e.data);

        //addPointAccel(e.data);
        //addPointGyr(e.data);

        Accel.push([parseFloat(e.data[9]), parseFloat(e.data[10]), parseFloat(e.data[11])]);

        vueApp.accel_x = parseFloat(e.data[9]).toPrecision(4);
        vueApp.accel_y = parseFloat(e.data[10]).toPrecision(4);
        vueApp.accel_z = parseFloat(e.data[11]).toPrecision(4);
    }
    else {
        vueApp.displayGains = e.data[1];
        vueApp.saving = e.data[0];

        vueApp.pressure = e.data[10];
        vueApp.temp = e.data[11];

        vueApp.gps_fix = e.data[2];
        vueApp.gps_latitude = e.data[3];
        vueApp.gps_longitude = e.data[4];

        var imode = parseInt(e.data[12]);
        if (imode == 0) {
            vueApp.mode = "Rates mode";
        }
        else if (imode == 1) {
            vueApp.mode = "Angle mode";
        }
        else if (imode == 2) {
            vueApp.mode = "Essai indiciel";
        }
        else if (imode == 3) {
            vueApp.mode = "Essai rampe";
        }
        else if (imode == 4) {
            vueApp.mode = "Essai sinusoidal";
        }
        else if (imode == 5) {
            vueApp.mode = "Direct control";
        }
        else if (imode == 6) {
            vueApp.mode = "Unknown";
        }
    }

    var now = new Date();
    var dt = now - time_start;
    if (iter == 50) {
        vueApp.frequency = (iter/dt*1000).toPrecision(4);
        console.log(dt + " : " + iter + " : " + iter / dt * 1000);
        time_start = new Date();
        iter = 0;
    }
    iter++;
}, false);


var parallel = async function (Data) {
    await Promise.all([
        (async () => addPointAccel(Data))(),
        (async () => addPointGyr(Data))()
    ]);
}

window.chartColors = {
    red: 'rgb(255, 99, 132)',
    orange: 'rgb(255, 159, 64)',
    yellow: 'rgb(255, 205, 86)',
    green: 'rgb(75, 192, 192)',
    blue: 'rgb(54, 162, 235)',
    purple: 'rgb(153, 102, 255)',
    grey: 'rgb(201, 203, 207)'
};



var AccelerationPlot = new Chart(E_Acceleration_plot, {
    type: 'line',
    data: {
        datasets: [{
            label: 'Acceleration_x',
            fill: false,
            backgroundColor: window.chartColors.blue,
            borderColor: window.chartColors.blue,
        },
        {
            label: 'Acceleration_y',
            fill: false,

            backgroundColor: window.chartColors.red,
            borderColor: window.chartColors.red,
        },
        {
            label: 'Acceleration_z',
            fill: false,

            backgroundColor: window.chartColors.green,
            borderColor: window.chartColors.green,
        }]
    },
    options: {
        responsive: true,
        title: {
            display: true,
            text: 'Chart.js Line Chart'
        },
        elements: {
            line: {
                tension: 0 // disables bezier curves
            }
        },
        showLines: true,
        scales: {
            xAxes: [{
                ticks: {
                    suggestedMin: 50,
                    suggestedMax: 100
                }
            }]
        },
        animation: {
            duration: 0 // general animation time
        },
        hover: {
            animationDuration: 0 // duration of animations when hovering an item
        },
        responsiveAnimationDuration: 0 // animation duration after a resize
    }
});



var GyrationPlot = new Chart(E_Gyration_plot, {
    type: 'line',
    data: {
        datasets: [{
            label: 'Gyration_x',
            fill: false,
            backgroundColor: window.chartColors.blue,
            borderColor: window.chartColors.blue,
        },
        {
            label: 'Gyration_y',
            fill: false,
            backgroundColor: window.chartColors.red,
            borderColor: window.chartColors.red,
        },
        {
            label: 'Gyration_z',
            fill: false,
            backgroundColor: window.chartColors.green,
            borderColor: window.chartColors.green,
        }]
    },
    options: {
        responsive: true,
        title: {
            display: true,
            text: 'Chart.js Line Chart'
        },
        elements: {
            line: {
                tension: 0 // disables bezier curves
            }
        },
        showLines: true,
        scales: {
            xAxes: [{
                ticks: {
                    suggestedMin: 50,
                    suggestedMax: 100
                }
            }]
        },
        animation: {
            duration: 0 // general animation time
        },
        hover: {
            animationDuration: 0 // duration of animations when hovering an item
        },
        responsiveAnimationDuration: 0 // animation duration after a resize
    }
});
async function addPointAccel(Data) {
    var time = parseFloat(Data[1]);
    AccelerationPlot.data.labels.push(time);

    for (let index = 0; index < 3; index++) {
        AccelerationPlot.data.datasets[index].data.push(parseFloat(Data[index + 9]));
    }

    AccelerationPlot.update();

    if (iter > 90) {
        removeData(AccelerationPlot);
    }
}
async function addPointGyr(Data) {
    var time = parseFloat(Data[1]);
    GyrationPlot.data.labels.push(time);

    for (let index = 0; index < 3; index++) {
        GyrationPlot.data.datasets[index].data.push(parseFloat(Data[index + 6]));
    }

    GyrationPlot.update();

    if (iter > 90) {
        removeData(GyrationPlot);
    }
}
var time_start_ = new Date();
var iteration = 1;


function addData(chart, label, data) {
    chart.data.labels.push(label);
    chart.data.datasets.forEach((dataset) => {
        dataset.data.push(data);
    });
    chart.update();
}

function removeData(chart) {
    chart.data.labels.shift();
    chart.data.datasets.forEach((dataset) => {
        dataset.data.shift();
    });
    chart.update();
}
