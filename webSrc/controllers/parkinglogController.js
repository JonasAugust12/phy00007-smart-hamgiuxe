const parkinglogData = [

];

const { firestoreDB } = require('../config/firebase');

const parkinglogController = (req, res) => {
    const currentPage = parseInt(req.query.page) || 1;

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