import pandas as pd
import numpy as np
import random

"""
class Network(object):
    # 'sizes' is a list as [2,3,1] with the num of each layers neuron
    def __init__(self, sizes):
        self.num_layers = len(sizes)  # num of the layers
        self.sizes = sizes
        self.biases = [np.random.randn(y, 1) for y in sizes[1:]]
        # randn(y,1) produce a y * 1 vector; 'size[1:]' means the list without first element [3,1]
        # If list is [2,3,4], the biases matrix would be two random vector: 3 * 1 biases[0] and 1 * 1 biases[1]
        self.weights = [np.random.randn(y, x)
                        for x, y in zip(sizes[:-1], sizes[1:])]
        # zip([1,2],[3,4])) is [(1,3),(2,4)]; [:-1] is the list without last element i.e. [2,3]
        # zip([2,3],[3,1]) = [(2,3),(3,1)], so weight[0] is 3 * 2 matrix, weight[1] is 1 *3 matrix

    def SGD(self, training_data, epochs, mini_batch_size, eta,
            test_data=None):
        if test_data:
            n_test = len(test_data)
        n = len(training_data)
        for j in range(epochs):
            random.shuffle(training_data)
            mini_batches = [
                training_data[k:k + mini_batch_size]
                for k in range(0, n, mini_batch_size)]  # separate training data into small batches
            for mini_batch in mini_batches:
                self.update_mini_batch(mini_batch, eta)
            if test_data:
                print('Epoch {0}:{1}/{2}'.format(
                    j, self.evaluate(test_data), n_test))
            else:
                print('Epoch {0} complete'.format(j))

    def update_mini_batch(self, mini_batch, eta):

        nabla_b = [np.zeros(b.shape) for b in self.biases]
        nabla_w = [np.zeros(w.shape) for w in self.weights]
        for x, y in mini_batch:
            delta_nabla_b, delta_nabla_w = self.backprop(x, y)
            nabla_b = [nb+dnb for nb, dnb in zip(nabla_b, delta_nabla_b)]
            nabla_w = [nw+dnw for nw, dnw in zip(nabla_w, delta_nabla_w)]
        self.weights = [w-(eta/len(mini_batch))*nw
                        for w, nw in zip(self.weights, nabla_w)]
        self.biases = [b-(eta/len(mini_batch))*nb
                       for b, nb in zip(self.biases, nabla_b)]
        
    def backprop(self, x, y):
        
        nabla_b = [np.zeros(b.shape) for b in self.biases]
        nabla_w = [np.zeros(w.shape) for w in self.weights]
        activation = x
        activations = [x] # list to store all the activations, layer by layer 
        zs = [] # list to store all the z vectors, layer by layer
        for b, w in zip(self.biases, self.weights):
            z = np.dot(w, activation)+b
            zs.append(z)
            activation = sigmoid(z)
            activations.append(activation)
        
"""