const fs = require('fs');
const path = require('path');
const express = require('express');

const app = express();

// serve index.html page in dist folder
app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'dist', 'index.html'));
});

// serve assets in dist folder
app.use(express.static(path.join(__dirname, 'dist')));

// listen on port 3000
app.listen(3000, () => {
    console.log('Server started on port 3000');
});

