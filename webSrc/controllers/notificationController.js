const notificationController = (req, res) => {
    res.render('notification', {
        title: 'Notifications',
        layout: 'layouts/main',
        user: req.session.user,
    });
}

module.exports = notificationController;