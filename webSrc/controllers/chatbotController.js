const chatbotController = (req, res) => {
    res.render('chatbot', {
        layout: 'layouts/main', 
        title: 'Chatbot',
    });
};

module.exports = chatbotController;