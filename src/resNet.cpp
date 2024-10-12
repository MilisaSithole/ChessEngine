#include "include/resNet.h"

ResNet::ResNet(){
    try{
        model = torch::jit::load(modelPath);
        cout << "Model loaded" << endl;

        cout << "Model test: " << endl;
        torch::Tensor input = torch::randn({1, 18, 8, 8});

        auto output = getOutput(input).toTuple();

        torch::Tensor policy = output->elements()[0].toTensor().squeeze();
        float value = output->elements()[1].toTensor().item<float>();

        cout << "Policy shape: " << policy.sizes() << endl;
        cout << "Value: " << value << endl;
    }
    catch(const c10::Error &e){
        cerr << "Error loading model" << e.what() << endl;
    }    
}

torch::jit::IValue ResNet::getOutput(torch::Tensor input){
    return model.forward({input});
}

torch::Tensor ResNet::getPolicy(torch::Tensor input){
    auto output = getOutput(input).toTuple();
    return output->elements()[0].toTensor().squeeze();
}

float ResNet::getValue(torch::Tensor input){
    auto output = getOutput(input).toTuple();
    return output->elements()[1].toTensor().item<float>();
}