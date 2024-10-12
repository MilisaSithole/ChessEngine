#ifndef RESNET_H
#define RESNET_H

#include <torch/torch.h>
#include <torch/script.h>
#include <memory>

using namespace std;

class ResNet{
public:
    ResNet();
    torch::jit::IValue getOutput(torch::Tensor input);
    torch::Tensor getPolicy(torch::Tensor input);

private:
    torch::jit::script::Module model;
    string modelPath = "../src/files/ResNet.pt";
};

#endif