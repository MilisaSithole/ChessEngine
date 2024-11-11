import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F
from torch.utils.data import DataLoader, TensorDataset
import numpy as np
import time
import sys

def loadDataset(file):
    inputs = []
    policies = []
    values = []

    with open(file, 'r') as f:
        while True:
            input_line = f.readline().strip()
            if not input_line:
                break
            policy_line = f.readline().strip()
            value_line = f.readline().strip()

            inputs.append(np.array(input_line.split(','), dtype=np.float32).reshape(19, 8, 8))
            policies.append(np.array(policy_line.split(','), dtype=np.float32))
            values.append(float(value_line))

    return np.array(inputs), np.array(policies), np.array(values)

def getTimeSince(startTime):
    elapsedTime = int(time.time() - startTime)

    hrs = elapsedTime // 3600
    mins = (elapsedTime % 3600) // 60
    secs = elapsedTime % 60

    if hrs > 0:
        return f'{hrs}:{mins:02d}:{secs:02d}'
    else:
        return f'{mins:02d}:{secs:02d}'

def traceAndSave(model, modelPath):
    model.eval()
    dummyInput = torch.randn(1, 19, 8, 8)
    tracedModel = torch.jit.trace(model, dummyInput)
    tracedModel.save(modelPath)


# M A I N =========================================================================================
args = {
    'maxEpochs': 25,
    'epsilon': 0.2,
    'batchSize': 32,
    'alpha': 1e-3,
    'minDelta': 0.005,
    'device': 'cuda' if torch.cuda.is_available() else 'cpu',
    'gamesFile': 'src/games/games_1.csv',
    'modelFile': 'src/files/ResNet1.pt'
}

print(f'Using device: {args["device"]}')

gamesFile = args['gamesFile']
inputs, policies, values = loadDataset(gamesFile)
inputs = torch.FloatTensor(inputs)
policies = torch.FloatTensor(policies)
values = torch.FloatTensor(values).unsqueeze(1)

dataset = TensorDataset(inputs, policies, values)
dataloader = DataLoader(dataset, batch_size = args['batchSize'], shuffle = True)

print(f'Inputs shape: {inputs.shape}')
print(f'Policies shape: {policies.shape}')
print(f'Values shape: {values.shape}')

try:
    model = torch.jit.load(args['modelFile'])
    print(f"Model loaded successfully from {args['modelFile']}")
except Exception as e:
    print(f"Error loading model: {e}")
    sys.exit(1)
model.to(args['device'])
model.train()

policyCriterion = nn.CrossEntropyLoss()
valueCriterion = nn.MSELoss()
optimizer = optim.Adam(model.parameters(), lr = args['alpha'])

# Training loop
totalStartTime = time.time()
epoch = 0
prevEpochLoss = 1000
lastEpochLoss = 10
while lastEpochLoss > args['epsilon'] \
    and epoch < args['maxEpochs'] \
    and abs(lastEpochLoss - prevEpochLoss) > args['minDelta']:
    epochLoss = 0
    for batchInputs, batchPolicies, batchValues in dataloader:
        batchInputs = batchInputs.to(args['device'])
        batchPolicies = batchPolicies.to(args['device'])
        batchPolicies = torch.argmax(batchPolicies, 1)
        batchValues = batchValues.to(args['device'])

        # Feed forward 
        policyOut, valueOut = model(batchInputs)

        # Calc losses
        policyLoss = F.cross_entropy(policyOut, batchPolicies)
        valueLoss = F.mse_loss(valueOut, batchValues)
        loss = policyLoss + valueLoss
        # print(f'Loss = {loss.item()}')

        # policyLoss = policyCriterion(policyOut, batchPolicies)
        # valueLoss = valueCriterion(valueOut, batchValues)
        # loss = policyLoss + valueLoss

        # Backprop
        optimizer.zero_grad()
        loss.backward()
        optimizer.step()
    
        epochLoss += loss.item()

    traceAndSave(model, args['modelFile'])

    # Update progress
    epochLoss /= len(dataloader)
    prevEpochLoss = lastEpochLoss
    lastEpochLoss = epochLoss
    epoch += 1
    # print(f'Epoch {epoch}, Loss: {round(epochLoss, 4)} | {getTimeSince(totalStartTime)}', end = '\r', flush = True)
    print(f'Epoch {epoch}, Loss: {round(epochLoss, 4)} | {getTimeSince(totalStartTime)}')
print()
# Save model
model.eval()
dummyInput = torch.randn(1, 19, 8, 8)
tracedModel = torch.jit.trace(model, dummyInput).to('cpu')
tracedModel.save(args['modelFile'])
print('=============================================================')
print(f'# {args["alpha"]} | {epoch} | {args["batchSize"]} | Final loss = {round(epochLoss, 4)} | {getTimeSince(totalStartTime)}')
print('=============================================================')

