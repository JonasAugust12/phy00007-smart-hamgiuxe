from flask import Flask, request, jsonify
from flask_cors import CORS
from model.chatbot import predict_intent
import logging
import sys

# Set up logging
logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.StreamHandler(sys.stdout)
    ]
)


app = Flask(__name__)
CORS(app)  # Enable CORS for all routes

@app.route('/api/chat', methods=['POST', 'GET'])
def chat():
    logging.info("Received request")
    data = request.json
    logging.info(f"Request data: {data}")

    user_message = data.get('message', '')
    logging.info(f"User message: {user_message}")
    
    try:
        # Get response from chatbot
        bot_response = predict_intent(user_message)
        return jsonify({'response': bot_response})
    except Exception as e:
        logging.error(f"Error occurred: {str(e)}", exc_info=True)
        return jsonify({'error': str(e)}), 500

if __name__ == '__main__':
    app.run(debug=True, port=5000)