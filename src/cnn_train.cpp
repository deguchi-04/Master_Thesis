#include "../include/cnn.h"
#define MLPACK_ENABLE_ANN_SERIALIZATION
#include <stdlib.h>
// defining namespaces

using namespace ens;
using namespace std;
using namespace arma;
using namespace cv;

/******************************/
/*         Parameters        */
/****************************/

//! Path to the dataset used for training and testing.
const string datasetPath = "/home/thaidy/Documents/grape_trunk_dataset_split";
const string datasetPathTrain = "/home/thaidy/Documents/grape_trunk_dataset_split/train";
const string datasetPathTest = "/home/thaidy/Documents/grape_trunk_dataset_split/test";
const string datasetPathVal = "/home/thaidy/Documents/grape_trunk_dataset_split/val";

const string datasetPathTrainCSV = "/home/thaidy/Documents/grape_trunk_dataset_split/image_train_labels.csv";
const string datasetPathTestCSV = "/home/thaidy/Documents/grape_trunk_dataset_split/image_test_labels.csv";
const string modelFile = "model.bin"; // File for saving the model.

constexpr int H1 = 64;  //! - H1: The number of neurons in the 1st layer.
constexpr int H2 = 128; //! - H2: The number of neurons in the 2nd layer.
constexpr int H3 = 64;  //! - H3: The number of neurons in the 3rd layer.

const int EPOCHS = 10; // Number of epochs for training.

constexpr double STEP_SIZE = 5e-2;      //! - STEP_SIZE: Step size of the optimizer.
constexpr int BATCH_SIZE = 50;          //! - BATCH_SIZE: Number of data points in each iteration of SGD.
constexpr double STOP_TOLERANCE = 1e-8; //! - STOP_TOLERANCE: Stop tolerance; A very small number implies that we do all iterations.

// NOTE: Training the model may take a long time, therefore once it is trained you can set this to false and use the model for prediction.
// NOTE: There is no error checking in this example to see if the trained model exists!
const bool bTrain = true;         // If true, the model will be trained; if false, the saved model will be read and used for prediction
const bool bLoadAndTrain = false; // You can load and further train a model by setting this to true.

/*********************************************/
/*Function to calculate MSE for arma::cube. */
/********************************************/
double ComputeMSE(mat &pred, mat &Y)
{
    return mlpack::SquaredEuclideanDistance::Evaluate(pred, Y) / (Y.n_elem);
}

