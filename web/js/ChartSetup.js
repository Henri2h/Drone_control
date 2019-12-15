var E_Gyration_plot = document.getElementById('gyrationPlot'); // chart
var E_Acceleration_plot = document.getElementById('accelerationPlot'); // chart

var worker = new Worker('js/Worker_Client.js');
var iter = 0;

// Setup an event listener that will handle messages received from the worker.
worker.addEventListener('message', function (e) {
    // Log the workers message.
    if (e.data[0] == "FStatus") {
        // parallel(e.data);

        addPointAccel(e.data);
        addPointGyr(e.data);

    }
    else{
        for (let index = 0; index < 18; index++) {
            document.getElementById("result_" + index).innerHTML = e.data[index + 1];
        }

        var imode = parseInt(e.data[12]);
        if(imode == 0){
            document.getElementById("result_mode").innerHTML = "Rates mode";
        }
        else if(imode == 1){
            document.getElementById("result_mode").innerHTML = "Angle mode";
        }
        else if(imode == 2){ 
            document.getElementById("result_mode").innerHTML = "Essai indiciel";
        }
        else if(imode == 3){ 
            document.getElementById("result_mode").innerHTML = "Essai rampe";
        }
        else if(imode == 4){
            document.getElementById("result_mode").innerHTML = "Essai sinusoidal";
        }
        else if(imode == 5){
            document.getElementById("result_mode").innerHTML = "Direct control";
        }
        else if(imode == 6){
            document.getElementById("result_mode").innerHTML = "Unknown";
        }
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
        showLines: false,
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
        showLines: false,
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
