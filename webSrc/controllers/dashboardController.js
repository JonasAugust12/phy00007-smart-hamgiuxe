const { db, firestoreDB } = require('../config/firebase');

const dashboardController = async (req, res) => {
    const snapshot = await db.ref('/').get();
    data = snapshot.val();

    const {BARRIER, CAR, TEMPERATURE, SIREN} = data;
    const {state: barrier} = BARRIER;
    const {state: siren} = SIREN;
    const {value: temperature} = TEMPERATURE;
    const {carInDay: dailyVehicleCount, carInMonth: weeklyVehicleCount} = CAR;
     firestoreDB.collection('Parking').get()
        .then((querySnapshot) => {
            const parkingArray = [];
            querySnapshot.forEach((doc) => {
                const data = doc.data();
                parkingArray.push({
                    parkingLocation: `Lot ${data.lot || 'Unknown'}`,
                    entryDate: data.checkin ? data.checkin.split(' ').slice(1, 3).join(' ') : 'Unknown',
                    exitDate: data.checkout ? data.checkout.split(' ').slice(1, 3).join(' ') : 'Unknown',
                    checkInTime: data.checkin ? data.checkin.split(' ')[3] : 'Unknown',
                    checkOutTime: data.checkout ? data.checkout.split(' ')[3] : 'Unknown',
                });
            });

            res.render('dashboard', {
                title: 'Dashboard',
                layout: 'layouts/main',
                user: req.session.user,
                parkinglogData: parkingArray.reverse(),
                barrier: barrier ? 'On' : 'Off',
                siren: siren ? 'On' : 'Off',
                temperature,
                dailyVehicleCount,
                weeklyVehicleCount
            });
        })
        .catch((error) => {
            console.error('Error fetching parking data:', error);
            res.status(500).send('Error fetching parking data');
        });
}

module.exports = dashboardController;
