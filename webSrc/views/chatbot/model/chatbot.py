import torch
from torch.utils.data import Dataset, DataLoader
from transformers import AutoTokenizer, AutoModel, AdamW
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import LabelEncoder
import pandas as pd
import os
import logging
import random
from .parkingdata import parking_data, fetching


logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)

# Cấu hình đường dẫn dữ liệu
current_dir = os.path.dirname(os.path.abspath(__file__))
data_path = os.path.join(current_dir, "data", "data.xlsx")

# Tải dữ liệu
data = pd.read_excel(data_path)

# Tiền xử lý dữ liệu
def preprocess(text):
    return text.replace('1', 'một').replace('2', 'hai').replace('3', 'ba')

# Chuẩn bị tokenizer PhoBERT
tokenizer = AutoTokenizer.from_pretrained("vinai/phobert-base")

# Lớp Dataset tùy chỉnh
class IoTLabelDataset(Dataset):
    def __init__(self, texts, labels, tokenizer, max_len=128):
        self.texts = texts
        self.labels = labels
        self.tokenizer = tokenizer
        self.max_len = max_len
    
    def __len__(self):
        return len(self.texts)
    
    def __getitem__(self, idx):
        encoding = self.tokenizer.encode_plus(
            self.texts[idx],
            add_special_tokens=True,
            max_length=self.max_len,
            padding='max_length',
            truncation=True,
            return_tensors='pt'
        )
        
        return {
            'input_ids': encoding['input_ids'].flatten(),
            'attention_mask': encoding['attention_mask'].flatten(),
            'label': torch.tensor(self.labels[idx], dtype=torch.long)
        }

# Mô hình phân loại
class PhoBERTClassifier(torch.nn.Module):
    def __init__(self, num_labels):
        super().__init__()
        self.phobert = AutoModel.from_pretrained("vinai/phobert-base")
        self.dropout = torch.nn.Dropout(0.3)
        self.classifier = torch.nn.Linear(
            self.phobert.config.hidden_size, 
            num_labels
        )
    
    def forward(self, input_ids, attention_mask):
        outputs = self.phobert(
            input_ids=input_ids, 
            attention_mask=attention_mask
        )
        pooled_output = outputs.last_hidden_state[:, 0, :]
        pooled_output = self.dropout(pooled_output)
        logits = self.classifier(pooled_output)
        return logits

# Quá trình huấn luyện
if not os.path.exists(os.path.join(current_dir, 'phobert_model.pth')):
    print("Training model...")
    def train_model(model, train_loader, val_loader, device, num_epochs=10):
        optimizer = AdamW(model.parameters(), lr=2e-5)
        criterion = torch.nn.CrossEntropyLoss()
        
        model.to(device)
        
        for epoch in range(num_epochs):
            model.train()
            total_loss = 0
            
            for batch in train_loader:
                input_ids = batch['input_ids'].to(device)
                attention_mask = batch['attention_mask'].to(device)
                labels = batch['label'].to(device)
                
                optimizer.zero_grad()
                outputs = model(input_ids, attention_mask)
                loss = criterion(outputs, labels)
                
                loss.backward()
                optimizer.step()
                
                total_loss += loss.item()
            
            model.eval()
            val_loss = 0
            correct_preds = 0
            total_preds = 0
            
            with torch.no_grad():
                for batch in val_loader:
                    input_ids = batch['input_ids'].to(device)
                    attention_mask = batch['attention_mask'].to(device)
                    labels = batch['label'].to(device)
                    
                    outputs = model(input_ids, attention_mask)
                    loss = criterion(outputs, labels)
                    val_loss += loss.item()
                    
                    _, predicted = torch.max(outputs, 1)
                    total_preds += labels.size(0)
                    correct_preds += (predicted == labels).sum().item()
            
            print(f"Epoch {epoch+1}/{num_epochs}")
            print(f"Train Loss: {total_loss/len(train_loader):.4f}")
            print(f"Validation Loss: {val_loss/len(val_loader):.4f}")
            print(f"Validation Accuracy: {100 * correct_preds/total_preds:.2f}%")
        
        return model

    # Chuẩn bị dữ liệu
    texts = [preprocess(item) for item in data["text"]]
    intents = [item for item in data["label"]]

    # Mã hóa nhãn
    label_encoder = LabelEncoder()
    encoded_labels = label_encoder.fit_transform(intents)

    # Chia dữ liệu
    X_train, X_val, y_train, y_val = train_test_split(
        texts, encoded_labels, test_size=0.2, random_state=42
    )

    # Tạo dataset và dataloader
    train_dataset = IoTLabelDataset(X_train, y_train, tokenizer)
    val_dataset = IoTLabelDataset(X_val, y_val, tokenizer)

    train_loader = DataLoader(train_dataset, batch_size=8, shuffle=True)
    val_loader = DataLoader(val_dataset, batch_size=8)

    # Khởi tạo và huấn luyện mô hình
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    model = PhoBERTClassifier(num_labels=len(label_encoder.classes_))
    trained_model = train_model(model, train_loader, val_loader, device)

    # Lưu mô hình
    torch.save({
        'model_state_dict': trained_model.state_dict(),
        'label_encoder': label_encoder
    }, os.path.join(current_dir, 'phobert_model.pth'))

