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
                if (data.lot !== undefined) {
                    parkingArray.push({
                        parkingLocation: `Section ${data.lot}`,
                        checkInTime: data.checkin,
                        checkOutTime: data.checkout
                    });
                }
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