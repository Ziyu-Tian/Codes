import psutil
import os
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.preprocessing import OneHotEncoder
from keras.datasets import mnist
import time
import ssl
ssl._create_default_https_context = ssl._create_unverified_context


# Sigmoid activation function
def sigmoid(z):
    return 1.0 / (1.0 + np.exp(-z))

def sigmoid_derivative(z):
    return sigmoid(z) * (1 - sigmoid(z))

# Load the MNIST dataset
(X_train, y_train), (X_test, y_test) = mnist.load_data()

start_time = time.time()

# Flatten the images
X_train_flatten = X_train.reshape(X_train.shape[0], -1)
X_test_flatten = X_test.reshape(X_test.shape[0], -1)

# Normalize pixel values
X_train_normalized = X_train_flatten / 255.0
X_test_normalized = X_test_flatten / 255.0

# One-hot encode the labels
encoder = OneHotEncoder(sparse=False)
y_train_onehot = encoder.fit_transform(y_train.reshape(-1, 1))
y_test_onehot = encoder.transform(y_test.reshape(-1, 1))

# Initialize weights and biases
input_size = X_train_normalized.shape[1]
output_size = y_train_onehot.shape[1]
hidden_neurons = 60

weights_input_hidden = np.random.randn(input_size, hidden_neurons)
bias_hidden = np.zeros(hidden_neurons)
weights_hidden_output = np.random.randn(hidden_neurons, output_size)
bias_output = np.zeros(output_size)

# Hyperparameters
learning_rate = 0.20
epochs = 10

# Training using stochastic gradient descent
for epoch in range(epochs):
    correct_train = 0
    for x, y in zip(X_train_normalized, y_train_onehot):
        # Forward pass - hidden layer
        z_hidden = np.dot(x, weights_input_hidden) + bias_hidden
        a_hidden = sigmoid(z_hidden)
        
        # Forward pass - output layer
        z_output = np.dot(a_hidden, weights_hidden_output) + bias_output
        a_output = sigmoid(z_output)
        
        # Backward pass
        delta_output = (a_output - y) * sigmoid_derivative(z_output)
        grad_weights_hidden_output = np.outer(a_hidden, delta_output)
        grad_bias_output = delta_output
        delta_hidden = np.dot(delta_output, weights_hidden_output.T) * sigmoid_derivative(z_hidden)
        grad_weights_input_hidden = np.outer(x, delta_hidden)
        grad_bias_hidden = delta_hidden
        
        # Update weights and biases
        weights_hidden_output -= learning_rate * grad_weights_hidden_output
        bias_output -= learning_rate * grad_bias_output
        weights_input_hidden -= learning_rate * grad_weights_input_hidden
        bias_hidden -= learning_rate * grad_bias_hidden
        
        # Calculate accuracy for this training example
        predicted_class = np.argmax(a_output)
        true_class = np.argmax(y)
        if predicted_class == true_class:
            correct_train += 1
    
    train_accuracy = correct_train / len(y_train_onehot)
    print(f"Epoch {epoch+1}/{epochs} - Training accuracy: {train_accuracy:.2f}")
    
end_time = time.time()

training_time = end_time - start_time

start_time = time.time()

print(f"Training Time: {training_time:.4f} s")

# Evaluate on test data
correct_test = 0
for x, y in zip(X_test_normalized, y_test_onehot):
    # Forward pass - hidden layer
    z_hidden = np.dot(x, weights_input_hidden) + bias_hidden
    a_hidden = sigmoid(z_hidden)

    # Forward pass - output layer
    z_output = np.dot(a_hidden, weights_hidden_output) + bias_output
    a_output = sigmoid(z_output)
    
    predicted_class = np.argmax(a_output)
    true_class = np.argmax(y)
    if predicted_class == true_class:
        correct_test += 1

test_accuracy = correct_test / len(y_test_onehot)
print(f"Test accuracy: {test_accuracy:.2f}")


end_time = time.time()
testing_time = end_time - start_time
print(f"Testing Time: {testing_time:.4f} s")

# Get the process PID 
pid = os.getpid()

# Get object of this process
process = psutil.Process(pid)

# Get the Python process
memory_info = process.memory_info()

def format_memory_size(size):
    for unit in ['', 'K', 'M', 'G', 'T']:
        if size < 1024:
            return f"{size:.2f} {unit}B"
        size /= 1024
    return f"{size:.2f} PB"

print(format_memory_size(memory_info.rss))