# Hàm dự đoán intent
def predict_intent(user_input):
    # Nạp mô hình đã lưu
    checkpoint = torch.load(os.path.join(current_dir, 'phobert_model.pth'))
    
    # Khôi phục mô hình
    model = PhoBERTClassifier(num_labels=len(checkpoint['label_encoder'].classes_))
    model.load_state_dict(checkpoint['model_state_dict'])
    model.eval()
    
    # Tiền xử lý đầu vào
    preprocessed_input = preprocess(user_input)
    
    # Mã hóa đầu vào
    encoding = tokenizer.encode_plus(
        preprocessed_input,
        add_special_tokens=True,
        max_length=128,
        padding='max_length',
        truncation=True,
        return_tensors='pt'
    )
    
    # Dự đoán
    with torch.no_grad():
        outputs = model(encoding['input_ids'], encoding['attention_mask'])
        _, predicted = torch.max(outputs, 1)
        
        # Ánh xạ nhãn dự đoán
        predicted_label = checkpoint['label_encoder'].inverse_transform(predicted)[0]
    
    # Trả về câu trả lời ngẫu nhiên
    parking_data = fetching()
    response_dict = {
    "Nhiệt độ": [
        f"Nhiệt độ hiện tại của hầm xe là {parking_data["temperature"]} độ C",
        f"Hiện tại, nhiệt độ hầm xe đang ở mức {parking_data["temperature"]} độ C",
    ],

    "Số xe hiện tại": [
        f"Hiện tại đang có {parking_data["current_car"]} xe trong bãi, còn {3 - parking_data["current_car"]} chỗ trống",
        f"Trong bãi đang có {parking_data["current_car"]} xe, còn {3 - parking_data["current_car"]} chỗ trống",
    ],

    "Bãi 1": [
        f"Bãi 1 đang {'có xe đậu' if (parking_data["lot1"] == True) else 'trống'}",
    ],

    "Bãi 2": [
        f"Bãi 2 đang {'có xe đậu' if (parking_data["lot2"] == True) else 'trống'}",
    ],

    "Bãi 3": [
        f"Bãi 3 đang {'có xe đậu' if (parking_data["lot3"] == True) else 'trống'}",
    ],

    "Tổng xe ngày hiện tại" : [
        f"Ngày hôm nay đã có {parking_data["today_car"]} xe vào bãi",
    ],

    "Tổng xe ngày hôm qua" : [
        f"Ngày hôm qua đã có {parking_data['yesterday_car']} xe vào bãi",
    ]
}
    return random.choice(response_dict[predicted_label])

# Từ điển phản hồi (giữ nguyên như ban đầu)


