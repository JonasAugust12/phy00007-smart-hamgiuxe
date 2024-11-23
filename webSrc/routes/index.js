const express = require('express');
const router = express.Router();

const dashboardController = require('../controllers/dashboardController');
const parkinglogController = require('../controllers/parkinglogController');
const settingsController = require('../controllers/settingController');
const {login, register, forgotPass, resetPass} = require('../controllers/authenticationController');

router.get('/', dashboardController);
router.get('/parkinglog', parkinglogController);
router.get('/setting', settingsController);
router.get('/login', login);
router.get('/register', register);
router.get('/forgot-password', forgotPass);
router.get('/reset-password', resetPass);

module.exports = router;