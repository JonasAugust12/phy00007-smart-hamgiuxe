const { firestoreDB } = require('../config/firebase');

const notificationController = async (req, res) => {
    try {
        const notificationsSnapshot = await firestoreDB.collection('notification').orderBy('createdAt', 'desc').get();

        const notifications = notificationsSnapshot.docs.map(doc => {
            const data = doc.data();
            let createdAtFormatted;

            if (data.type === 'monthlyVehicle') {
                createdAtFormatted = new Date(data.createdAt.toDate()).toLocaleString('en-US', {
                    month: 'long',
                    year: 'numeric',
                });
            } else {
                createdAtFormatted = data.createdAt.toDate().toLocaleString();
            }

            return {
                type: data.type,
                createdAt: createdAtFormatted,
                value: data.value || '',
            };
        });

        res.render('notification', {
            title: 'Notifications',
            layout: 'layouts/main',
            user: req.session.user,
            notifications: notifications,
            notification: req.session.user.notifications,
        });
    } catch (error) {
        console.error("Error fetching notifications from Firestore:", error);
        res.status(500).send("Error fetching notifications");
    }
};

module.exports = notificationController;
