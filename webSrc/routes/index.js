const express = require('express');
const router = express.Router();

const dashboardController = require('../controllers/dashboardController');
const parkinglogController = require('../controllers/parkinglogController');
const { renderSettingPage, updateSetting, sendEmail } = require('../controllers/settingController');
const {
    login, 
    register, 
    forgotPass,
    verifyToken,
    logout
} = require('../controllers/authenticationController');
const { renderProfilePage, updateProfile, uploadAvatar} = require('../controllers/profileController');
const notificationController = require('../controllers/notificationController');
const chatbotController = require('../controllers/chatbotController');
const authenticationMiddleware  = require('../middleware/authenticationMiddleware');
const fetchNotifications = require('../middleware/notificationMiddleware');
const upload = require('../middleware/multer');


router.get('/', authenticationMiddleware, fetchNotifications, dashboardController);
router.get('/parkinglog', authenticationMiddleware, fetchNotifications, parkinglogController);
router.get('/setting', authenticationMiddleware, fetchNotifications, renderSettingPage);
router.get('/profile', authenticationMiddleware, fetchNotifications, renderProfilePage);
router.get('/notification', authenticationMiddleware, fetchNotifications, notificationController);
router.get('/chatbot', authenticationMiddleware, fetchNotifications, chatbotController);
router.post('/upload-avatar', authenticationMiddleware, upload.single('image'), uploadAvatar);
router.post('/send-email',authenticationMiddleware, sendEmail);

router.get('/login', login);
router.get('/register', register);
router.get('/forgot-password', forgotPass);
router.post('/logout', logout);

router.post('/verify-token', verifyToken);
router.post('/update-profile', authenticationMiddleware, updateProfile);
router.post('/update-setting', authenticationMiddleware, updateSetting);

module.exports = router;