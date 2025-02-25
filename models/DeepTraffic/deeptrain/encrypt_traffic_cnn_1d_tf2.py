import time
import sys
import os
import numpy as np
import tensorflow as tf
from tensorflow.keras import layers, models
from sklearn.metrics import classification_report

# Configuration
DATA_DIR = sys.argv[1]
CLASS_NUM = int(sys.argv[2])
TRAIN_ROUND = int(sys.argv[3])

# Class dictionaries
class_dicts = {
    2: {0:'Novpn', 1:'Vpn'},
    6: {0:'Chat', 1:'Email', 2:'File', 3:'P2p', 4:'Streaming', 5:'Voip'},
    12: {0:'Chat', 1:'Email', 2:'File', 3:'P2p', 4:'Streaming', 5:'Voip',
         6:'Vpn_Chat', 7:'Vpn_Email', 8:'Vpn_File', 9:'Vpn_P2p', 10:'Vpn_Streaming', 11:'Vpn_Voip'}
}

def build_model(input_shape, num_classes):
    model = models.Sequential([
        layers.Conv1D(32, 25, activation='relu', padding='same', input_shape=input_shape),
        layers.MaxPooling1D(pool_size=3, strides=3, padding='same'),
        layers.Conv1D(64, 25, activation='relu', padding='same'),
        layers.MaxPooling1D(pool_size=3, strides=3, padding='same'),
        layers.Flatten(),
        layers.Dense(1024, activation='relu'),
        layers.Dropout(0.5),
        layers.Dense(num_classes, activation='softmax')
    ])
    return model

def main():
    # Load custom data
    def load_custom_data(data_dir):
        import gzip
        import struct
        
        def read_images(filename):
            with gzip.open(filename, 'rb') as f:
                magic, num, rows, cols = struct.unpack(">IIII", f.read(16))
                images = np.frombuffer(f.read(), dtype=np.uint8)
                images = images.reshape(num, rows * cols)
                return images / 255.0
        
        def read_labels(filename):
            with gzip.open(filename, 'rb') as f:
                magic, num = struct.unpack(">II", f.read(8))
                labels = np.frombuffer(f.read(), dtype=np.uint8)
                return labels
        
        train_images = read_images(os.path.join(data_dir, 'train-images-idx3-ubyte.gz'))
        train_labels = read_labels(os.path.join(data_dir, 'train-labels-idx1-ubyte.gz'))
        test_images = read_images(os.path.join(data_dir, 't10k-images-idx3-ubyte.gz'))
        test_labels = read_labels(os.path.join(data_dir, 't10k-labels-idx1-ubyte.gz'))
        
        return (train_images, train_labels), (test_images, test_labels)

    # Load data
    (train_images, train_labels), (test_images, test_labels) = load_custom_data(DATA_DIR)
    
    # Reshape and normalize data for 1D CNN
    train_images = train_images.reshape(-1, train_images.shape[1], 1).astype('float32') / 255.0
    test_images = test_images.reshape(-1, test_images.shape[1], 1).astype('float32') / 255.0
    
    # Convert labels to one-hot encoding
    train_labels = tf.keras.utils.to_categorical(train_labels, CLASS_NUM)
    test_labels = tf.keras.utils.to_categorical(test_labels, CLASS_NUM)
    
    # Build model
    model = build_model((784, 1), CLASS_NUM)
    
    # Compile model
    model.compile(optimizer=tf.keras.optimizers.Adam(learning_rate=1e-4),
                  loss='categorical_crossentropy',
                  metrics=['accuracy'])
    
    # Model checkpoint callback
    folder = os.path.split(DATA_DIR)[1]
    model_name = f"model_{CLASS_NUM}class_{folder}"
    model_path = os.path.join(model_name, "model.h5")
    
    if not os.path.exists(model_name):
        os.makedirs(model_name)
    
    checkpoint_callback = tf.keras.callbacks.ModelCheckpoint(
        filepath=model_path,
        save_weights_only=False,  # Save full model
        save_best_only=True,
        verbose=1
    )
    
    # Train or load model
    if not os.path.exists(model_path):
        print("Training new model...")
        model.fit(train_images, train_labels,
                  epochs=TRAIN_ROUND // (len(train_images) // 50),
                  batch_size=50,
                  validation_split=0.1,
                  callbacks=[checkpoint_callback])
    else:
        print("Loading existing model...")
        model = tf.keras.models.load_model(model_path)
    
    # Evaluate model
    test_loss, test_acc = model.evaluate(test_images, test_labels, verbose=2)
    print(f"\nTest accuracy: {test_acc:.4f}")
    
    # Generate classification report
    y_pred = model.predict(test_images)
    y_pred_classes = np.argmax(y_pred, axis=1)
    y_true_classes = np.argmax(test_labels, axis=1)
    
    class_dict = class_dicts.get(CLASS_NUM, {})
    target_names = [class_dict.get(i, str(i)) for i in range(CLASS_NUM)]
    
    # Print inference results
    print("\nInference Results:")
    for i in range(len(test_images)):
        true_label = target_names[y_true_classes[i]]
        pred_label = target_names[y_pred_classes[i]]
        print(f"Sample {i+1}: True Label = {true_label}, Predicted Label = {pred_label}")
    
    report = classification_report(y_true_classes, y_pred_classes, target_names=target_names)
    print("\nClassification Report:")
    print(report)
    
    # Save results
    with open('out.txt', 'a') as f:
        f.write("\n")
        t = time.strftime('%Y-%m-%d %X', time.localtime())
        f.write(t + "\n")
        f.write(f'DATA_DIR: {DATA_DIR}\n')
        f.write(report)
        f.write(f'Total accuracy: {test_acc:.4f}\n\n')

if __name__ == "__main__":
    main()

