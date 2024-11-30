const express = require('express');
const router = express.Router();

const dashboardController = require('../controllers/dashboardController');
const parkinglogController = require('../controllers/parkinglogController');
const { renderSettingPage, updateSetting } = require('../controllers/settingController');
const {
    login, 
    register, 
    forgotPass, 
    resetPass,
    verifyToken
} = require('../controllers/authenticationController');
const { renderProfilePage, updateProfile} = require('../controllers/profileController');
const notificationController = require('../controllers/notificationController');
const chatbotController = require('../controllers/chatbotController');
const authenticationMiddleware  = require('../middleware/authenticationMiddleware');


router.get('/', authenticationMiddleware, dashboardController);
router.get('/parkinglog', authenticationMiddleware, parkinglogController);
router.get('/setting', authenticationMiddleware, renderSettingPage);
router.get('/profile', authenticationMiddleware, renderProfilePage);
router.get('/notification', authenticationMiddleware, notificationController);
router.get('/chatbot', authenticationMiddleware, chatbotController);

router.get('/login', login);
router.get('/register', register);
router.get('/forgot-password', forgotPass);
router.get('/reset-password', resetPass);

router.post('/verify-token', verifyToken);
router.post('/update-profile', updateProfile);
router.post('/update-setting', updateSetting);

module.exports = router;