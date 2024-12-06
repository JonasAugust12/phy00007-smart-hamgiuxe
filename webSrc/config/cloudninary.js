const cloudinary = require('cloudinary').v2;

cloudinary.config({
    cloud_name: process.env.CLOUDINARY_CLOUD_NAME,
    api_key: process.env.CLOUDINARY_API_KEY,
    api_secret: process.env.CLOUDINARY_API_SECRET
});

const uploadImage = async (req, res) => {
    const file = req.file;
    const uid = req.session.user.uid;

    if (!file) {
        return res.status(400).send({ message: 'File not found' });
    }

    try {
        const uploadedImage = await cloudinary.uploader.upload(req.file.path, {
            resource_type: 'image',
            public_id: uid,
            folder: 'phy000007',
            overwrite: true,
        });

        return uploadedImage;
    } catch (error) {
        return res.status(400).json({ message: error.message });
    }
}

module.exports = { uploadImage };