const express = require('express');
const router = express.Router();

const dashboardController = require('../controllers/dashboardController');
const parkinglogController = require('../controllers/parkinglogController');
const settingsController = require('../controllers/settingController');
const {login, register, forgotPass, resetPass} = require('../controllers/authenticationController');
const profileController = require('../controllers/profileController');
const notificationController = require('../controllers/notificationController');
const chatbotController = require('../controllers/chatbotController');

router.get('/', dashboardController);
router.get('/parkinglog', parkinglogController);
router.get('/setting', settingsController);
router.get('/login', login);
router.get('/register', register);
router.get('/forgot-password', forgotPass);
router.get('/reset-password', resetPass);
router.get('/profile', profileController);
router.get('/notification', notificationController);
router.get('/chatbot', chatbotController);

module.exports = router;