let profileController = {};

profileController.renderProfilePage = (req, res) => {
    res.render('edit-profile', {
        title: 'Profile',
        layout: 'layouts/main',
        user: req.session.user,
    });
};

profileController.updateProfile = (req, res) => {
    const { username, phone } = req.body;
    
    req.session.user.username = username;
    req.session.user.phone = phone;
    res.status(200).send({ message: 'Profile updated successfully' });
};

module.exports = profileController;