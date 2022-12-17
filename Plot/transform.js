const fs = require('fs');
const path = require('path');

const transformBinaryFile = (inName, outName) => {
    // read file "FALL.TXT", into a buffer
    const buffer = fs.readFileSync(path.join(__dirname, inName));
    const array = [];

    array.push(Array(96 / 8).fill(0).map((_, i) => buffer.readDoubleLE(i * 8)));

    // one block is unsigned long + float * 6 = 28 bytes
    for (let i = 96; i < buffer.length; i += 28) {
        const time = buffer.readUInt32LE(i);
        const data = Array(6).fill(0).map((_, j) => buffer.readFloatLE(i + 4 + j * 4));
        array.push([time, ...data]);
    }
    
    // write the array to a file in the form of "time x y z" multiple lines
    fs.writeFileSync(path.join(__dirname, outName), array.map(line => line.join(' ')).concat().join('\n'));
}

transformBinaryFile('FILE3LOW.TXT', 'FALLParsed.txt');

