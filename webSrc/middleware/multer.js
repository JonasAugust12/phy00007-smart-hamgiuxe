const multer = require('multer');

const storage = multer.diskStorage({
    filename: (req, file, cb) => {
        let fileExt = file.originalname.split('.').pop();
        const fileName = `${new Date().getTime()}.${fileExt}`;

        cb(null, fileName);
    },
});

const imageFilter = (req, file, cb) => {
    if (file.mimetype !== 'image/jpeg' && file.mimetype !== 'image/png') {
        req.fileValidationError = 'File type must be image/jpeg or image/png';

        return cb(null, false, req.fileValidationError);
    } else {
        cb(null, true);
    }
};

const upload = multer({ storage: storage, fileFilter: imageFilter });

module.exports = upload;