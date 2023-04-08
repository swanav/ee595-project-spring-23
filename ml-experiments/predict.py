import os
import pandas as pd
import pickle

from sklearn.model_selection import train_test_split
from sklearn.tree import DecisionTreeClassifier, export_text, plot_tree
# from sklearn_porter import Porter

from keras.models import Sequential, load_model as load_keras_model
from keras.layers import Dense
from keras.optimizers import Adam
from keras.losses import binary_crossentropy
from keras.metrics import binary_accuracy
from keras.callbacks import EarlyStopping, ModelCheckpoint, CSVLogger, ReduceLROnPlateau, TensorBoard, LearningRateScheduler, TerminateOnNaN, LambdaCallback, ProgbarLogger


model_dnn_name = 'model.h5'
model_dt_name = 'decision_tree.pkl'

def load_dnn_model():
    # Load the keras .h5 model from the file
    model = load_keras_model(model_dnn_name)
    return model

def build_dnn_model(X_train, y_train):
    # Create a Deep Neural Network model with 2 hidden layers
    # with 64 neurons each and an output layer with 1 neuron
    # The input layer has 30 neurons

    model = Sequential()
    model.add(Dense(128, input_dim=99, activation='relu'))
    # The activation function for the hidden layers is ReLU
    model.add(Dense(128, activation='relu'))
    # The activation function for the output layer is Sigmoid
    model.add(Dense(1, activation='sigmoid'))

    # The loss function is binary crossentropy
    # The optimizer is Adam
    # The metrics are accuracy
    model.compile(loss=binary_crossentropy, optimizer=Adam(learning_rate=0.001), metrics=[binary_accuracy])

    # The learning rate is 0.001
    # The number of epochs is 100
    # The batch size is 32
    # The class weight is None
    # The initial epoch is 0
    # The validation split is 0.2
    model.fit(X_train, y_train, shuffle=True, epochs=50, batch_size=32, validation_split=0.2)

    # Save the model
    model.save(model_dnn_name)

    return model

def load_dt_model() -> DecisionTreeClassifier:
    with open(model_dt_name, 'rb') as f:
        dt = pickle.load(f)
    return dt

def build_dt_model(X_train, y_train) -> DecisionTreeClassifier:
    # Create a desicion tree model
    dt = DecisionTreeClassifier()
    dt.fit(X_train, y_train)

    # Print the decision tree
    plot_tree(dt, filled=True, max_depth=3)

    # Save the model
    with open('decision_tree.pkl', 'wb') as f:
        pickle.dump(dt, f)

    r = export_text(dt)

    # porter = Porter(dt, language='c++')
    # output = porter.export(embed_data=True)

    # Write the C++ code to a file
    with open('decision_tree.txt', 'w') as f:
        f.write(r)

    return dt

def main():

    # read data from dataset.csv
    df = pd.read_csv('dataset.csv', delimiter=' ', header='infer')
    # print the first 5 rows of the data
    print(df.head())

    # Split the data into features and labels
    X = df.drop('RESULTAT', axis=1)
    y = df['RESULTAT']

    print(X.head())
    print(y.head())

    # Convert the RESULTAT column to 0 and 1
    y = y.map({'A': 0, 'C': 1})

    # Split the data into training and test sets
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2)

    # if os.path.exists(model_dnn_name):
    #     # If file exists, load the model
    #     model = load_dnn_model()
    # else:
    #     # Otherwise, train the model
    #     model = build_dnn_model(X_train, y_train)

    if not os.path.exists(model_dt_name):
        # If file exists, load the model
        model = load_dt_model()
    else:
        # Otherwise, train the model
        model = build_dt_model(X_train, y_train)


    # Predict the test set
    y_pred = model.predict(X_test)

    # Set the threshold to 0.5
    threshold = 0.8

    # Print the first 5 predictions
    y_pred = [1 if y >= threshold else 0 for y in y_pred]
    print(y_pred[:5])

    # Print the first 5 actual values
    print(y_test[:5].values)

    # Print the accuracy
    print(f'Accuracy: {(sum(y_pred == y_test) / len(y_test) * 100):.2f}%')


if __name__ == '__main__':
    main()