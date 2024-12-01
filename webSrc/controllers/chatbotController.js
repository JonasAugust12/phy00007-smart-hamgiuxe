const chatbotController = (req, res) => {
    res.render('chatbot', {
        layout: 'layouts/main', 
        title: 'Chatbot',
        user: req.session.user,
        notification: req.session.user.notifications,
    });
};

module.exports = chatbotController;