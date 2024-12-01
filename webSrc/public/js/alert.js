import { database, ref, onValue } from "./firebase-config.js";

const fireRef = ref(database, 'FIRE');

onValue(fireRef, (snapshot) => {
    const data = snapshot.val();
    const isFire = data.state;

    const nav = document.querySelector('nav');
    const sidebar = document.querySelector('.side-bar');
    const content = document.querySelector('main');
    if (isFire) {
        if (nav) {
            nav.classList.add('alert-background');
        }
        if (sidebar) {
            sidebar.classList.add('alert-background');
        }
        if (content) {
            content.classList.add('alert-background-content');
        }
    } else {
        if (nav) {
            nav.classList.remove('alert-background');
        }
        if (sidebar) {
            sidebar.classList.remove('alert-background');
        }
        if (content) {
            content.classList.remove('alert-background-content');
        }
    }
});    