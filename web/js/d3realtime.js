let h = 200;//window.innerHeight;
let w = 500;//window.innerWidth;

let time = 0;
let num = 300;

let noise = new SimplexNoise();
let seed = 50 + 100 * Math.random();
let data = [seed];
let averages_50 = [0];
let averages_25 = [0];
let deltas = [seed];

let latestData = [seed];
let latestAverages_50 = [0];
let latestAverages_25 = [0];
let latestDeltas = [seed];

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
  .attr({width: w, height: h})
.append('g')
  .attr('transform', 'translate(30, 20)');

let $xAxis = svg.append('g')
  .attr('class', 'x axis')
  .attr('transform', `translate(0, ${h - 40})`)
  .call(xAxis);

let $yAxis = svg.append('g')
  .attr('class', 'y axis')
  .call(yAxis);

let $data = svg.append('path')
  .attr('class', 'line data');

let $averages_50 = svg.append('path')
  .attr('class', 'line average-50');

let $averages_25 = svg.append('path')
  .attr('class', 'line average-25');


let legend = svg.append('g')
  .attr('transform', `translate(20, 20)`)
  .selectAll('g')
  .data([['Value', '#000'], ['Trailing Average - 50', '#0ff'], ['Trailing Average - 25', '#ff0']])
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
  time++;
  data[time] = Math.cos(time/10);
  averages_25[time] = Math.sin(time/10);
  averages_50[time] = Math.sin(time)*Math.sin(time/100);
  

  deltas[time] = data[time] - data[time - 1];

  if (time <= num) {
    latestData = data.slice(-num);
    latestAverages_50 = averages_50.slice(-num);
    latestAverages_25 = averages_25.slice(-num);
    latestDeltas = deltas.slice(-num);
  }
  else {
    latestData.shift();
    latestAverages_50.shift();
    latestAverages_25.shift();
    latestDeltas.shift();
    latestData.push(data[time]);
    latestAverages_50.push(averages_50[time]);
    latestAverages_25.push(averages_25[time]);
    latestDeltas.push(deltas[time]);
  }
}

function update() {
  x.domain([time - num, time]);
  let yDom = d3.extent(latestData);
  yDom[0] = -1.5;//Math.max(yDom[0] - 1, 0);
  yDom[1] = 1.5;
  y.domain(yDom);

  $xAxis
    .call(xAxis);

  $yAxis
    .call(yAxis);

  $data
    .datum(latestData)
    .attr('d', line);

  $averages_50
    .datum(latestAverages_50)
    .attr('d', line);

  $averages_25
    .datum(latestAverages_25)
    .attr('d', line);

}

for (let i = 0; i < num + 50; i++) {
  tick();
}

update();

setInterval(() => {
  tick();
  update();
}, 10);