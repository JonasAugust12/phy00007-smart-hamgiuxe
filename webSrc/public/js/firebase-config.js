import { initializeApp } from 'https://www.gstatic.com/firebasejs/11.0.2/firebase-app.js';
import { 
    getDatabase, 
    ref, 
    onValue, 
    set, 
    update
} from 'https://www.gstatic.com/firebasejs/11.0.2/firebase-database.js';
import {
    getAuth,
    createUserWithEmailAndPassword,
    signInWithEmailAndPassword,
    signOut,
    sendPasswordResetEmail,
    updatePassword,
} from 'https://www.gstatic.com/firebasejs/11.0.2/firebase-auth.js';
import {
    getFirestore,
    doc,
    getDoc,
    setDoc,
    updateDoc
} from 'https://www.gstatic.com/firebasejs/11.0.2/firebase-firestore.js';

const firebaseConfig = {
    apiKey: "AIzaSyDT9N4qIWK179RqKhlGmR5TqUzJfP4hMvY",
    authDomain: "phy00007-smart-hamgiuxe-22326.firebaseapp.com",
    databaseURL: "https://phy00007-smart-hamgiuxe-22326-default-rtdb.firebaseio.com",
    projectId: "phy00007-smart-hamgiuxe-22326",
    storageBucket: "phy00007-smart-hamgiuxe-22326.firebasestorage.app",
    messagingSenderId: "1072684765533",
    appId: "1:1072684765533:web:e04d0489458b4eeb3c58b4"
};

const app = initializeApp(firebaseConfig);
const database = getDatabase(app);
const auth = getAuth(app);
const firestore = getFirestore(app);

export { 
    database,
    auth, 
    firestore,
    ref, 
    onValue, 
    set, 
    update,
    createUserWithEmailAndPassword,
    signInWithEmailAndPassword,
    signOut,
    sendPasswordResetEmail,
    updatePassword,
    getDoc,
    setDoc,
    doc,
    updateDoc
};
