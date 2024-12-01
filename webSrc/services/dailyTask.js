const { db, firestoreDB } = require('../config/firebase');
let dailyTask = {};

dailyTask.dailyVehicleNoti = async () => {
     try {
        const carDataSnapshot = await db.ref('/CAR').get();
        const carData = carDataSnapshot.val();
        const dailyVehicleCount = carData?.carInDay || 0;

        const notificationRef = firestoreDB.collection('notification');
        const newNotification = {
            type: 'dailyVehicle',
            value: dailyVehicleCount,
            createdAt: new Date(),
        };
        await notificationRef.add(newNotification);

        console.log("Daily vehicle count notification added to Firestore!");
    } catch (error) {
        console.error("Error logging daily vehicle count to Firestore:", error);
    }
};

module.exports = dailyTask;