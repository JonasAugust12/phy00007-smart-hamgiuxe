let authenticationController = {};

authenticationController.login = (req, res) => {
    res.render('authentication/login', {
        layout: 'layouts/auth',
    });
}

authenticationController.register = (req, res) => {
    res.render('authentication/register', {
        layout: 'layouts/auth',
    });
}

authenticationController.forgotPass = (req, res) => {
    res.render('authentication/forgot-password', {
        layout: 'layouts/auth',
    });
}

authenticationController.resetPass = (req, res) => {
    res.render('authentication/reset-password', {
        layout: 'layouts/auth',
    });
}

module.exports = authenticationController;