let settingController = {};

settingController.renderSettingPage = (req, res) => {
    res.render('setting', {
        title: 'Setting',
        layout: 'layouts/main',
        user: req.session.user,
        notification: req.session.user.notifications,
    });
}

settingController.updateSetting = (req, res) => {
    const { settingId, status } = req.body;
    
    req.session.user[settingId] = status;
    res.status(200).send({ message: 'Setting updated successfully' });
};

module.exports = settingController;