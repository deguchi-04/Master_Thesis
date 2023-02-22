#include "../include/cnn.h"
#define MLPACK_ENABLE_ANN_SERIALIZATION
#include <stdlib.h>
// defining namespaces

int main()
{
    arma::mat dataset_train, dataset_test;
    cv::Mat img_val, img_test, img_train;
    vector<cv::Mat> *vecimg_val, *vecimg_test, *vecimg_train;
    vector<String> fn_val, fn_test, fn_train;
    int count_train = 0, count_test = 0, count_val = 0;

    //cv::Mat *vecimg_val, *vecimg_test, *vecimg_train;


    cv::glob("../../grape_trunk_dataset_split/val/*.jpg", fn_val);
    cv::glob("../../grape_trunk_dataset_split/test/*.jpg", fn_test);
    cv::glob("../../grape_trunk_dataset_split/train/*.jpg", fn_train);

    cout << "train " << fn_train.size() << endl; // to display no of files
    cout << "test " << fn_test.size() << endl;   // to display no of files
    cout << "val " << fn_val.size() << endl;     // to display no of files

    vecimg_train = (vector<cv::Mat>*)malloc(fn_train.size() + 1);
    vecimg_test = (vector<cv::Mat>*)malloc(fn_test.size() + 1);
    vecimg_val = (vector<cv::Mat>*)malloc(fn_val.size() + 1);

    // Img train dataset read
    cout << "Reading train data..." << endl;
    for (size_t i = 0; i < fn_train.size(); ++i)
    {
        // cout << fn_train[i] << endl;
        img_train = cv::imread(fn_train[i], cv::COLOR_BGR2HSV);
        if (img_train.empty())
        {
            std::cout << "Could not read the image: " << fn_train[i] << std::endl;
            return 1;
        }
        vecimg_train->push_back(img_train);
        cout << count_train << endl;
        count_train += 1;
    }

    // Img test dataset read
    cout << "Reading test data..." << endl;
    for (size_t i = 0; i < fn_test.size(); ++i)
    {
        // cout << fn_test[i] << endl;
        img_test = cv::imread(fn_test[i], cv::COLOR_BGR2HSV);
        if (img_test.empty())
        {
            std::cout << "Could not read the image: " << fn_test[i] << std::endl;
            return 1;
        }
        vecimg_test->push_back(img_test);
        // cout << count_test << endl;
        count_test += 1;
    }

    // Img validation dataset read
    cout << "Reading val data..." << endl;
    for (size_t i = 0; i < fn_val.size(); ++i)
    {
        // cout << fn_val[i] << endl;
        img_val = cv::imread(fn_val[i], cv::COLOR_BGR2HSV);
        if (img_val.empty())
        {
            std::cout << "Could not read the image: " << fn_val[i] << std::endl;
            return 1;
        }
        vecimg_val->push_back(img_val);
        // cout << count_val << endl;
        count_val += 1;
    }

    cout << "Total imgs: " << count_val + count_test + count_train << endl;

    // // In Armadillo rows represent features, columns represent data points.
    // cout << "Reading data." << endl;
    //  bool loadedDatasetTrain = mlpack::data::Load(datasetPathTrain, dataset_train, true);
    //  bool loadedDatasetTest = mlpack::data::Load(datasetPathTest, dataset_test, true);
    //  // If dataset is not loaded correctly, exit.
    //  if (!loadedDatasetTrain || !loadedDatasetTest)
    //      return -1;

    //  // Split the dataset into training and validation sets.
    //  arma::mat trainData, validData;
    //  trainData = loadedDatasetTrain;
    //  validData = loadedDatasetTest;

    //  // The train and valid datasets contain both - the features as well as the
    //  // prediction. Split these into separate matrices.
    //  arma::mat trainX =
    //      trainData.submat(1, 0, trainData.n_rows - 1, trainData.n_cols - 1);
    //  arma::mat validX =
    //      validData.submat(1, 0, validData.n_rows - 1, validData.n_cols - 1);

    //  // Create prediction data for training and validatiion datasets.
    //  arma::mat trainY = trainData.row(0);
    //  arma::mat validY = validData.row(0);

    //  // Scale all data into the range (0, 1) for increased numerical stability.
    //  data::MinMaxScaler scaleX;
    //  // Scaler for predictions.
    //  data::MinMaxScaler scaleY;
    //  // Fit scaler only on training data.
    //  scaleX.Fit(trainX);
    //  scaleX.Transform(trainX, trainX);
    //  scaleX.Transform(validX, validX);

    //  // Scale training predictions.
    //  scaleY.Fit(trainY);
    //  scaleY.Transform(trainY, trainY);
    //  scaleY.Transform(validY, validY);

    //  // Only train the model if required.
    //  if (bTrain || bLoadAndTrain)
    //  {
    //      // Specifying the NN model.
    //      FFN<MeanSquaredError, HeInitialization> model;
    //      if (bLoadAndTrain)
    //      {
    //          // The model will be trained further.
    //          cout << "Loading and further training the model." << endl;
    //          data::Load(modelFile, "NNRegressor", model);
    //      }
    //      else
    //      {
    //          // This intermediate layer is needed for connection between input
    //          // data and the next LeakyReLU layer.
    //          // Parameters specify the number of neurons in the next layer.
    //          model.Add<Linear>(H1);
    //          // Activation layer:
    //          model.Add<LeakyReLU>();
    //          // Connection layer between two activation layers.
    //          model.Add<Linear>(H2);
    //          // Activation layer.
    //          model.Add<LeakyReLU>();
    //          // Connection layer.
    //          model.Add<Linear>(H3);
    //          // Activation layer.
    //          model.Add<LeakyReLU>();
    //          // Connection layer => output.
    //          // The output of one neuron is the regression output for one record.
    //          model.Add<Linear>(1);
    //      }

    //      // Set parameters for the Stochastic Gradient Descent (SGD) optimizer.
    //      ens::Adam optimizer(
    //          STEP_SIZE,                 // Step size of the optimizer.
    //          BATCH_SIZE,                // Batch size. Number of data points that are used in each
    //                                     // iteration.
    //          0.9,                       // Exponential decay rate for the first moment estimates.
    //          0.999,                     // Exponential decay rate for the weighted infinity norm
    //                                     // estimates.
    //          1e-8,                      // Value used to initialise the mean squared gradient parameter.
    //          trainData.n_cols * EPOCHS, // Max number of iterations.
    //          STOP_TOLERANCE,            // Tolerance.
    //          true);

    //      model.Train(trainX,
    //                  trainY,
    //                  optimizer,
    //                  // PrintLoss Callback prints loss for each epoch.
    //                  ens::PrintLoss(),
    //                  // Progressbar Callback prints progress bar for each epoch.
    //                  ens::ProgressBar(),
    //                  // Stops the optimization process if the loss stops decreasing
    //                  // or no improvement has been made. This will terminate the
    //                  // optimization once we obtain a minima on training set.
    //                  ens::EarlyStopAtMinLoss(20));

    //      cout << "Finished training. \nSaving Model" << endl;
    //      data::Save(modelFile, "NNRegressor", model);
    //      cout << "Model saved in " << modelFile << endl;
    //  }

    //  // NOTE: the code below is added in order to show how in a real application
    //  // the model would be saved, loaded and then used for prediction.
    //  // The following steps will be performed after normalizing the dataset.
    //  FFN<MeanSquaredError, HeInitialization> modelP;
    //  // Load weights into the model.
    //  data::Load(modelFile, "NNRegressor", modelP);

    //  // Create predictions on the dataset.
    //  arma::mat predOut;
    //  modelP.Predict(validX, predOut);

    //  // We will test the quality of our model by calculating Mean Squared Error on
    //  // validation dataset.
    //  double validMSE = ComputeMSE(validY, predOut);
    //  cout << "Mean Squared Error on Prediction data points: " << validMSE << endl;

    //  // To get meaningful predictions we need to undo the scaling operation on
    //  // predictions.
    //  scaleY.InverseTransform(predOut, predOut);
    //  // Save the prediction results.
    //  bool saved = data::Save("results.csv", predOut, true);

    //  if (!saved)
    //      cout << "Results have not been saved." << endl;


    
    free(vecimg_train);
    free(vecimg_test);
    free(vecimg_val);
    return 0;
}
