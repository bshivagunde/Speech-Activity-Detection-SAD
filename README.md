# Custom Speech Activity Detection (SAD) Machine Learning Model

## Overview
This repository contains a custom Speech Activity Detection (SAD) machine learning model designed to classify audio clips into two categories: "SPEECH" and "NO_SPEECH". The model has been trained and optimized for deployment on edge devices, balancing accuracy and performance efficiency.

---

## Data
The dataset is composed of 1-second audio clips categorized into:
- **NO_SPEECH**: 943 files located in `Data/No_Speech.rar`
- **SPEECH**: 942 files located in `Data/Speech.rar`

---

## Neural Network Architecture
The model processes 650 input features and consists of the following layers:
1. **Input Layer**: Handles the 650 features.
2. **Reshape Layer**: Reshapes the input into 13 columns.
3. **Two 1D Convolutional Layers**: Includes pooling layers for feature reduction and dropout layers to prevent overfitting.
4. **Flatten Layer**: Converts the 1D outputs into a vector.
5. **Dense Layer**: Fully connected layer for feature integration.
6. **Output Layer**: Predicts two classes (SPEECH and NO_SPEECH).

![Neural Network Architecture](Images/Neural%20Network%20Architecture.PNG)

---

## Training Performance
The model's performance on the validation set:
- **Accuracy**: 91.7%
- **Loss**: 0.20 (indicating good convergence)

![Training Performance](Images/Training%20Performance.PNG)

---

## Expected On-device Performance (Training)
Performance metrics comparing Quantized (int8) and Unoptimized (float32) models for edge deployment:

| Metric               | Quantized (int8)       | Unoptimized (float32) |
|----------------------|------------------------|------------------------|
| **Inferencing Time** | 3 ms (classification) | 22 ms (classification) |
| **Peak RAM Usage**   | 3.8 KB                | 7 KB                  |
| **Flash Usage**      | 31.6 KB               | 27.2 KB               |
| **Accuracy**         | 90.03%                | 90.29%                |

![Expected On-device Performance](Images/Expected%20On-device%20Performance%20(training).PNG)

---

## Testing Performance
The model's performance on the test set:
- **Accuracy**: 90.29%
- **AUC (Area Under Curve)**: 0.93
- **Weighted Metrics**:
  - Precision: 0.93
  - Recall: 0.93
  - F1-Score: 0.93
- **Confusion Matrix**: Indicates accurate classification of both "SPEECH" and "NO_SPEECH" with minimal misclassification.

![Testing Performance](Images/Testing%20Performance.PNG)

---

## Model Optimization
Quantization significantly improves the model's performance on edge devices:
- **Latency**: Quantized models reduce classification latency (3 ms vs. 22 ms).
- **Memory Usage**:
  - RAM: Quantized models require less RAM (3.8 KB vs. 7 KB for classification).
  - Flash: Quantized models occupy slightly more flash memory (31.6 KB vs. 27.2 KB).
- **Accuracy**: Minimal accuracy difference between quantized (90.03%) and unoptimized (90.29%).

![Model Optimization](Images/Model%20Optimization.PNG)

---

## Conclusion
This custom SAD model demonstrates strong performance metrics and is optimized for efficient deployment on edge devices. Quantization ensures reduced latency and memory usage, making it ideal for resource-constrained environments.


## Contact
Created by [@bshivagunde](https://github.com/bshivagunde) - Feel free to contact me!