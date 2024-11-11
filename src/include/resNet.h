#ifndef RESNET_H
#define RESNET_H

#include <torch/torch.h>
#include <torch/script.h>
#include <memory>

using namespace std;

class ResNet{
public:
    ResNet(int modelVersion = 0);
    torch::jit::IValue getOutput(torch::Tensor input);
    torch::Tensor getPolicy(torch::Tensor input);
    torch::Device getDevice(){return device;};
    int getVersion(){return modelVersion;};

private:
    torch::jit::script::Module model;
    int modelVersion;
    torch::Device device;
};

#endif