#include "include/resNet.h"

ResNet::ResNet(int modelVersion)
    : modelVersion(modelVersion), device(torch::kCPU){
    
    string modelPath = "../src/files/ResNet" + to_string(modelVersion) + ".pt";
    try{
        model = torch::jit::load(modelPath);

        if(torch::cuda::is_available()){
            device = torch::kCUDA;
        }
        else{
            device = torch::kCPU;
        }

        model.to(device);
    }
    catch(const c10::Error &e){
        cerr << "Error loading model" << e.what() << endl;
        throw;
    }    
}

torch::jit::IValue ResNet::getOutput(torch::Tensor input){
    try {
        input = input.to(device);
        auto output = model.forward({input});
        return output;
    } catch (const c10::Error& e) {
        std::cerr << "Error during model execution: " << e.what() << std::endl;
        std::cerr << "Input tensor shape: " << input.sizes() << std::endl;
        std::cerr << "Input tensor device: " << input.device() << std::endl;
        std::cerr << "Model device: " << device << std::endl;
        throw;
    }
}

torch::Tensor ResNet::getPolicy(torch::Tensor input){
    auto output = getOutput(input).toTuple();
    auto policy = output->elements()[0].toTensor().squeeze();
    policy = torch::nn::functional::softmax(policy, 0);
    return policy;
}