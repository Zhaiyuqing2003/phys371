import { text } from "./data"
import Plotly from 'plotly.js-dist'

// the data is in the form of "time x y z" multiple lines
const parse = data => data.split('\n').map(line => line.split(' ').map(value => parseFloat(value)));
const getFrequency = (obj, start, end) => {
    const time = obj[end][0] - obj[start][0];
    return (end - start) / time * 1_000_000;
}

const object = parse(text);
const offset = object[0];

const lsm = {
    ax : offset[0],
    ay : offset[1],
    az : offset[2],
    axRms : offset[3],
    ayRms : offset[4],
    azRms : offset[5],
}

const lis = {
    ax : offset[6],
    ay : offset[7],
    az : offset[8],
    axRms : offset[9],
    ayRms : offset[10],
    azRms : offset[11],
}

const obj = object.slice(1);

// console.log(detectImpact(obj, 10, isImpact).map(([start, end]) => [obj[start][0], obj[end][0]]));


Plotly.newPlot('Accel X', [{
    x: obj.map(line => line[0] / 1_000_000),
    y: obj.map(line => line[1] - lsm.ax),
    type: 'scatter',
    mode: 'lines',
    // mode: 'markers',
    name: 'Accel X (LSM)'
}, {
    x: obj.map(line => line[0] / 1_000_000),
    y: obj.map(line => line[2] - lsm.ay),
    type: 'scatter',
    name: 'Accel Y (LSM)'
}, {
    x: obj.map(line => line[0] / 1_000_000),
    y: obj.map(line => line[3] - lsm.az),
    type: 'scatter',
    name : 'Accel Z (LSM)'
}, {
    x: obj.map(line => line[0] / 1_000_000),
    y: obj.map(line => line[4] - lis.ax),
    type: 'scatter',
    name : 'Accel X (LIS)'
}, {
    x: obj.map(line => line[0] / 1_000_000),
    y: obj.map(line => line[5] - lis.ay),
    type: 'scatter',
    name : 'Accel Y (LIS)'
}, {
    x: obj.map(line => line[0] / 1_000_000),
    y: obj.map(line => line[6] - lis.az),
    type: 'scatter',
    name : 'Accel Z (LIS)'
}], {
    xaxis : {
        title: 'Time (s)',
    },
    yaxis : {
        title: 'Acceleration (m/s^2)',
    },
    title: `Acceleration over time (average frequency: ${getFrequency(obj, 0, obj.length - 1).toFixed(2)}Hz)`
})




// // get the time difference
// const timeDiff = obj[obj.length - 1][0] - obj[0][0] 
// // get the average time between each point
// const averageTime = timeDiff / obj.length / 1_000_000;
// const averageFrequency = 1 / averageTime;

// const detectImpact = (obj, inclusion, isImpactFn) => {
//     // impactRange is an [number, number][] array
//     // each [number, number] indicates the start and end index of an impact
//     const impactRange = [];

//     // inclusion is the number of points to include before and after the impact

//     let isInImpact = 0;

//     for (let i = 0; i < obj.length - 1; i++) {
//         if (isImpactFn(obj[i], obj[i + 1])) {
//             if (isInImpact === 0) {
//                 impactRange.push([i - inclusion, i + inclusion]);
//             }
//             isInImpact = inclusion;
//         } else {
//             if (isInImpact > 0) {
//                 isInImpact--;
//                 impactRange[impactRange.length - 1][1] = i + inclusion;
//             }
//         }
//     }

//     return impactRange;
// }

// const isImpact = (prev, curr) => {
//     // check the difference
//     const prevMag = Math.sqrt(prev[1] ** 2 + prev[2] ** 2 + prev[3] ** 2);
//     const currMag = Math.sqrt(curr[1] ** 2 + curr[2] ** 2 + curr[3] ** 2);

//     const diff = Math.abs(prevMag - currMag);

//     if (diff > 10 * 9.8) {
//         return true;
//     }
// }