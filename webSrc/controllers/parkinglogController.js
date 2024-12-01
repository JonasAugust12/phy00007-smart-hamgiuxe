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
                        checkOutTime: data.checkout
                    });
                }
            });
            parkingArray.reverse();
            const totalPages = Math.ceil(parkingArray.length / 12);
            const start = (currentPage - 1) * 12;
            const end = start + 12;
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