const { db, firestoreDB } = require('../config/firebase');
var push = require('pushsafer-notifications');

const p = new push({
    k: process.env.PUSHSAFER_KEY,
    debug: true
});

const message = {
    m: 'Cháy hầm rồi kìa',
    t: "Smart Hamgiuxe Alert",
    s: '8',
    v: '2',
    i: '5',
    c: '#FF0000',
    u: 'https://www.pushsafer.com',
    ut: 'Open Link',
    d: 'a'
};

var first_message = true;

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
                
                // UNCOMMENT THIS TO SEND PUSH NOTIFICATION
                /* if (first_message) {
                    p.send(message, (err, result) => {
                        if (err) {
                            console.error("Error sending push notification:", err);
                        } else {
                            console.log("Push notification sent successfully!");
                        }
                    });
                    first_message = false;
                } */

            } catch (error) {
                console.error("Error adding notification to Firestore:", error);
            }
        } else {
            first_message = true;
        }
    });
};

module.exports = alarmNoti;