int main()
{
    arma::mat dataset_train, dataset_test;
    cv::Mat img_val, img_test, img_train;
    vector<cv::Mat> vecimg_val, vecimg_test, vecimg_train;
    vector<String> fn_val, fn_test, fn_train;
    int count_train = 0, count_test = 0, count_val = 0;

    // cv::Mat *vecimg_val, *vecimg_test, *vecimg_train;

    cv::glob("../../grape_trunk_dataset_split/val/*.jpg", fn_val);
    cv::glob("../../grape_trunk_dataset_split/test/*.jpg", fn_test);
    cv::glob("../../grape_trunk_dataset_split/train/*.jpg", fn_train);

    cout << "train " << fn_train.size() << endl; // to display no of files
    cout << "test " << fn_test.size() << endl;   // to display no of files
    cout << "val " << fn_val.size() << endl;     // to display no of files

    // In Armadillo rows represent features, columns represent data points.
    cout << "Reading data." << endl;
    bool loadedDatasetTrain = mlpack::data::Load(datasetPathTrain, dataset_train, true);
    bool loadedDatasetTest = mlpack::data::Load(datasetPathTest, dataset_test, true);
    // If dataset is not loaded correctly, exit.
    if (!loadedDatasetTrain || !loadedDatasetTest)
        return -1;

    // Split the dataset into training and validation sets.
    arma::mat trainData, validData;
    trainData = loadedDatasetTrain;
    validData = loadedDatasetTest;

    // The train and valid datasets contain both - the features as well as the
    // prediction. Split these into separate matrices.
    arma::mat trainX =
        trainData.submat(1, 0, trainData.n_rows - 1, trainData.n_cols - 1);
    arma::mat validX =
        validData.submat(1, 0, validData.n_rows - 1, validData.n_cols - 1);

    // Create prediction data for training and validatiion datasets.
    arma::mat trainY = trainData.row(0);
    arma::mat validY = validData.row(0);

    // Scale all data into the range (0, 1) for increased numerical stability.
    mlpack::data::MinMaxScaler scaleX;
    // Scaler for predictions.
    mlpack::data::MinMaxScaler scaleY;
    // Fit scaler only on training data.
    scaleX.Fit(trainX);
    scaleX.Transform(trainX, trainX);
    scaleX.Transform(validX, validX);

    // Scale training predictions.
    scaleY.Fit(trainY);
    scaleY.Transform(trainY, trainY);
    scaleY.Transform(validY, validY);

    // Only train the model if required.
    if (bTrain || bLoadAndTrain)
    {
        // Specifying the NN model.
        mlpack::FFN<mlpack::MeanSquaredError, mlpack::HeInitialization> model;
        if (bLoadAndTrain)
        {
            // The model will be trained further.
            cout << "Loading and further training the model." << endl;
            mlpack::data::Load(modelFile, "NNRegressor", model);
        }
        else
        {
            // This intermediate layer is needed for connection between input
            // data and the next LeakyReLU layer.
            // Parameters specify the number of neurons in the next layer.
            model.Add<mlpack::Linear>(H1);
            // Activation layer:
            model.Add<mlpack::LeakyReLU>();
            // Connection layer between two activation layers.
            model.Add<mlpack::Linear>(H2);
            // Activation layer.
            model.Add<mlpack::LeakyReLU>();
            // Connection layer.
            model.Add<mlpack::Linear>(H3);
            // Activation layer.
            model.Add<mlpack::LeakyReLU>();
            // Connection layer => output.
            // The output of one neuron is the regression output for one record.
            model.Add<mlpack::Linear>(1);
        }

        // Set parameters for the Stochastic Gradient Descent (SGD) optimizer.
        ens::Adam optimizer(
            STEP_SIZE,                 // Step size of the optimizer.
            BATCH_SIZE,                // Batch size. Number of data points that are used in each
                                       // iteration.
            0.9,                       // Exponential decay rate for the first moment estimates.
            0.999,                     // Exponential decay rate for the weighted infinity norm
                                       // estimates.
            1e-8,                      // Value used to initialise the mean squared gradient parameter.
            trainData.n_cols * EPOCHS, // Max number of iterations.
            STOP_TOLERANCE,            // Tolerance.
            true);

        model.Train(trainX,
                    trainY,
                    optimizer,
                    // PrintLoss Callback prints loss for each epoch.
                    ens::PrintLoss(),
                    // Progressbar Callback prints progress bar for each epoch.
                    ens::ProgressBar(),
                    // Stops the optimization process if the loss stops decreasing
                    // or no improvement has been made. This will terminate the
                    // optimization once we obtain a minima on training set.
                    ens::EarlyStopAtMinLoss(20));

        cout << "Finished training. \nSaving Model" << endl;
        mlpack::data::Save(modelFile, "NNRegressor", model);
        cout << "Model saved in " << modelFile << endl;
    }

    // NOTE: the code below is added in order to show how in a real application
    // the model would be saved, loaded and then used for prediction.
    // The following steps will be performed after normalizing the dataset.
    mlpack::FFN<mlpack::MeanSquaredError, mlpack::HeInitialization> modelP;
    // Load weights into the model.
    mlpack::data::Load(modelFile, "NNRegressor", modelP);

    // Create predictions on the dataset.
    arma::mat predOut;
    modelP.Predict(validX, predOut);

    // We will test the quality of our model by calculating Mean Squared Error on
    // validation dataset.
    double validMSE = ComputeMSE(validY, predOut);
    cout << "Mean Squared Error on Prediction data points: " << validMSE << endl;

    // To get meaningful predictions we need to undo the scaling operation on
    // predictions.
    scaleY.InverseTransform(predOut, predOut);
    // Save the prediction results.
    bool saved = mlpack::data::Save("results.csv", predOut, true);

    if (!saved)
        cout << "Results have not been saved." << endl;

    return 0;
}
