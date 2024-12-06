const { uploadImage } = require('../config/cloudninary');
const { firestoreDB } = require('../config/firebase');
let profileController = {};

profileController.renderProfilePage = (req, res) => {
    res.render('edit-profile', {
        title: 'Profile',
        layout: 'layouts/main',
        user: req.session.user,
        notification: req.session.user.notifications,
    });
};

profileController.updateProfile = (req, res) => {
    const { username, phone } = req.body;
    
    req.session.user.username = username;
    req.session.user.phone = phone;
    res.status(200).send({ message: 'Profile updated successfully' });
};

profileController.uploadAvatar = async (req, res) => {
        try {
        if (!req.file) {
            return res.status(400).json({ message: 'No file uploaded' });
        }

        const uid = req.session.user.uid;
        const uploadedImage = await uploadImage(req, res);
        const userRef = firestoreDB.collection('users').doc(uid);
        await userRef.update({
            avatarUrl: uploadedImage.secure_url,
        });
        req.session.user.avatarUrl = uploadedImage.secure_url;
        return res.status(200).json({
            message: 'Avatar uploaded successfully!',
            avatarUrl: uploadedImage.secure_url,
        });
    } catch (error) {
        console.error('Error uploading avatar:', error);
        return res.status(500).json({ message: 'Error uploading avatar' });
    }
}

module.exports = profileController;