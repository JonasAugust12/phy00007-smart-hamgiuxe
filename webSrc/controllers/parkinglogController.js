const { firestoreDB } = require('../config/firebase');

const parkinglogController = (req, res) => {
    const currentPage = parseInt(req.query.page) || 1;

    firestoreDB.collection('Parking').get()
        .then((querySnapshot) => {
            let parkingArray = [];
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
            parkingArray.sort((a, b) => Number(a.id) - Number(b.id)).reverse();
            const totalPages = Math.ceil(parkingArray.length / 10);
            const start = (currentPage - 1) * 10;
            const end = start + 10;
            const pageLogs = parkingArray.slice(start, end);

            res.render('parkinglog', {
                title: 'Parking log',
                layout: 'layouts/main',
                parkinglogData: pageLogs,
                user: req.session.user,
                notification: req.session.user.notifications,
                currentPage,
                totalPages,
            });
        })
        .catch((error) => {
            console.error('Error fetching parking data:', error);
            res.status(500).send('Error fetching parking data');
        });
};

module.exports = parkinglogController;