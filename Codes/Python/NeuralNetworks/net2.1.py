import psutil
import os
import numpy as np
from sklearn.datasets import load_iris
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.preprocessing import OneHotEncoder

# Sigmoid activation function
def sigmoid(z):
    return 1.0 / (1.0 + np.exp(-z))

# Derivative of the sigmoid function
def sigmoid_derivative(z):
    return sigmoid(z) * (1 - sigmoid(z))

# Load the Iris dataset
iris = load_iris()
X = iris.data  # Features
y = iris.target  # Labels

# Split the data into training and test sets
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# Standardize the features
scaler = StandardScaler()
X_train_standardized = scaler.fit_transform(X_train)
X_test_standardized = scaler.transform(X_test)

# One-hot encode the labels
encoder = OneHotEncoder(sparse=False)
y_train_onehot = encoder.fit_transform(y_train.reshape(-1, 1))
y_test_onehot = encoder.transform(y_test.reshape(-1, 1))

# Initialize weights and biases
input_size = X_train_standardized.shape[1]
output_size = y_train_onehot.shape[1]

# Set the number of hidden neurons
hidden_neurons = 5

weights_input_hidden = np.random.randn(input_size, hidden_neurons)
bias_hidden = np.zeros(hidden_neurons)
weights_hidden_output = np.random.randn(hidden_neurons, output_size)
bias_output = np.zeros(output_size)

# Hyperparameters
learning_rate = 0.2
epochs = 30

# Training using stochastic gradient descent
for epoch in range(epochs):
    correct_train = 0
    for x, y in zip(X_train_standardized, y_train_onehot):
        # Forward pass - hidden layer
        z_hidden = np.dot(x, weights_input_hidden) + bias_hidden
        a_hidden = sigmoid(z_hidden)
        
        # Forward pass - output layer
        z_output = np.dot(a_hidden, weights_hidden_output) + bias_output
        a_output = sigmoid(z_output)
        
        # Backward pass
        delta_output = (a_output - y) * sigmoid_derivative(z_output)
        #print(delta_output)
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

# Evaluate on test data
correct_test = 0
for x, y in zip(X_test_standardized, y_test_onehot):
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



# Get the process PID 
pid = os.getpid()

# Get object of this process
process = psutil.Process(pid)

# Get the Python process
memory_info = process.memory_info()

def format_memory_size(size):
    # Choose unit base on size
    for unit in ['', 'K', 'M', 'G', 'T']:
        if size < 1024:
            return f"{size:.2f} {unit}B"
        size /= 1024
    return f"{size:.2f} PB"

print(format_memory_size(memory_info.rss))
