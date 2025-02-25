import tensorflow as tf

# Load your trained model
model_path = "model.h5"  # Update to your actual path
model = tf.keras.models.load_model(model_path)

# Convert to TensorFlow Lite
converter = tf.lite.TFLiteConverter.from_keras_model(model)
tflite_model = converter.convert()

# Save the TFLite model
tflite_model_path = "model.tflite"
with open(tflite_model_path, "wb") as f:
    f.write(tflite_model)

print(f"TensorFlow Lite model saved at {tflite_model_path}")
