const fs = require('fs');
const path = require('path');

const transformBinaryFile = (inName, outName) => {
    // read file "FALL.TXT", into a buffer
    const buffer = fs.readFileSync(path.join(__dirname, inName));
    // the file is a binary file, its format is long, float, float, float, repeatedly
    // so we need to read the buffer 4 bytes at a time, and convert it to a long
    const array = [];

    array.push(Array(96 / 8).fill(0).map((_, i) => buffer.readDoubleLE(i * 8)));

    for (let i = 96; i < buffer.length; i += 20) {
        const time = buffer.readUInt32LE(i);
        const x = buffer.readFloatLE(i + 4);
        const y = buffer.readFloatLE(i + 8);
        const z = buffer.readFloatLE(i + 12);
        const is_lsm = buffer.readInt32LE(i + 16);
    
        array.push([time, x, y, z, is_lsm]);
    }
    
    // write the array to a file in the form of "time x y z" multiple lines
    fs.writeFileSync(path.join(__dirname, outName), array.map(line => line.join(' ')).concat().join('\n'));
}

transformBinaryFile('FALL.TXT', 'FALLParsed.txt');

