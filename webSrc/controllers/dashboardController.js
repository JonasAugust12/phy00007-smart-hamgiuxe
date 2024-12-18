const { db, firestoreDB } = require('../config/firebase');

const dashboardController = async (req, res) => {
    const snapshot = await db.ref('/').get();
    data = snapshot.val();

    const {BARRIER, DAILY, MONTHLY, TEMPERATURE, SIREN, LOT, PARKING} = data;
    const {state: barrier} = BARRIER;
    const {state: siren} = SIREN;
    const {value: temperature} = TEMPERATURE;
    const {slotleft: slotleft} = LOT;
    const {docId: parkingDocId, lot: parkingLot, checkin: parkingCheckin, checkout: parkingCheckout} = PARKING;

    if (parkingDocId !== undefined) {
        if (parkingCheckout === "None") {
            firestoreDB.collection('Parking').doc(String(parkingDocId)).set({
                lot: parkingLot,
                checkin: parkingCheckin,
                checkout: parkingCheckout
            })
        } 

        if (parkingCheckin === "None") {
            firestoreDB.collection('Parking').doc(String(parkingDocId)).update({
                checkout: parkingCheckout
            })
        }
    }

    const formatDateWithoutLeadingZeros = (date) => {
        const day = date.getDate();
        const month = date.getMonth() + 1;
        const year = date.getFullYear();
        return `${day}-${month}-${year}`;
    };

    const formatMonthWithoutLeadingZeros = (date) => {
        const month = date.getMonth() + 1;
        const year = date.getFullYear();
        return `${month}-${year}`;
    }

    const today = new Date();
    const todayKey = formatDateWithoutLeadingZeros(today);
    const monthKey = formatMonthWithoutLeadingZeros(today);

    dailyVehicleCount = DAILY[todayKey] ? DAILY[todayKey] : 0;
    weeklyVehicleCount = MONTHLY[monthKey] ? MONTHLY[monthKey] : 0;

     firestoreDB.collection('Parking').get()
        .then((querySnapshot) => {
            const parkingArray = [];
            querySnapshot.forEach((doc) => {
                const data = doc.data();
                if (data.lot !== undefined) {
                    parkingArray.push({
                        parkingLocation: `Section ${data.lot}`,
                        checkInTime: data.checkin,
                        checkOutTime: data.checkout,
                        id: doc.id,
                    });
                }
            });
            parkingArray.sort((a, b) => Number(a.id) - Number(b.id));
            res.render('dashboard', {
                title: 'Dashboard',
                layout: 'layouts/main',
                user: req.session.user,
                notification: req.session.user.notifications,
                parkinglogData: parkingArray.reverse().slice(0, 5),
                barrier: barrier ? 'On' : 'Off',
                siren: siren ? 'On' : 'Off',
                temperature,
                dailyVehicleCount,
                weeklyVehicleCount,
                slotleft,
            });
        })
        .catch((error) => {
            console.error('Error fetching parking data:', error);
            res.status(500).send('Error fetching parking data');
        });
}

module.exports = dashboardController;