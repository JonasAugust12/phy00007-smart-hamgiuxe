const emailAlert = 'On';
const mobileReport = 'Off';
const emailReport = 'Off';

const settingController = (req, res) => {
    res.render('setting', {
        title: 'Setting',
        layout: 'layouts/main',
        emailAlert,
        mobileReport,
        emailReport,
    });
}

module.exports = settingController;