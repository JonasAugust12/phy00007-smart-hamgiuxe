const parkinglogData = [
    {
        parkingLocation: 'Section 1',
        entryDate: '15-08-2019',
        checkInTime: '12:51',
        checkOutTime: '1:20'
    },
    {
        parkingLocation: 'Section 1',
        entryDate: '15-08-2019',
        checkInTime: '12:51',
        checkOutTime: '1:20'
    },
    {
        parkingLocation: 'Section 1',
        entryDate: '15-08-2019',
        checkInTime: '12:51',
        checkOutTime: '1:20'
    },
    {
        parkingLocation: 'Section 1',
        entryDate: '15-08-2019',
        checkInTime: '12:51',
        checkOutTime: '1:20'
    },
    {
        parkingLocation: 'Section 1',
        entryDate: '15-08-2019',
        checkInTime: '12:51',
        checkOutTime: '1:20'
    }
]
const siren = 'Off';
const temperature = 25;

const db = require('../config/firebase');
let data = {};

db.ref('/').on('value', (snapshot) => {
    data = snapshot.val();
});

const dashboardController = async (req, res) => {
    const {BARRIER, CAR} = data;
    const {state: barrier} = BARRIER;
    const {carInDay: dailyVehicleCount, carInMonth: weeklyVehicleCount} = CAR;
    res.render('dashboard', {
        title: 'Dashboard',
        layout: 'layouts/main',
        parkinglogData,
        barrier : barrier ? 'On' : 'Off',
        siren,
        temperature,
        dailyVehicleCount,
        weeklyVehicleCount
    });
}

const toggleBarrier = async (req, res) => {
    try {
        const {status} = req.body;
        const ref = db.ref('BARRIER');

        await ref.child('state').set(status);
        res.status(200).json({message: 'Barrier status updated'});
    } catch (error) {
        console.error(error);
        res.status(500).json({message: 'An error occurred'});
    }
}

module.exports = {
    dashboardController,
    toggleBarrier
};