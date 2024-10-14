#include "include/resNet.h"

ResNet::ResNet(){
    try{
        model = torch::jit::load(modelPath);
    }
    catch(const c10::Error &e){
        cerr << "Error loading model" << e.what() << endl;
        throw;
    }    
}

torch::jit::IValue ResNet::getOutput(torch::Tensor input){
    return model.forward({input});
}

torch::Tensor ResNet::getPolicy(torch::Tensor input){
    auto output = getOutput(input).toTuple();
    return output->elements()[0].toTensor().squeeze();
}