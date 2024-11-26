var admin = require("firebase-admin");

var serviceAccount = require("../phy00007-smart-hamgiuxe-22326-firebase-adminsdk-7vqqz-4a0a6b40fc.json");
const databaseURL = process.env.FIREBASE_DATABASE_URL;

admin.initializeApp({
  credential: admin.credential.cert(serviceAccount),
  databaseURL,
});

const db = admin.database();
module.exports = db;
