import os
from dotenv import load_dotenv
import firebase_admin
from firebase_admin import credentials, db
import time

# Tải các biến môi trường từ file .env
load_dotenv()

# Lấy thông tin từ file .env
service_account_path = os.getenv("FIREBASE_SERVICE_ACCOUNT_PATH")
database_url = os.getenv("FIREBASE_DATABASE_URL")

# Kiểm tra các biến môi trường đã được tải chưa
if not service_account_path or not database_url:
    raise ValueError("Missing FIREBASE_SERVICE_ACCOUNT_PATH or FIREBASE_DATABASE_URL in .env file")

# Khởi tạo Firebase Admin SDK
cred = credentials.Certificate(service_account_path)
firebase_admin.initialize_app(cred, {
    'databaseURL': database_url
})

# Kết nối tới Realtime Database
realtime_db = db.reference()

# Kiểm tra kết nối bằng cách đọc dữ liệu từ gốc của Database
try:
    data = realtime_db.get()
except Exception as e:
    print("Lỗi khi đọc dữ liệu từ Firebase:", e)

today = time.strftime("%#d-%#m-%Y")
yesterday = time.strftime("%#d-%#m-%Y", time.localtime(time.time() - 86400))


yesterday_car = 0
today_car = 0

try :
    yesterday_car = data["DAILY"][yesterday]
except Exception as e:
    yesterday_car = 0

try:
    today_car = data["DAILY"][today]
except Exception as e:
    today_car = 0

parking_data = {
    "temperature": data["TEMPERATURE"]["value"],
    "current_car": int(bool(data["LOT"]["lot1"]) + bool(data["LOT"]["lot2"]) + bool(data["LOT"]["lot3"])),
    "lot1": bool(data["LOT"]["lot1"] != 0),
    "lot2": bool(data["LOT"]["lot2"] != 0),
    "lot3": bool(data["LOT"]["lot3"] != 0),
    "today_car": today_car,
    "yesterday_car": yesterday_car
}

def fetching():
    global parking_data
    try:
        data = realtime_db.get()
    except Exception as e:
        print("Lỗi khi đọc dữ liệu từ Firebase:", e)

    today = time.strftime("%#d-%#m-%Y")
    yesterday = time.strftime("%#d-%#m-%Y", time.localtime(time.time() - 86400))

    yesterday_car = 0
    today_car = 0

    try :
        yesterday_car = data["DAILY"][yesterday]
    except Exception as e:
        yesterday_car = 0

    try:
        today_car = data["DAILY"][today]
    except Exception as e:
        today_car = 0

    parking_data = {
        "temperature": data["TEMPERATURE"]["value"],
        "current_car": int(bool(data["LOT"]["lot1"]) + bool(data["LOT"]["lot2"]) + bool(data["LOT"]["lot3"])),
        "lot1": bool(data["LOT"]["lot1"]),
        "lot2": bool(data["LOT"]["lot2"]),
        "lot3": bool(data["LOT"]["lot3"]),
        "today_car": today_car,
        "yesterday_car": yesterday_car
    }

    return parking_data