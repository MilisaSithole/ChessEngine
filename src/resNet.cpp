#include "include/resNet.h"

ResNet::ResNet(){
    try{
        model = torch::jit::load(modelPath);
        cout << "Model loaded" << endl;
    }
    catch(const c10::Error &e){
        cerr << "Error loading model" << endl;
    }

    cout << "Model test: " << endl;
    torch::Tensor input = torch::randn({1, 18, 8, 8});
    vector<torch::jit::IValue> inputs;
    inputs.push_back(input);
    auto output = model.forward(inputs);
    cout << output << endl;
}