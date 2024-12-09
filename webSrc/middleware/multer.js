const multer = require('multer');

const storage = multer.memoryStorage();

const limits = {
    fileSize: 1024 * 1024 * 10, // 10 MB
};
const imageFilter = (req, file, cb) => {
    if (file.mimetype !== 'image/jpeg' && file.mimetype !== 'image/png' && file.mimetype !== 'image/jpg') {
        req.fileValidationError = 'File type must be image/jpeg or image/png';
        return cb(null, false, req.fileValidationError);
    } else {
        cb(null, true);
    }
};

const upload = multer({ storage: storage, limits: limits, fileFilter: imageFilter });

module.exports = upload;