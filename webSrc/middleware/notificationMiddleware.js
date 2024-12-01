const { firestoreDB } = require('../config/firebase');

const fetchNotifications = async (req, res, next) => {
    try {
        // Fetch the latest 10 notifications
        const notificationsSnapshot = await firestoreDB
            .collection('notification')
            .orderBy('createdAt', 'desc')
            .limit(10)
            .get();
        const notifications = notificationsSnapshot.docs.map(doc => {
            const data = doc.data();
            return {
                type: data.type,
                createdAt: data.createdAt.toDate(),
                value: data.value || null,
            };
        });
        req.session.user.notifications = notifications;
        next();
    } catch (error) {
        console.error("Error fetching notifications in middleware:", error);
        next();
    }
};

module.exports = fetchNotifications;