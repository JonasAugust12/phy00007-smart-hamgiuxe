const admin = require("firebase-admin");
const firestoreDB = admin.firestore();

let authenticationController = {};

authenticationController.login =  (req, res) => {
    res.render('authentication/login', {
        layout: 'layouts/auth',
        title: 'Login',
    });
}

authenticationController.verifyToken = async (req, res) => {
    const { idToken } = req.body;
    try {
        const decodedToken = await admin.auth().verifyIdToken(idToken);
        const userDoc = await firestoreDB.collection('users').doc(decodedToken.uid).get();
        if (!userDoc.exists) {
            throw new Error("User not found");
        }
        req.session.user = {
            uid: decodedToken.uid,
            ...userDoc.data()
        };
        res.status(200).send({ message: 'User authenticated and session set' });
    } catch (error) {
        console.error("Token verification failed:", error);
        res.status(401).send({ error: "Unauthorized" });
    }
};

authenticationController.register = (req, res) => {
    res.render('authentication/register', {
        layout: 'layouts/auth',
        title: 'Register',
    });
}

authenticationController.forgotPass = (req, res) => {
    res.render('authentication/forgot-password', {
        layout: 'layouts/auth',
        title: 'Forgot Password',
    });
}

authenticationController.resetPass = (req, res) => {
    res.render('authentication/reset-password', {
        layout: 'layouts/auth',
        title: 'Reset Password',
    });
}

module.exports = authenticationController;