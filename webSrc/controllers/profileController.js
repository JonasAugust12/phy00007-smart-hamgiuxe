const profileController = (req, res) => {
    res.render('edit-profile', {
        title: 'Profile',
        layout: 'layouts/main',
    });
}

module.exports = profileController;