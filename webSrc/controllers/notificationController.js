const notificationController = (req, res) => {
    res.render('notification', {
        title: 'Notifications',
        layout: 'layouts/main',
    });
}

module.exports = notificationController;