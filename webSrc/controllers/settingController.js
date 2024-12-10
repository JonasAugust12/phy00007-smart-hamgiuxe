require('dotenv').config();
const { db, firestoreDB } = require('../config/firebase');
const nodemailer = require('nodemailer');
const { google } = require('googleapis');
const path = require('path');
const fs = require('fs');

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

const CLIENT_ID = process.env.CLIENT_ID;
const CLIENT_SECRET = process.env.CLIENT_SECRET;
const REDIRECT_URI = process.env.REDIRECT_URI;
const REFRESH_TOKEN = process.env.REFRESH_TOKEN;
const templatePath = path.join(__dirname, '../public/template.html');
const template = fs.readFileSync(templatePath, 'utf8');

const formatDate = (date, monthOnly = false) => {
    const [year, month, day] = date.toISOString().split('T')[0].split('-');
    if (monthOnly) {
        return `${month}-${year}`;
    }
    return `${day}-${month}-${year}`;
};

settingController.sendEmail = async (req, res) => {
    try {
        const email = req.session.user.email;
        if (!email) {
            return res.status(400).send({ message: 'Email is required' });
        }

        const snapshot = await db.ref('/').get();
        data = snapshot.val();

        const today = new Date();
        const formattedDate = formatDate(today);
        const formattedMonth = formatDate(today, true);
        
        const yesterday = new Date(today);
        yesterday.setDate(yesterday.getDate() - 1);
        const formattedYesterday = formatDate(yesterday);
        
        const lastMonth = new Date(today);
        lastMonth.setMonth(lastMonth.getMonth() - 1);
        const formattedLastMonth = formatDate(lastMonth, true);
        
        const time = formattedDate + ' ' + today.getHours() + ':' + today.getMinutes() + ':' + today.getSeconds();
        
        const {TEMPERATURE, DAILY, MONTHLY} = data;
        const {value: temperature} = TEMPERATURE;

        const dailyVehicleCount = DAILY[formattedDate] || 0;
        const yesterdayVehicleCount = DAILY[formattedYesterday] || 0;
        const monthlyVehicleCount = MONTHLY[formattedMonth] || 0;
        const lastMonthVehicleCount = MONTHLY[formattedLastMonth] || 0;
        
        const dailyVehicleCountChange = dailyVehicleCount - yesterdayVehicleCount;
        const monthlyVehicleCountChange = monthlyVehicleCount - lastMonthVehicleCount;

        const dayDiffString = dailyVehicleCountChange > 0 ? `↗ ${dailyVehicleCountChange}` : `↘ ${-dailyVehicleCountChange}`;
        const monthDiffString = monthlyVehicleCountChange > 0 ? `↗ ${monthlyVehicleCountChange}` : `↘ ${-monthlyVehicleCountChange}`;

        const oAuth2Client = new google.auth.OAuth2(
            CLIENT_ID,
            CLIENT_SECRET,
            REDIRECT_URI
        );
        oAuth2Client.setCredentials({ refresh_token: REFRESH_TOKEN });
        const accessToken = await oAuth2Client.getAccessToken();
    
        const transporter = nodemailer.createTransport({
            service: 'gmail',
            auth: {
                type: 'OAuth2',
                user: 'ndhoang22@clc.fitus.edu.vn',
                clientId: CLIENT_ID,
                clientSecret: CLIENT_SECRET,
                refreshToken: REFRESH_TOKEN,
                accessToken: accessToken.token,
            },
        });
    
        const editedTemplate = template
            .replace('{{Time}}', time)
            .replace('{{Temp}}', temperature)
            .replace('{{Daily}}', dailyVehicleCount)
            .replace('{{Monthly}}', monthlyVehicleCount)
            .replace('{{dayDiff}}', dayDiffString)
            .replace('{{monthDiff}}', monthDiffString);

        const mailOptions = {
            from: 'Parking Lot <smartparkinglot@spls.email>',
            to: email,
            subject: 'Parking Lot Report',
            html: editedTemplate,
        };
        const result = await transporter.sendMail(mailOptions);
        res.status(200).send({ message: 'Email sent successfully' });
    } catch (error) {
        console.error('Error sending email:', error);
        res.status(500).send({ message: 'Error sending email' });
    }
}

module.exports = settingController;