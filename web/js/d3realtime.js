let h = 200;//window.innerHeight;
let w = window.innerWidth;

let time = 0;
let num = 300;

let accel_x = [0];
let accel_y = [0];
let accel_z = [0];

let latestAccel_x = [0];
let latestAccel_y = [0];
let latestAccel_z = [0];

let x = d3.scale.linear().range([0, w - 40]);
let y = d3.scale.linear().range([h - 40, 0]);

let xAxis = d3.svg.axis()
  .scale(x)
  .orient('bottom')
  .innerTickSize(-h + 40)
  .outerTickSize(0)
  .tickPadding(10);

let yAxis = d3.svg.axis()
  .scale(y)
  .orient('left')
  .innerTickSize(-w + 40)
  .outerTickSize(0)
  .tickPadding(10);

let line = d3.svg.line()
  .x((d, i) => x(i + time - num))
  .y(d => y(d));

let svg = d3.select('#charts').append('svg')
  .attr({ width: "100%", height: h })
  .append('g')
  .attr('transform', 'translate(30, 20)');

let $xAxis = svg.append('g')
  .attr('class', 'x axis')
  .attr('transform', `translate(0, ${h - 40})`)
  .call(xAxis);

let $yAxis = svg.append('g')
  .attr('class', 'y axis')
  .call(yAxis);

let $latestAccel_x = svg.append('path')
  .attr('class', 'line accel-x');

let $latestAccel_y = svg.append('path')
  .attr('class', 'line accel-y');

let $latestAccel_z = svg.append('path')
  .attr('class', 'line accel-z');


let legend = svg.append('g')
  .attr('transform', `translate(20, 20)`)
  .selectAll('g')
  .data([['Accel x', '#000'], ['Accel y', '#0ff'], ['Accel z', '#ff0']])
  .enter()
  .append('g');

legend
  .append('circle')
  .attr('fill', d => d[1])
  .attr('r', 5)
  .attr('cx', 0)
  .attr('cy', (d, i) => i * 15);

legend
  .append('text')
  .text(d => d[0])
  .attr('transform', (d, i) => `translate(10, ${i * 15 + 4})`);

function tick() {

  if (Accel.length > 0) {
    time++;
    accel_x[time] = Accel[0][0];
    accel_y[time] = Accel[0][1];
    accel_z[time] = Accel[0][2];
    Accel.shift();

    if (time <= num) {
      latestAccel_x = accel_x.slice(-num);
      latestAccel_y = accel_y.slice(-num);
      latestAccel_z = accel_z.slice(-num);
    }
    else {
      latestAccel_x.shift();
      latestAccel_y.shift();
      latestAccel_z.shift();

      latestAccel_x.push(accel_x[time]);
      latestAccel_y.push(accel_y[time]);
      latestAccel_z.push(accel_z[time]);
    }
    return true;
  }
  return false;


}

function update() {
  x.domain([time - num, time]);
  let yDom = d3.extent(latestAccel_x);
  yDom[0] = -15;//Math.max(yDom[0] - 1, 0);
  yDom[1] = 15;
  y.domain(yDom);

  $xAxis
    .call(xAxis);

  $yAxis
    .call(yAxis);

  $latestAccel_x
    .datum(latestAccel_x)
    .attr('d', line);

  $latestAccel_y
    .datum(latestAccel_y)
    .attr('d', line);

  $latestAccel_z
    .datum(latestAccel_z)
    .attr('d', line);

}

setInterval(() => {
  if(tick()){
    update();
  }
}, 1);