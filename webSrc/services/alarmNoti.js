const { db, firestoreDB } = require('../config/firebase');

const alarmNoti = () => {
    const fireRef = db.ref('/FIRE');

    fireRef.on('value', async (snapshot) => {
        const fireStatus = snapshot.val();

        if (fireStatus.state === true) {
            try {
                const notificationRef = firestoreDB.collection('notification');
                const newNotification = {
                    type: 'alarm',
                    createdAt: new Date(),
                };

                await notificationRef.add(newNotification);
                console.log("Fire alarm notification added to Firestore!");
            } catch (error) {
                console.error("Error adding notification to Firestore:", error);
            }
        }
    });
};

module.exports = alarmNoti;