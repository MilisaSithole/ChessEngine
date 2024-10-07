#ifndef RESNET_H
#define RESNET_H

#include <torch/torch.h>
#include <torch/script.h>

#include <memory>
#include <vector>

using namespace std;

class ResNet{
public:
    ResNet();

private:
    torch::jit::script::Module model;
    string modelPath = "../src/files/ResNet.pt";
};

#endif