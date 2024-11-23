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
const barrier = 'On';
const siren = 'Off';
const temperature = 25;
const dailyVehicleCount = 500;
const weeklyVehicleCount = 3500;

const dashboardController = (req, res) => {
    res.render('dashboard', {
        title: 'Dashboard',
        layout: 'layouts/main',
        parkinglogData,
        barrier,
        siren,
        temperature,
        dailyVehicleCount,
        weeklyVehicleCount
    });
}

module.exports = dashboardController;