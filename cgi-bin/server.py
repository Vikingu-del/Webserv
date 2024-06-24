const express = require('express');
const multer = require('multer');
const path = require('path');
const app = express();
const port = 8002;

// Serve static files from gameHub directory
app.use(express.static(path.join(__dirname, 'gameHub')));

// Set up multer for file uploads
const storage = multer.diskStorage({
    destination: (req, file, cb) => {
        cb(null, 'uploads/');
    },
    filename: (req, file, cb) => {
        cb(null, file.originalname);
    }
});
const upload = multer({ storage: storage });

app.post('/upload', upload.single('file'), (req, res) => {
    if (!req.file) {
        return res.status(400).send('No file uploaded.');
    }
    res.send('File uploaded successfully!');
});

app.listen(port, () => {
    console.log(`Server running on http://localhost:${port}`);
});
