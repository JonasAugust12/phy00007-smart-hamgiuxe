require('dotenv').config();
const express = require('express');
const path = require('path');
const expressLayouts = require('express-ejs-layouts');
const session = require('express-session');
const bodyParser = require('body-parser');
const mainRoutes = require('./routes');
const alarmNoti  = require('./services/alarmNoti');
const cron = require('node-cron');
const dailyTask = require('./services/dailyTask');
const monthlyTask = require('./services/monthlyTask');

const app = express();

app.use(session({
  secret: 'mysecretkey',
  resave: false,
  saveUninitialized: false,
  cookie: { secure: false, maxAge: 7200000 }
}));

app.set('view engine', 'ejs');
app.set('views', path.join(__dirname, 'views'));

// Middleware
app.use(expressLayouts);
app.use(express.json());

app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());

// Routes
app.use('/', mainRoutes);

app.use(express.static(path.join(__dirname, 'public')));

alarmNoti();
// Schedule daily task at 23:59
cron.schedule('59 23 * * *', dailyTask.dailyVehicleNoti);
// Schedule monthly task at 23:02 on the 1st day of every month
cron.schedule('59 23 1 * *', monthlyTask.monthlyVehicleNoti);

const PORT = process.env.PORT || 5000;
app.listen(PORT, () => {
    console.log(`Server is running on http://localhost:${PORT}`);
});
