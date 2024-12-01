const { db, firestoreDB } = require('../config/firebase');
let monthlyTask = {};

monthlyTask.monthlyVehicleNoti = async () => {
     try {
        const carDataSnapshot = await db.ref('/CAR').get();
        const carData = carDataSnapshot.val();
        const monthlyVehicleCount = carData?.carInMonth || 0;

        const notificationRef = firestoreDB.collection('notification');
        const newNotification = {
            type: 'monthlyVehicle',
            value: monthlyVehicleCount,
            createdAt: new Date(),
        };
        await notificationRef.add(newNotification);

        console.log("Monthly vehicle count notification added to Firestore!");
    } catch (error) {
        console.error("Error logging monthly vehicle count to Firestore:", error);
    }
};

module.exports = monthlyTask;